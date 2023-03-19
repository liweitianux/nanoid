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
 * Nano ID command utility
 * A tiny, secure, URL-friendly, unique string ID generator.
 * https://github.com/ai/nanoid
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> /* clock_gettime() */
#include <unistd.h> /* getopt() */

#include "nanoid.h"


static char *progname;
static size_t speed_count = 1000000; /* iterations for speed test */

static void usage(void);


static uint64_t
timespec_diff(struct timespec *tend, struct timespec *tstart)
{
    uint64_t t;

    t = (tend->tv_sec - tstart->tv_sec) * 1000000000UL;
    t += tend->tv_nsec;
    t -= tstart->tv_nsec;

    return t;
}


static int
cmd_generate(int argc, char *argv[])
{
    const char *alphabet;
    char *buf, *endp;
    size_t length;
    int opt;

    alphabet = NULL;
    length = NANOID_SIZE;

    while ((opt = getopt(argc, argv, "a:l:")) != -1) {
        switch (opt) {
        case 'a':
            alphabet = optarg;
            break;
        case 'l':
            length = (size_t)strtoul(optarg, &endp, 10);
            if (length == 0 || endp == optarg || *endp != '\0') {
                fprintf(stderr, "ERROR: invalid length: %s\n", optarg);
                exit(1);
            }
            break;
        default:
            usage();
        }
    }
    if (argc != optind)
        usage();

    buf = malloc(length);
    if (buf == NULL) {
        fprintf(stderr, "ERROR: failed to allocate memory\n");
        exit(1);
    }

    if (nanoid_generate_r(buf, length, (const unsigned char *)alphabet,
                          alphabet ? strlen(alphabet) : 0) == NULL) {
        fprintf(stderr, "ERROR: failed to generate ID\n");
        exit(1);
    }

    printf("%.*s\n", (int)length, buf);
    free(buf);

    return 0;
}


static int
cmd_speed(int argc, char *argv[])
{
    struct timespec tstart, tend;
    uint64_t t;
    size_t count, burnin, length, i;
    char *buf, *endp;
    int opt;

    length = NANOID_SIZE;
    count = speed_count;
    burnin = 0;

    while ((opt = getopt(argc, argv, "b:c:l:")) != -1) {
        switch (opt) {
        case 'b':
            burnin = (size_t)strtoul(optarg, &endp, 10);
            if (burnin == 0 || endp == optarg || *endp != '\0') {
                fprintf(stderr, "ERROR: invalid burnin: %s\n", optarg);
                exit(1);
            }
            break;
        case 'c':
            count = (size_t)strtoul(optarg, &endp, 10);
            if (count == 0 || endp == optarg || *endp != '\0') {
                fprintf(stderr, "ERROR: invalid count: %s\n", optarg);
                exit(1);
            }
            break;
        case 'l':
            length = (size_t)strtoul(optarg, &endp, 10);
            if (length == 0 || endp == optarg || *endp != '\0') {
                fprintf(stderr, "ERROR: invalid length: %s\n", optarg);
                exit(1);
            }
            break;
        default:
            usage();
        }
    }
    if (argc != optind)
        usage();

    if (burnin == 0)
        burnin = count / 10;

    buf = malloc(length);
    if (buf == NULL) {
        fprintf(stderr, "ERROR: failed to allocate memory\n");
        exit(1);
    }

    printf("Burning in ... (n=%zu)\n", burnin);
    for (i = 1; i < burnin; ++i) {
        nanoid_generate_r(buf, length, NULL, 0);
        (void)buf;
    }

    printf("Running speed test ... (n=%zu)\n", count);
    clock_gettime(CLOCK_MONOTONIC, &tstart);
    for (i = 1; i < count; ++i) {
        nanoid_generate_r(buf, length, NULL, 0);
        (void)buf;
    }
    clock_gettime(CLOCK_MONOTONIC, &tend);

    t = timespec_diff(&tend, &tstart);
    printf("Speed: %zu ns/id\n", t / count);

    free(buf);
    return 0;
}


static void
usage(void)
{
    fprintf(stderr,
            "Nano ID command utility.\n"
            "\n"
            "Generate ID:\n"
            ">>> %s [-a alphabet] [-l length]\n"
            "    -a: specify the custom alphabet\n"
            "    -l: specify the custom ID length\n"
            "\n"
            "Speed test:\n"
            ">>> %s speed [-b burnin] [-c count] [-l length]\n"
            "    -b: specify the burn-in iterations (default: count/10)\n"
            "    -c: specify the test iterations (default: %zu)\n"
            "    -l: specify the custom ID length\n"
            "\n"
            , progname, progname, speed_count);
    exit(1);
}


int
main(int argc, char *argv[])
{
    const char *cmd;

    progname = argv[0];

    if (argc == 1) {
        return cmd_generate(argc, argv);
    }

    cmd = argv[1];
    if (strcmp(cmd, "speed") == 0) {
        optind++;
        return cmd_speed(argc, argv);
    } else {
        return cmd_generate(argc, argv);
    }

    return 0;
}
