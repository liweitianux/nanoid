/*-
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2023 Aaron LI
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef NANOID_H_
#define NANOID_H_

#include <stddef.h> /* size_t */


/* ID default size/length (without the terminating NUL) */
#define NANOID_SIZE     21

/*
 * Generates an ID of length <buflen> and stores into <buf>, using alphabet
 * <alphabet> of size <alphacnt>.
 * If <alphabet> is NULL, then the default alphabet is used; <alphacnt> is
 * also ignored in this case.
 *
 * Returns a pointer to <buf> on success, or NULL on error.
 *
 * Reentrantable (i.e., thread-safe).
 */
void *nanoid_generate_r(void *buf, size_t buflen,
                        const unsigned char *alphabet, size_t alphacnt);

/*
 * Generates an ID of the default length, using alphabet <alphabet> of size
 * <alphacnt>.
 * If <alphabet> is NULL, then the default alphabet is used; <alphacnt> is
 * also ignored in this case.
 *
 * Uses an internal static buffer to store the ID, so NOT thread-safe.
 * The ID buffer will be NUL-terminated.
 *
 * Returns a pointer to the internal buffer on success, or NULL on error.
 */
const char *nanoid_generate(const unsigned char *alphabet, size_t alphacnt);


#endif
