// Copyright (c) 2025 Playful KittyKat
//
// This file is part of Slippy.
//
// Slippy is free software: you can redistribute it and/or modify it under the
// terms of the GNU Affero General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// Slippy is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
// details.
//
// You should have received a copy of the GNU Affero General Public License
// along with Slippy. If not, see <https://www.gnu.org/licenses/>.

#include "slippy.h"
#include <criterion/criterion.h>
#include <stddef.h>
#include <stdint.h>

// NOLINTBEGIN(readability-magic-numbers)
Test(write_end, too_small)
{
    uint8_t buffer[] = { 0x00 };
    size_t actual = Slippy_write_end(&buffer[0], 0);
    cr_assert_eq(actual, 0);
}
Test(write_end, just_right)
{
    uint8_t buffer[] = { 0x00 };
    size_t actual = Slippy_write_end(&buffer[0], sizeof(buffer));
    cr_assert_eq(actual, 1);

    uint8_t expected[] = { SLIPPY_BYTE_END };
    cr_assert_arr_eq(buffer, expected, sizeof(expected));
}
Test(write_end, too_big)
{
    uint8_t buffer[] = { 0x00, 0xFF };
    size_t actual = Slippy_write_end(&buffer[0], sizeof(buffer));
    cr_assert_eq(actual, 1);

    uint8_t expected[] = { SLIPPY_BYTE_END, 0xFF };
    cr_assert_arr_eq(buffer, expected, sizeof(expected));
}

Test(write, too_small_end)
{
    uint8_t buffer[] = { 0x00 };
    size_t actual = Slippy_write(SLIPPY_BYTE_END, &buffer[0], sizeof(buffer));
    cr_assert_eq(actual, 0);

    uint8_t expected[] = { 0x00 };
    cr_assert_arr_eq(buffer, expected, sizeof(expected));
}
Test(write, too_small_esc)
{
    uint8_t buffer[] = { 0x00 };
    size_t actual = Slippy_write(SLIPPY_BYTE_ESC, &buffer[0], sizeof(buffer));
    cr_assert_eq(actual, 0);

    uint8_t expected[] = { 0x00 };
    cr_assert_arr_eq(buffer, expected, sizeof(expected));
}
Test(write, too_small_simple)
{
    uint8_t buffer[] = { 0x00 };
    size_t actual = Slippy_write(0x00, &buffer[0], 0);
    cr_assert_eq(actual, 0);

    uint8_t expected[] = { 0x00 };
    cr_assert_arr_eq(buffer, expected, sizeof(expected));
}
Test(write, end)
{
    uint8_t buffer[] = { 0x00, 0x00 };
    size_t actual = Slippy_write(SLIPPY_BYTE_END, &buffer[0], sizeof(buffer));
    cr_assert_eq(actual, 2);

    uint8_t expected[] = { SLIPPY_BYTE_ESC, SLIPPY_BYTE_ESC_END };
    cr_assert_arr_eq(buffer, expected, sizeof(expected));
}
Test(write, esc)
{
    uint8_t buffer[] = { 0x00, 0x00 };
    size_t actual = Slippy_write(SLIPPY_BYTE_ESC, &buffer[0], sizeof(buffer));
    cr_assert_eq(actual, 2);

    uint8_t expected[] = { SLIPPY_BYTE_ESC, SLIPPY_BYTE_ESC_ESC };
    cr_assert_arr_eq(buffer, expected, sizeof(expected));
}
Test(write, simple)
{
    uint8_t buffer[] = { 0x00 };
    size_t actual = Slippy_write(0xaa, &buffer[0], sizeof(buffer));
    cr_assert_eq(actual, 1);

    uint8_t expected[] = { 0xaa };
    cr_assert_arr_eq(buffer, expected, sizeof(expected));
}
// NOLINTEND(readability-magic-numbers)
