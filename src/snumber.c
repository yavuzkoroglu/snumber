#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DIGITS  (20)
#define MAX_SLOTS   MAX_DIGITS

#define COUNT_SPLITS(nDigits) ((1 << (nDigits - 1)) - 1)

static uint64_t pow[MAX_DIGITS + 1][MAX_DIGITS];

static uint64_t n_choose_r[MAX_DIGITS + 1][MAX_DIGITS + 1];

static bool checkSplit(uint64_t split, unsigned const true_nDigits) {
    unsigned nDigits = 0;
    while (split > 0) {
        uint64_t const slot_nDigits = split % true_nDigits;
        if (slot_nDigits == 0) return 0;
        nDigits += slot_nDigits;
        split /= true_nDigits;
    }

    return nDigits == true_nDigits;
}

static void computeSplits(uint64_t* const splits, unsigned const nDigits, unsigned const nSplits) {
    splits[0] = 0;
    for (unsigned slotId = 0; slotId < nDigits; slotId++)
        splits[0] += pow[nDigits][slotId];

    uint64_t delta = pow[nDigits][nDigits - 2] * (nDigits - 1);
    for (unsigned splitId = 1; splitId < nSplits; splitId++) {
        splits[splitId] = splits[splitId - 1] - delta;
        while (!checkSplit(splits[splitId], nDigits))
            splits[splitId] -= delta;
        delta = (delta > nDigits - 1) ? delta / nDigits : nDigits - 1;
    }
}

static unsigned countDigits(uint64_t const x) {
    if (x == 0) return 1;

    unsigned l = 0;
    unsigned r = MAX_DIGITS;

    while (l < r) {
        unsigned m = (l + r) >> 1;
        if (pow[10][m] > x) r = m; else l = m + 1;
    }

    return r;
}

static void dumpSplit(uint64_t s, uint64_t split, unsigned const nDigits) {
    uint64_t slots[MAX_SLOTS + 1];

    unsigned slotId = 0;
    while (split > 0) {
        uint64_t const slot_nDigits = split % nDigits;
        uint64_t const base         = pow[10][slot_nDigits];
        slots[slotId++] = s % base;
        s /= base;
        split /= nDigits;
    }

    printf("%"PRIu64, slots[--slotId]);
    while (slotId > 0)
        printf(" + %"PRIu64, slots[--slotId]);
    puts("");
}

static void fillAll_n_choose_r(int const max_n) {
    n_choose_r[0][0] = 1;
    n_choose_r[1][0] = 1;
    n_choose_r[1][1] = 1;
    for (int n = 2; n <= max_n; n++) {
        n_choose_r[n][0] = 1;
        n_choose_r[n][n] = 1;
        for (int r = 1; r < n; r++)
            n_choose_r[n][r] = n_choose_r[n - 1][r] + n_choose_r[n - 1][r - 1];
    }
}

static void fillAll_pow(unsigned const max_b, unsigned const max_p) {
    for (unsigned b = 0; b <= max_b; b++) {
        pow[b][0] = 1;

        uint64_t b_to_p_power = 1;
        for (unsigned p = 1; p <= max_p; p++)
            pow[b][p] = (b_to_p_power *= b);
    }
}

static uint64_t sumSplit(uint64_t s, uint64_t split, unsigned const nDigits) {
    uint64_t sum = 0;

    while (split > 0) {
        uint64_t const slot_nDigits = split % nDigits;
        uint64_t const base         = pow[10][slot_nDigits];
        sum += s % base;
        s /= base;
        split /= nDigits;
    }

    return sum;
}

int main(int argc, char* argv[]) {
    puts("");

    fillAll_pow(MAX_DIGITS, MAX_DIGITS - 1);
    fillAll_n_choose_r(MAX_DIGITS);

    if (argc < 2) {
        printf(" Usage: %s <N>\n\n", argv[0]);
        return EXIT_SUCCESS;
    }

    uint64_t max_s;
    sscanf(argv[1], "%"SCNu64, &max_s);

    unsigned const max_nDigits = countDigits(max_s);
    unsigned const max_nSplits = COUNT_SPLITS(max_nDigits);

    size_t const splits_size_in_bytes   = (size_t)max_nSplits * sizeof(uint64_t);
    uint64_t* const splits              = malloc(splits_size_in_bytes);

    uint64_t s;
    uint64_t t          = 0;
    unsigned nSplits    = 0;
    unsigned nDigits    = 1;
    for (uint64_t x = 4; (s = x*x) <= max_s; x++) {
        unsigned const new_nDigits = countDigits(s);
        if (new_nDigits > nDigits) {
            nDigits = new_nDigits;
            nSplits = COUNT_SPLITS(nDigits);
            computeSplits(splits, nDigits, nSplits);
        }

        for (unsigned splitId = 0; splitId < nSplits; splitId++) {
            uint64_t const split    = splits[splitId];
            uint64_t const sum      = sumSplit(s, split, nDigits);

            if (x == sum) {
                printf("√%*"PRIu64" = %*"PRIu64" = ", max_nDigits, s, max_nDigits >> 1, x);
                dumpSplit(s, split, nDigits);
                t += s;
                break;
            }
        }
    }

    free(splits);

    printf("\n T = %"PRIu64"\n", t);

    puts("");
    return EXIT_SUCCESS;
}
