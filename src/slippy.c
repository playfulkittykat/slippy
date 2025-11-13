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
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>

static inline short
read_unescaped(Slippy_Reader* const reader, const uint8_t c)
{
    switch (c) {
        case SLIPPY_BYTE_ESC:
            *reader = SLIPPY_ESCAPE;
            return -1;

        // GCOVR_EXCL_START
        case SLIPPY_BYTE_END:
            abort();
            // GCOVR_EXCL_STOP

        default:
            return c;
    }
}

static inline short
read_escaped(Slippy_Reader* const reader, const uint8_t c)
{
    *reader = SLIPPY_READ;

    switch (c) {
        case SLIPPY_BYTE_ESC_END:
            return SLIPPY_BYTE_END;
        case SLIPPY_BYTE_ESC_ESC:
            return SLIPPY_BYTE_ESC;
        default:
            // Unrecognized escape sequence.
            *reader = SLIPPY_UNSYNCHRONIZED;
            return -1;
    }
}

short
Slippy_read(Slippy_Reader* const reader, const uint8_t c)
{
    if (SLIPPY_END == *reader) {
        *reader = SLIPPY_READ;
    }

    if (SLIPPY_BYTE_END == c) {
        if (SLIPPY_UNSYNCHRONIZED == *reader) {
            *reader = SLIPPY_READ;
        } else {
            *reader = SLIPPY_END;
        }
        return -1;
    }

    switch (*reader) {
        case SLIPPY_ESCAPE:
            return read_escaped(reader, c);
        case SLIPPY_READ:
            return read_unescaped(reader, c);
        case SLIPPY_UNSYNCHRONIZED:
            return -1;
        // GCOVR_EXCL_START
        case SLIPPY_END:
            // `SLIPPY_END` is unconditionally converted to `SLIPPY_READ` above.
        default:
            abort();
            // GCOVR_EXCL_STOP
    }
}

void
Slippy_decode(Slippy_Packets* packets)
{
    if (!packets) {
        abort(); // GCOVR_EXCL_LINE
    }

    Slippy_Reader reader =
      packets->packet ? SLIPPY_READ : SLIPPY_UNSYNCHRONIZED;
    packets->packet = 0;
    packets->packet_length = 0;

    if (!packets->buffer || !packets->buffer_length) {
        return;
    }

    uint8_t* write = 0;
    size_t written = 0;

    while (0 < packets->buffer_length) {
        uint8_t c = *packets->buffer;

        short output = Slippy_read(&reader, c);

        if (SLIPPY_UNSYNCHRONIZED != reader && !write) {
            write = packets->buffer;
        }

        packets->buffer_length--;
        packets->buffer += 1;

        if (0 <= output) {
            assert(write); // GCOVR_EXCL_LINE
            write[written++] = (uint8_t)output;
        }

        if (SLIPPY_END == reader) {
            packets->packet = write;
            packets->packet_length = written;
            return;
        }
    }
}

// NOLINTBEGIN(readability-redundant-declaration)
extern inline size_t
Slippy_write(uint8_t c, uint8_t* buffer, size_t length);
extern inline size_t
Slippy_write_end(uint8_t* buffer, size_t length);
// NOLINTEND(readability-redundant-declaration)
