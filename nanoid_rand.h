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

#ifndef NANOID_RAND_H_
#define NANOID_RAND_H_

#undef HAVE_GETENTROPY
#undef HAVE_GETRANDOM
#undef HAVE_ARC4RANDOM_BUF

/*
 * Credit: https://sourceforge.net/p/predef/wiki/OperatingSystems/
 */
#if defined(__DragonFly__)
#  include <sys/param.h>
#  if __DragonFly_version >= 600200
#    define HAVE_GETENTROPY
#  elif __DragonFly_version >= 500710
#    define HAVE_GETRANDOM
#  else
#    define HAVE_ARC4RANDOM_BUF
#  endif
#elif defined(__FreeBSD__)
#  include <sys/param.h>
#  if __FreeBSD_version >= 1200000 /* 12.0 */
#    define HAVE_GETENTROPY
#  else
#    define HAVE_ARC4RANDOM_BUF
#  endif
#elif defined(__NetBSD__)
#  include <sys/param.h>
#  if __NetBSD_Version__ >= 1000000000 /* 10.0 */
#    define HAVE_GETENTROPY
#  else
#    define HAVE_ARC4RANDOM_BUF
#  endif
#elif defined(__OpenBSD__)
#  include <sys/param.h>
#  if OpenBSD >= 201411 /* 5.6 */
#    define HAVE_GETENTROPY
#  else
#    define HAVE_ARC4RANDOM_BUF
#  endif
#elif defined(__APPLE__)
#  include <Availability.h>
#  if __MAC_OS_X_VERSION_MAX_ALLOWED >= 101200 /* __MAC_10_12 */
#    define HAVE_GETENTROPY
#  elif __MAC_OS_X_VERSION_MAX_ALLOWED >= 1070 /* __MAC_10_7 */
#    define HAVE_ARC4RANDOM_BUF
#  endif
#elif defined(__linux__)
#  if defined(__GLIBC_MINOR__) && __GLIBC_MINOR__ >= 25
#    define HAVE_GETENTROPY
#  endif
#else
#  warning "Unsupported operation system! Fallback to [/dev/urandom]."
#endif

#if defined(HAVE_GETENTROPY)
#include <unistd.h> /* getentropy() */
#elif defined(HAVE_GETRANDOM)
#include <sys/random.h> /* getrandom() */
#elif defined(HAVE_ARC4RANDOM_BUF)
#include <stdlib.h> /* arc4random_buf() */
#else
#include <errno.h>
#include <fcntl.h> /* open() */
#include <unistd.h> /* read(), close() */
#endif


/*
 * Generate crypto-secure pseudorandom data to fill the buffer $buf
 * of size $n.
 *
 * Try to obtain random data from the following sources:
 * - getentropy()
 * - getrandom()
 * - arc4random_buf()
 * - read(/dev/urandom)
 *
 * Return 0 on success, -1 on error.
 */
static inline int
generate_randombytes(void *buf, size_t n)
{
    int ret;

#if defined(HAVE_GETENTROPY)
    ret = getentropy(buf, n);
#elif defined(HAVE_GETRANDOM)
    ret = (int)getrandom(buf, n, 0);
#elif defined(HAVE_ARC4RANDOM_BUF)
    arc4random_buf(buf, n);
    ret = 0;
#else
    int fd = open("/dev/urandom", O_RDONLY);
    if (fd == -1) {
        ret = -1;
        goto out;
    }

    size_t o = 0;
    while (o < n) {
        ssize_t r = read(fd, (unsigned char *)buf + o, n - o);
        if (r < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
                continue;
            ret = -1;
            goto out;
        }
        o += (size_t)r;
    }

    ret = 0;

out:
    if (fd >= 0)
        close(fd);
#endif

    return ret;
}


#endif
