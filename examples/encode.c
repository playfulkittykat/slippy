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
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

int
main(void)
{
    int c;
    uint8_t buf[2];
    size_t length;

    /*
     * By default, the decoder will discard all bytes until the first
     * `SLIPPY_BYTE_END`. Send that here.
     */
    length = Slippy_write_end(&buf[0], sizeof(buf));
    assert(length == 1);
    putchar(buf[0]);

    while ((c = getchar()) != EOF) {
        if ('\n' == c) {
            length = Slippy_write_end(&buf[0], sizeof(buf));
        } else {
            length = Slippy_write((uint8_t)c, &buf[0], sizeof(buf));
        }

        assert(length > 0 && length < 3);

        putchar(buf[0]);
        if (length == 2) {
            putchar(buf[1]);
        }

        fflush(stdout);
    }

    return 0;
}
