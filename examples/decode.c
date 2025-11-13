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

    /*
     * Starting in `SLIPPY_UNSYNCHRONIZED` will discard any input until a
     * `SLIPPY_BYTE_END` is received. Alternatively, starting in `SLIPPY_READ`
     * would preserve it.
     */
    Slippy_Reader reader = SLIPPY_UNSYNCHRONIZED;

    while ((c = getchar()) != EOF) {
        short output = Slippy_read(&reader, (uint8_t)c);

        if (0 <= output) {
            putchar(output);
        }

        if (SLIPPY_END == reader) {
            putchar('\n');

            /*
             * By default, slippy does not require packets (after the first) to
             * start with `SLIPPY_BYTE_END`. If you'd like that behaviour,
             * reset back to `SLIPPY_UNSYNCHRONIZED` after every packet:
             */
            // reader = SLIPPY_UNSYNCHRONIZED;
        }

        fflush(stdout);
    }

    return 0;
}
