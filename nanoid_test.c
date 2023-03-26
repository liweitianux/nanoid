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
 * Test of Nano ID distribution uniformity.
 */

#include <assert.h>
#include <math.h>
#include <stdlib.h>

/*--------------------------------------------------------------------------*/

/*
 * Credit:
 * https://www.codeproject.com/Articles/432194/How-to-Calculate-the-Chi-Squared-P-Value
 * https://en.wikipedia.org/wiki/Chi-squared_distribution#Cumulative_distribution_function
 * https://www.stat.uchicago.edu/~mcpeek/software/CCQLSpackage1.3/chisq.c
 */

/*
 * Calculate the incomplete gamma function using its power series expansion.
 *
 * Credit:
 * https://en.wikipedia.org/wiki/Incomplete_gamma_function#Evaluation_formulae
 */
static double
igf(double s, double z)
{
    const double eps = 1e-6;
    double coef, sum, term, nom, denom;

    if (z <= 0.0)
        return 0.0;

    coef = pow(z, s) * exp(-z);

    sum = 1.0 / s; /* first term */
    nom = 1.0;
    denom = s;
    do {
        nom *= z;
        denom *= ++s;
        term = nom / denom;
        sum += term;
    } while (term > eps);

    return coef * sum;
}

/*
 * Calculate the p-value of critical value <cv> and degree of freedom <dof>
 * accroding to the chi-square distribution.
 * chisq(cv, dof) = 1 - chisq_CDF(cv, dof)
 */
static double
chisq_p(double cv, int dof)
{
    double pvalue;

    if (cv <= 0.0 || dof < 1)
        return 0.0;

    if (dof == 2)
        return exp(-0.5 * cv);

    pvalue = igf(0.5 * dof, 0.5 * cv);
    if (isnan(pvalue) || isinf(pvalue) || pvalue <= 1e-8)
        return 1e-14;

    pvalue /= tgamma(0.5 * dof);

    return 1.0 - pvalue;
}


#ifdef CHISQTEST

#include <stdio.h>

static double
chisq_cv(double p, int dof)
{
    const double eps = 1e-6;
    double cv, min, max = 1e6;

    if (p <= 0.0)
        return max;
    else if (p >= 1.0)
        return 0.0;

    cv = dof / sqrt(p);  /* fair first value */
    while (max - min > eps) {
        if (chisq_p(cv, dof) < p)
            max = cv;
        else
            min = cv;
        cv = (min + max) * 0.5;
    }

    return cv;
}

int main(void)
{
    const double probs[] = { 0.1, 0.05, 0.01, 0.005, 0.001 };
    int df;
    size_t i;

    printf("%4s ", "df");
    for (i = 0; i < sizeof(probs)/sizeof(probs[0]); ++i)
        printf("%8.3f ", probs[i]);
    putchar('\n');

    for (df = 1; df < 30; df++) {
        printf("%4d ", df);
        for (i = 0; i < sizeof(probs)/sizeof(probs[0]); ++i)
            printf("%8.3f ", chisq_cv(probs[i], df));
        putchar('\n');
    }

    return 0;
}

#endif /* CHISQTEST */

/*--------------------------------------------------------------------------*/

struct sample {
    size_t size; /* number of items */
    size_t len; /* length of every item */
    int counts[256]; /* counts of every character in all items */
};


static struct sample *
sample_new(void)
{
    return calloc(1, sizeof(struct sample));
}


static void
sample_free(struct sample *s)
{
    free(s);
}


static void
sample_add(struct sample *s, const void *id, size_t len)
{
    const unsigned char *p;
    size_t i;

    if (s->len == 0)
        s->len = len;
    assert(s->len == len);

    for (i = 0, p = id; i < len; ++i, p++)
        s->counts[*p]++;

    s->size++;
}


/*
 * Perform a chi-square test to check whether or not the sample distribution
 * is uniform.
 */
static int
sample_test(struct sample *s)
{
    const double alpha = 0.05; /* significance level */
    double exp, chisq, pvalue, v;
    int dof, alphacnt, i;

    printf("Sample: size=%zu, len=%zu\n", s->size, s->len);

    alphacnt = 0;
    for (i = 0; i < 256; ++i) {
        if (s->counts[i] > 0) {
            alphacnt++;
        }
    }
    dof = alphacnt - 1;

    exp = (double)(s->size * s->len) / (double)alphacnt;
    printf("dof=%d, expectation=%.3f\n", dof, exp);

    chisq = 0;
    for (i = 0; i < 256; ++i) {
        if (s->counts[i] > 0) {
            v = (double)s->counts[i] - exp;
            chisq += v * v / exp;
        }
    }
    pvalue = chisq_p(chisq, dof);
    printf("chisq=%.6f, p-value=%.4f\n", chisq, pvalue);

    if (pvalue >= alpha) {
        printf("Distribution is uniform (alpha=%.4f).\n", alpha);
        return 0;
    } else {
        printf("Distribution is NOT uniform (alpha=%.4f)!\n", alpha);
        return 1;
    }
}
