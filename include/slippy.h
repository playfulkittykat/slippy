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

#ifndef slippy_h__
#define slippy_h__

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(__GNUC__)
#define SLIPPY_PACKED __attribute__((__packed__))
#else
#define SLIPPY_PACKED
#endif

#define SLIPPY_BYTE_END (192)
#define SLIPPY_BYTE_ESC (219)
#define SLIPPY_BYTE_ESC_END (220)
#define SLIPPY_BYTE_ESC_ESC (221)

    enum SLIPPY_PACKED Slippy_Reader
    {
        SLIPPY_UNSYNCHRONIZED = 0,
        SLIPPY_READ,
        SLIPPY_ESCAPE,
        SLIPPY_END,
    };

    typedef enum Slippy_Reader Slippy_Reader;

    extern short Slippy_read(Slippy_Reader* reader, uint8_t c);

    struct Slippy_Packets
    {
        uint8_t* buffer;
        size_t buffer_length;

        uint8_t* packet;
        size_t packet_length;
    };

    typedef struct Slippy_Packets Slippy_Packets;

    extern void Slippy_decode(Slippy_Packets* packets);

    inline size_t Slippy_write(const uint8_t c,
                               uint8_t* const buffer,
                               const size_t length)
    {
        if (SLIPPY_BYTE_END == c) {
            if (2 > length) {
                return 0;
            }
            buffer[0] = SLIPPY_BYTE_ESC;
            buffer[1] = SLIPPY_BYTE_ESC_END;
            return 2;
        } else if (SLIPPY_BYTE_ESC == c) {
            if (2 > length) {
                return 0;
            }
            buffer[0] = SLIPPY_BYTE_ESC;
            buffer[1] = SLIPPY_BYTE_ESC_ESC;
            return 2;
        } else {
            if (1 > length) {
                return 0;
            }
            buffer[0] = c;
            return 1;
        }
    }

    inline size_t Slippy_write_end(uint8_t* const buffer, const size_t length)
    {
        if (1 > length) {
            return 0;
        }

        buffer[0] = SLIPPY_BYTE_END;
        return 1;
    }

#undef SLIPPY_PACKED

#ifdef __cplusplus
}
#endif

#endif /* slippy_h__ */
