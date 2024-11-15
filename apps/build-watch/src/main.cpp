#include "BuildWatch/BuildWatchTask.hpp"
#include "BuildWatch/ConfigReader.hpp"
#include "spdlog/async.h"

#include <CLI/CLI.hpp>
#include <cpptrace/cpptrace.hpp>
#include <cpptrace/from_current.hpp>
#include <csignal>
#include <fmt/std.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include "git.h"
#include "info.hpp"

namespace {
std::atomic_bool quit = false;
}

void signalHandler(const int signal)
{
    if (!quit) {
        spdlog::info("Received signal {}", signal);
    }
    quit = true;
}

void setUpLogging(const int debugLevel, std::string const& logPath)
{
    if (logPath.empty()) {
        if (debugLevel == 1) {
            spdlog::set_level(spdlog::level::debug);
        } else if (debugLevel == 2) {
            spdlog::set_level(spdlog::level::trace);
        }
        return;
    }

    const auto ptrConsoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    ptrConsoleSink->set_level(spdlog::level::info);

    const auto ptrFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logPath, true);
    // https://github.com/gabime/spdlog/wiki/3.-Custom-formatting
    ptrFileSink->set_pattern("[%Y-%m-%d %H:%M:%S.%F %z] [%l] [tid %t] %v");

    if (debugLevel == 1) {
        ptrFileSink->set_level(spdlog::level::debug);
    } else if (debugLevel == 2) {
        ptrFileSink->set_level(spdlog::level::trace);
    } else {
        ptrFileSink->set_level(spdlog::level::info);
    }

    spdlog::sinks_init_list sinks{ptrFileSink, ptrConsoleSink};
    // Default replacement logger
    auto logger = std::make_shared<spdlog::logger>("default", sinks);
    logger->flush_on(spdlog::level::info);
    spdlog::set_default_logger(logger);
}

std::filesystem::path getRootPath(std::string const& rootPath)
{
    if (rootPath.empty()) {
        spdlog::info("Watching current path: {}", std::filesystem::current_path());
        return std::filesystem::current_path();
    }

    spdlog::info("Watching root path: {}", rootPath);
    return {rootPath};
}

int main(int argc, char** argv)
{
    CLI::App app;
    std::stringstream ss;
    ss << "build-watch: watch files and regenerate build files\n\n"
       << "Built  : " __DATE__ " " __TIME__ "\n"
       << "Version: " << btl::info::PROJECT_VERSION << "\n"
       << "hash   : " << git::CommitSHA1() << "\n\n"
       << "If on your path, to run from anywhere in your source tree try:\n\n"
       << "     alias bw='build-watch $(git rev-parse --show-toplevel)'" << "\n\n"
       << "Run with `-g` to get a default config file, and copy that to `.config/BuildWatch/config.json`\n\n";

    app.description(ss.str());

    std::string root;
    app.add_option("root", root, "root directory to watch, otherwise $(pwd)")->capture_default_str();

    int debug{0};
    app.add_flag("-d,--debug", debug, "debug info, twice for trace");

    bool dryRun{false};
    app.add_flag("--dry-run", dryRun, "Write generated files to stdout, not to disk");

    bool generateConfig{false};
    app.add_flag("-g,--generateConfig", generateConfig, "generate a simple config");

    std::string logPath{};
    app.add_option("-l,--log", logPath, "path to log file");

    CLI11_PARSE(app, argc, argv);

    setUpLogging(debug, logPath);

    if (generateConfig) {
        std::cout << btl::Config::defaultConfiguration() << std::endl;
        return 0;
    }

    const auto path = getRootPath(root);
    const btl::ConfigReader configReader{path, btl::to_string(btl::Config::defaultConfiguration())};
    const auto& config = configReader.get();

    std::signal(SIGINT, signalHandler);

    btl::BuildWatchTask task;
    task.start(path, config, dryRun);

    while (!quit.load()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        spdlog::trace("Sleeping main thread");
    }

    // Stop the thread using the stop token
    task.stop();

    return 0;
}
