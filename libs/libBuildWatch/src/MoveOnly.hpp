/*
 * Copyright (c) 2024.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#pragma once
#include <utility>

namespace btl {

/// Trivial wrapper to make a type move-only.
/// @tparam T
/// @tparam defaultValue
template<typename T, T defaultValue = {}>
class MoveOnly
{
public:
    /// Default construction
    MoveOnly()
        : value(defaultValue)
    {}

    /// Copy construction
    explicit MoveOnly(const T& value)
        : value(value)
    {}

    /// Move construction
    MoveOnly(MoveOnly&& other) noexcept
        : value(std::exchange(other.value, defaultValue))
    {}

    /// Assignment
    MoveOnly& operator=(MoveOnly&& other) noexcept
    {
        value = std::exchange(other.value, defaultValue);
        return *this;
    }

    /// Assignment
    MoveOnly& operator=(T const& other) noexcept
    {
        value = other;
        return *this;
    }

    /// Implicit type conversion
    operator T() { return value; }

    /// Explicit type conversion
    [[nodiscard]] T get() const { return value; }

    [[nodiscard]] bool operator==(MoveOnly const& other) const { return value == other.value; }

private:
    T value;
};
} // namespace btl