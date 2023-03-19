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
/*
 * Nano ID: A tiny, secure, URL-friendly, unique string ID generator.
 * https://github.com/ai/nanoid
 */

#include <sys/random.h> /* getrandom() */
#include <stdint.h>

#include "nanoid.h"

/* Alphabet: A-Za-z0-9-_ (i.e., base64url; see RFC 4648, Section 5) */
static const unsigned char default_alphabet[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";


/*
 * Round up to the next highest power of 2.
 * Credit: https://graphics.stanford.edu/%7Eseander/bithacks.html#RoundUpPowerOf2
 */
static inline uint32_t
roundup2(uint32_t v)
{
    if (v == 0 || v == 1)
        return v;

#if defined(__GNUC__) || defined(__clang__)
    v = 2U << (31 - __builtin_clz((v - 1) | 1));
#else
    v--;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v++;
#endif

    return v;
}


void *
nanoid_generate_r(void *buf, size_t buflen, const unsigned char *alphabet,
                  size_t alphacnt)
{
    if (alphabet == NULL) {
        alphabet = default_alphabet;
        alphacnt = sizeof(default_alphabet) - 1;
    }

    if (alphacnt <= 1 || alphacnt >= 256) {
        return NULL;
    }

    /*
     * Use a mask instead of 'random % alphacnt' to ensure a uniform
     * distribution.
     * See: https://github.com/ai/nanoid#security
     */
    uint32_t mask = roundup2((uint32_t)alphacnt) - 1;

    /* Size of 32 is tuned by benchmarks for the default size. */
    unsigned char bytes[32];
    size_t len = 0;
    while (1) {
        getrandom(bytes, sizeof(bytes), 0);
        size_t i, ai;
        for (i = 0; i < sizeof(bytes); ++i) {
            ai = bytes[i] & mask;
            if (ai >= alphacnt)
                continue;
            ((unsigned char *)buf)[len++] = alphabet[ai];
            if (len == buflen)
                return buf;
        }
    }

    return NULL;
}


const char *
nanoid_generate(const unsigned char *alphabet, size_t alphacnt)
{
    static char buf[NANOID_SIZE + 1];

    if (nanoid_generate_r(buf, NANOID_SIZE, alphabet, alphacnt) == NULL)
        return NULL;

    buf[NANOID_SIZE] = '\0';
    return buf;
}
