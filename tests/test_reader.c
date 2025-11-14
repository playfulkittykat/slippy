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
#include <criterion/parameterized.h>
#include <stdint.h>

// NOLINTBEGIN(readability-magic-numbers)

typedef struct StateTransitionParams
{
    Slippy_Reader initial;
    uint8_t c;
    short output;
    Slippy_Reader final;
} StateTransitionParams;

ParameterizedTestParameters(read, state_transition)
{
    static StateTransitionParams params[] = {
        {
          .initial = SLIPPY_UNSYNCHRONIZED,
          .c = 0xc0,
          .output = -1,
          .final = SLIPPY_READ,
        },
        {
          .initial = SLIPPY_UNSYNCHRONIZED,
          .c = 0xdb,
          .output = -1,
          .final = SLIPPY_UNSYNCHRONIZED,
        },
        {
          .initial = SLIPPY_UNSYNCHRONIZED,
          .c = 0x00,
          .output = -1,
          .final = SLIPPY_UNSYNCHRONIZED,
        },
        {
          .initial = SLIPPY_UNSYNCHRONIZED,
          .c = 0xdc,
          .output = -1,
          .final = SLIPPY_UNSYNCHRONIZED,
        },
        {
          .initial = SLIPPY_UNSYNCHRONIZED,
          .c = 0xdd,
          .output = -1,
          .final = SLIPPY_UNSYNCHRONIZED,
        },

        {
          .initial = SLIPPY_READ,
          .c = 0xc0,
          .output = -1,
          .final = SLIPPY_END,
        },
        {
          .initial = SLIPPY_READ,
          .c = 0xdb,
          .output = -1,
          .final = SLIPPY_ESCAPE,
        },
        {
          .initial = SLIPPY_READ,
          .c = 0x00,
          .output = 0,
          .final = SLIPPY_READ,
        },
        {
          .initial = SLIPPY_READ,
          .c = 0xdc,
          .output = 0xdc,
          .final = SLIPPY_READ,
        },
        {
          .initial = SLIPPY_READ,
          .c = 0xdd,
          .output = 0xdd,
          .final = SLIPPY_READ,
        },

        {
          // This is the behaviour in the reference implementation on pages 5
          // and 6 of RFC 1055, weirdly enough. Unlike invalid escapes, it
          // isn't mentioned as a protocol violation.
          .initial = SLIPPY_ESCAPE,
          .c = 0xc0,
          .output = -1,
          .final = SLIPPY_END,
        },
        {
          .initial = SLIPPY_ESCAPE,
          .c = 0xdb,
          .output = -1,
          .final = SLIPPY_UNSYNCHRONIZED,
        },
        {
          .initial = SLIPPY_ESCAPE,
          .c = 0x00,
          .output = -1,
          .final = SLIPPY_UNSYNCHRONIZED,
        },
        {
          .initial = SLIPPY_ESCAPE,
          .c = 0xdc,
          .output = 0xc0,
          .final = SLIPPY_READ,
        },
        {
          .initial = SLIPPY_ESCAPE,
          .c = 0xdd,
          .output = 0xdb,
          .final = SLIPPY_READ,
        },

        {
          .initial = SLIPPY_END,
          .c = 0xc0,
          .output = -1,
          .final = SLIPPY_END,
        },
        {
          .initial = SLIPPY_END,
          .c = 0xdb,
          .output = -1,
          .final = SLIPPY_ESCAPE,
        },
        {
          .initial = SLIPPY_END,
          .c = 0x00,
          .output = 0,
          .final = SLIPPY_READ,
        },
        {
          .initial = SLIPPY_END,
          .c = 0xdc,
          .output = 0xdc,
          .final = SLIPPY_READ,
        },
        {
          .initial = SLIPPY_END,
          .c = 0xdd,
          .output = 0xdd,
          .final = SLIPPY_READ,
        },
    };

    size_t nb_params = sizeof(params) / sizeof(StateTransitionParams);
    return cr_make_param_array(StateTransitionParams, params, nb_params);
}

ParameterizedTest(StateTransitionParams* params, read, state_transition)
{
    Slippy_Reader reader = params->initial;
    short output = Slippy_read(&reader, params->c);

    cr_assert_eq(output, params->output, "Incorrect return value.");
    cr_assert_eq(reader, params->final, "Did not enter expected state.");
}

// Some of the decode tests stolen from:
// https://github.com/jmaygarden/slip-codec/blob/master/src/decoder.rs

Test(decode, one_end)
{
    uint8_t buffer[] = { 0xc0 };

    Slippy_Packets packets = {
        .buffer = &buffer[0],
        .buffer_length = sizeof(buffer),
        .packet = 0,
        .packet_length = 0,
    };

    Slippy_decode(&packets);

    cr_assert_eq(packets.buffer_length, 0);
    cr_assert_null(packets.packet);
    cr_assert_eq(packets.packet_length, 0);
}

Test(decode, two_end)
{
    uint8_t buffer[] = { 0xc0, 0xc0 };

    Slippy_Packets packets = {
        .buffer = &buffer[0],
        .buffer_length = sizeof(buffer),
        .packet = 0,
        .packet_length = 0,
    };

    Slippy_decode(&packets);

    cr_assert_eq(packets.buffer_length, 0);
    cr_assert_eq(packets.packet, &buffer[0]);
    cr_assert_eq(packets.packet_length, 0);
}

Test(decode, simple)
{
    uint8_t buffer[] = { 0xc0, 0x01, 0x02, 0x03, 0x04, 0x05, 0xc0 };

    Slippy_Packets packets = {
        .buffer = &buffer[0],
        .buffer_length = sizeof(buffer),
        .packet = 0,
        .packet_length = 0,
    };

    Slippy_decode(&packets);

    cr_assert_eq(packets.buffer_length, 0);
    cr_assert_eq(packets.packet, &buffer[0]);

    uint8_t expected[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };

    cr_assert_eq(packets.packet_length, sizeof(expected));
    cr_assert_arr_eq(packets.packet, expected, sizeof(expected));
}

Test(decode, two_packets)
{
    uint8_t buffer[] = { 0xc0, 0x01, 0xc0, 0x02, 0xc0 };

    Slippy_Packets packets = {
        .buffer = &buffer[0],
        .buffer_length = sizeof(buffer),
        .packet = 0,
        .packet_length = 0,
    };

    Slippy_decode(&packets);

    cr_assert_eq(packets.buffer_length, 2);
    cr_assert_eq(packets.packet, &buffer[0]);

    uint8_t expected[] = { 0x01 };

    cr_assert_eq(packets.packet_length, sizeof(expected));
    cr_assert_arr_eq(packets.packet, expected, sizeof(expected));

    Slippy_decode(&packets);

    cr_assert_eq(packets.buffer_length, 0);
    cr_assert_eq(packets.packet, &buffer[3]);

    uint8_t expected2[] = { 0x02 };

    cr_assert_eq(packets.packet_length, sizeof(expected2));
    cr_assert_arr_eq(packets.packet, expected2, sizeof(expected2));
}

Test(decode, esc_then_esc_end_sequence)
{
    uint8_t buffer[] = { 0xc0, 0x01, 0xdb, 0xdc, 0x03, 0xc0 };

    Slippy_Packets packets = {
        .buffer = &buffer[0],
        .buffer_length = sizeof(buffer),
        .packet = 0,
        .packet_length = 0,
    };

    Slippy_decode(&packets);

    cr_assert_eq(packets.buffer_length, 0);
    cr_assert_eq(packets.packet, &buffer[0]);

    uint8_t expected[] = { 0x01, 0xc0, 0x03 };

    cr_assert_eq(packets.packet_length, sizeof(expected));
    cr_assert_arr_eq(packets.packet, expected, sizeof(expected));
}

Test(decode, esc_then_esc_esc_sequence)
{
    uint8_t buffer[] = { 0xc0, 0x01, 0xdb, 0xdd, 0x03, 0xc0 };

    Slippy_Packets packets = {
        .buffer = &buffer[0],
        .buffer_length = sizeof(buffer),
        .packet = 0,
        .packet_length = 0,
    };

    Slippy_decode(&packets);

    cr_assert_eq(packets.buffer_length, 0);
    cr_assert_eq(packets.packet, &buffer[0]);

    uint8_t expected[] = { 0x01, 0xdb, 0x03 };

    cr_assert_eq(packets.packet_length, sizeof(expected));
    cr_assert_arr_eq(packets.packet, expected, sizeof(expected));
}

Test(decode, incomplete)
{
    uint8_t buffer[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };

    Slippy_Packets packets = {
        .buffer = &buffer[0],
        .buffer_length = sizeof(buffer),
        .packet = &buffer[0],
        .packet_length = 0,
    };

    Slippy_decode(&packets);

    cr_assert_eq(packets.buffer_length, 0);
    cr_assert_null(packets.packet);
    cr_assert_eq(packets.packet_length, 0);
}

// NOLINTEND(readability-magic-numbers)
