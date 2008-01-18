/* Simple and Fast Twister */

#include <string.h>
#if defined(__GNUC__)
  #include <stdint.h>
#endif
#include <stdio.h>
#include <assert.h>
#include "random.h"
#include "paramsD11.h"

#ifndef MEXP
#define MEXP 19937
#endif

#define WORDSIZE 104
#define N (MEXP / WORDSIZE)
#define MAXDEGREE (WORDSIZE * (N + 1))

union W128_T {
    uint64_t a[2];
    double d[2];
};
typedef union W128_T w128_t;

union W64_T {
    double d;
    uint64_t u;
};
typedef union W64_T w64_t;

static w128_t sfmt[N + 1];
static double *psfmt = (double *)&(sfmt[0].d[0]);
static int idx;

void fill_array_open_close(double array[], int size);
void fill_array_close_open(double array[], int size);
void fill_array_open_open(double array[], int size);
void fill_array_close1_open2(double array[], int size);
INLINE double genrand_open_close(void);
INLINE double genrand_close_open(void);
INLINE double genrand_open_open(void);
INLINE double genrand_close1_open2(void);

#if defined(__GNUC__)
#define ALWAYSINLINE __attribute__((always_inline)) 
#else
#define ALWAYSINLINE
#endif
INLINE static void lshift128(w128_t *out, const w128_t *in, int shift)
    ALWAYSINLINE;
INLINE static void do_recursion(w128_t *r, w128_t *a, w128_t *b, w128_t *c,
				w128_t *lung) ALWAYSINLINE;
#if 0
INLINE static void convert_co(w128_t array[], int size) ALWAYSINLINE;
#endif
INLINE static void convert_oc(w128_t array[], int size) ALWAYSINLINE;
INLINE static void convert_oo(w128_t array[], int size) ALWAYSINLINE;

INLINE static void lshift128(w128_t *out, const w128_t *in, int shift) {
    out->a[0] = in->a[0] << (shift * 8);
    out->a[1] = in->a[1] << (shift * 8);
    out->a[1] |= in->a[0] >> (64 - shift * 8);
}

INLINE static void do_recursion(w128_t *r, w128_t *a, w128_t *b, w128_t *c,
				w128_t *lung){
    w128_t x;

    lshift128(&x, a, SL2);
    r->a[0] = a->a[0] ^ x.a[0] ^ ((b->a[0] >> SR1) & MSK1) ^ (c->a[0] >> SR2)
	^ (c->a[0] << SL1) ^ lung->a[1];
    r->a[1] = a->a[1] ^ x.a[1] ^ ((b->a[1] >> SR1) & MSK2) ^ (c->a[1] >> SR2)
	^ (c->a[1] << SL1) ^ lung->a[0];
    r->a[0] = r->a[0] & LOW_MASK;
    r->a[1] = r->a[1] & LOW_MASK;
    lung->a[0] ^= r->a[0];
    lung->a[1] ^= r->a[1];
    r->a[0] = r->a[0] | HIGH_CONST;
    r->a[1] = r->a[1] | HIGH_CONST;
}

#if 0
INLINE static void convert_co(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].d[0] = array[i].d[0] - 1.0;
	array[i].d[1] = array[i].d[1] - 1.0;
    }
}
#endif

INLINE static void convert_oc(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].d[0] = 2.0 - array[i].d[0];
	array[i].d[1] = 2.0 - array[i].d[1];
    }
}

INLINE static void convert_oo(w128_t array[], int size) {
    int i;

    for (i = 0; i < size; i++) {
	array[i].a[0] |= 1;
	array[i].a[1] |= 1;
	array[i].d[0] -= 1.0;
	array[i].d[1] -= 1.0;
    }
}

INLINE static void gen_rand_all(void) {
    int i;
    w128_t lung;

    lung = sfmt[N];
    do_recursion(&sfmt[0], &sfmt[0], &sfmt[POS1], &sfmt[N -1], &lung);
    for (i = 1; i < N - POS1; i++) {
	do_recursion(&sfmt[i], &sfmt[i], &sfmt[i + POS1], &sfmt[i - 1], &lung);
    }
    for (; i < N; i++) {
	do_recursion(&sfmt[i], &sfmt[i], &sfmt[i + POS1 - N], &sfmt[i - 1],
		     &lung);
    }
    sfmt[N] = lung;
}

INLINE static void gen_rand_array12(w128_t array[], int size) {
    int i, j;
    w128_t lung;

    lung = sfmt[N];
    do_recursion(&array[0], &sfmt[0], &sfmt[POS1], &sfmt[N - 1], &lung);
    for (i = 1; i < N - POS1; i++) {
	do_recursion(&array[i], &sfmt[i], &sfmt[i + POS1], &array[i - 1],
		     &lung);
    }
    for (; i < N; i++) {
	do_recursion(&array[i], &sfmt[i], &array[i + POS1 - N],
		     &array[i - 1], &lung);
    }
    for (; i < size - N; i++) {
	do_recursion(&array[i], &array[i - N], &array[i + POS1 - N],
		     &array[i - 1], &lung);
    }
    for (j = 0; j < 2 * N - size; j++) {
	sfmt[j] = array[j + size - N];
    }
    for (; i < size; i++, j++) {
	do_recursion(&array[i], &array[i - N], &array[i + POS1 - N],
		     &array[i - 1], &lung);
	sfmt[j] = array[i];
    }
    sfmt[N] = lung;
}

INLINE static void gen_rand_arrayco(w128_t array[], int size) {
    int i, j;
    w128_t lung;

    lung = sfmt[N];
    do_recursion(&array[0], &sfmt[0], &sfmt[POS1], &sfmt[N - 1], &lung);
    for (i = 1; i < N - POS1; i++) {
	do_recursion(&array[i], &sfmt[i], &sfmt[i + POS1], &array[i - 1],
		     &lung);
    }
    for (; i < N; i++) {
	do_recursion(&array[i], &sfmt[i], &array[i + POS1 - N],
		     &array[i - 1], &lung);
    }
    for (; i < size - N; i++) {
	do_recursion(&array[i], &array[i - N], &array[i + POS1 - N],
		     &array[i - 1], &lung);
	array[i - N].d[0] = array[i - N].d[0] - 1.0;
	array[i - N].d[1] = array[i - N].d[1] - 1.0;
    }
    for (j = 0; j < 2 * N - size; j++) {
	sfmt[j] = array[j + size - N];
    }
    for (; i < size; i++, j++) {
	do_recursion(&array[i], &array[i - N], &array[i + POS1 - N],
		     &array[i - 1], &lung);
	sfmt[j] = array[i];
	array[i - N].d[0] = array[i - N].d[0] - 1.0;
	array[i - N].d[1] = array[i - N].d[1] - 1.0;
    }
    for (i = size - N; i < size; i++) {
	array[i].d[0] = array[i].d[0] - 1.0;
	array[i].d[1] = array[i].d[1] - 1.0;
    }
    sfmt[N] = lung;
}

INLINE double genrand_close1_open2(void) {
    double r;

    if (idx >= N * 2) {
	gen_rand_all();
	idx = 0;
    }
    r = psfmt[idx++];
    return r;
}

INLINE double genrand_open_open(void) {
    w64_t r;

    r.d = genrand_close1_open2();
    r.u |= 1;
    return r.d - 1.0;
}

void fill_array_open_close(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_array12((w128_t *)array, size / 2);
    convert_oc((w128_t *)array, size / 2);
}

void fill_array_close_open(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_arrayco((w128_t *)array, size / 2);
}

void fill_array_open_open(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_array12((w128_t *)array, size / 2);
    convert_oo((w128_t *)array, size / 2);
}

void fill_array_close1_open2(double array[], int size)
{
    assert(size % 2 == 0);
    assert(size >= 2 * N * 2);
    gen_rand_array12((w128_t *)array, size / 2);
}

void init_gen_rand(uint64_t seed)
{
    int i;
    uint64_t *psfmt;

    psfmt = (uint64_t *)&sfmt[0];
    psfmt[0] = (seed & LOW_MASK) | HIGH_CONST;
    for (i = 1; i < (N + 1) * 2; i++) {
        psfmt[i] = 1812433253UL * (psfmt[i - 1] ^ (psfmt[i - 1] >> 30)) + i;
        psfmt[i] = (psfmt[i] & LOW_MASK) | HIGH_CONST;
    }
    idx = N * 2;
}

#include "test_time3.c"
