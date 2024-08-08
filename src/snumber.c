#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DIGITS  (20)
#define MAX_SLOTS   MAX_DIGITS

#define COUNT_SPLITS(nDigits) ((1 << (nDigits - 1)) - 1)

static uint64_t pow[MAX_DIGITS + 1][MAX_DIGITS];
static uint64_t n_choose_r[MAX_DIGITS][MAX_DIGITS];

static void computeSplits(uint64_t* const splits, unsigned const nDigits) {
    unsigned nSlots = nDigits;
    splits[0] = 0;
    for (unsigned slotId = 0; slotId < nSlots; slotId++)
        splits[0] += pow[nDigits][slotId];

    unsigned old_splitId    = 0;
    unsigned splitId        = 1;
    unsigned groupId        = 0;
    unsigned shift_amount   = 1;
    while (nSlots > 2) {
        splits[splitId] = splits[old_splitId] - pow[nDigits][nSlots - 1] + pow[nDigits][nSlots - 1 - shift_amount];
        if (++shift_amount == nSlots) {
            if (splits[splitId] < pow[nDigits][nSlots - 1] << 1) {
                nSlots--;
                old_splitId += n_choose_r[nDigits - 1][groupId++] + 1;
            }
            shift_amount = 1;
        }
        splitId++;
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
    uint64_t slot_values[MAX_SLOTS + 1];
    unsigned slot_nDigits[MAX_SLOTS + 1];

    unsigned slotId = 0;
    while (split > 0) {
        slot_nDigits[slotId]    = split % nDigits;
        uint64_t const base     = pow[10][slot_nDigits[slotId]];
        slot_values[slotId++]   = s % base;
        s /= base;
        split /= nDigits;
    }

    slotId--;
    printf("%0*"PRIu64, slot_nDigits[slotId], slot_values[slotId]);
    while (slotId-- > 0)
        printf(" + %0*"PRIu64, slot_nDigits[slotId], slot_values[slotId]);
    puts("");
}

static void fillAll_pow(unsigned const max_b, unsigned const max_p) {
    for (unsigned b = 0; b <= max_b; b++) {
        pow[b][0] = 1;

        uint64_t b_to_p_power = 1;
        for (unsigned p = 1; p <= max_p; p++)
            pow[b][p] = (b_to_p_power *= b);
    }
}

static void fillAll_n_choose_r(unsigned const max_n) {
    n_choose_r[0][0] = 1;
    n_choose_r[1][0] = 1;
    n_choose_r[1][1] = 1;
    for (unsigned n = 2; n <= max_n; n++) {
        n_choose_r[n][0] = 1;
        n_choose_r[n][n] = 1;
        for (unsigned r = 1; r < n; r++)
            n_choose_r[n][r] = n_choose_r[n - 1][r - 1] + n_choose_r[n - 1][r];
    }
}

static uint64_t sumSplit(uint64_t s, uint64_t split, unsigned const nDigits) {
    uint64_t sum = 0;

    while (split > 0) {
        unsigned const slot_nDigits = split % nDigits;
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
    fillAll_n_choose_r(MAX_DIGITS - 1);

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
            computeSplits(splits, nDigits);
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
