#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DIGITS                      20

#define COUNT_GROUPS(nDigits)           ((nDigits) - 1)

#define MAX_GROUPS                      COUNT_GROUPS(MAX_DIGITS)

#define COUNT_SLOTS(nDigits, groupId)   ((nDigits) - (groupId))

static uint64_t const pow_ten[MAX_DIGITS] = {
    1ULL,                   /* 10^0 */
    10ULL,                  /* 10^1 */
    100ULL,                 /* 10^2 */
    1000ULL,                /* 10^3 */
    10000ULL,               /* 10^4 */
    100000ULL,              /* 10^5 */
    1000000ULL,             /* 10^6 */
    10000000ULL,            /* 10^7 */
    100000000ULL,           /* 10^8 */
    1000000000ULL,          /* 10^9 */
    10000000000ULL,         /* 10^10 */
    100000000000ULL,        /* 10^11 */
    1000000000000ULL,       /* 10^12 */
    10000000000000ULL,      /* 10^13 */
    100000000000000ULL,     /* 10^14 */
    1000000000000000ULL,    /* 10^15 */
    10000000000000000ULL,   /* 10^16 */
    100000000000000000ULL,  /* 10^17 */
    1000000000000000000ULL, /* 10^18 */
    10000000000000000000ULL /* 10^19 */
};

static int const binomial[MAX_DIGITS + 1][MAX_GROUPS] = {
    {0},                                                                                        /* 0 */
    {1},                                                                                        /* 1 */
    {1,1},                                                                                      /* 2 */
    {1,2,1},                                                                                    /* 3 */
    {1,3,3,1},                                                                                  /* 4 */
    {1,4,6,4,1},                                                                                /* 5 */
    {1,5,10,10,5,1},                                                                            /* 6 */
    {1,6,15,20,15,6,1},                                                                         /* 7 */
    {1,7,21,35,35,21,7,1},                                                                      /* 8 */
    {1,8,28,56,70,56,28,8,1},                                                                   /* 9 */
    {1,9,36,84,126,126,84,36,9,1},                                                              /* 10 */
    {1,10,45,120,210,252,210,120,45,10,1},                                                      /* 11 */
    {1,11,55,165,330,462,462,330,165,55,11,1},                                                  /* 12 */
    {1,12,66,220,495,792,924,792,495,220,66,12,1},                                              /* 13 */
    {1,13,78,286,715,1287,1716,1716,1287,715,286,78,13,1},                                      /* 14 */
    {1,14,91,364,1001,2002,3003,3432,3003,2002,1001,364,91,14,1},                               /* 15 */
    {1,15,105,455,1365,3003,5005,6435,6435,5005,3003,1365,455,105,15,1},                        /* 16 */
    {1,16,120,560,1820,4368,8008,11440,12870,11440,8008,4368,1820,560,120,16,1},                /* 17 */
    {1,17,136,680,2380,6188,12376,19448,24310,24310,19448,12376,6188,2380,680,136,17,1},        /* 18 */
    {1,18,153,816,3060,8568,18564,31824,43758,48620,43758,31824,18564,8568,3060,816,153,18,1},  /* 19 */
    {1,19,171,969,3876,11628,27132,50388,75582,92378,75582,50388,27132,11628,3876,969,171,19,1} /* 20 */
};

#define COUNT_SPLITS(nDigits, groupId)  (binomial[nDigits][groupId])

typedef struct SplitBody {
    uint64_t    sum;
    int*        slots;
} Split;

static Split** computeSplits(int const nDigits) {
    int const nGroups   = COUNT_GROUPS(nDigits);
    size_t const gp_sz  = (size_t)nGroups * sizeof(Split*);
    Split** splits      = malloc(gp_sz);

    /* COUNT_SPLITS(nDigits, 0) = 1 */
    splits[0] = malloc(sizeof(Split));

    /* COUNT_SLOTS(nDigits, 0) = nDigits */
    splits[0][0].slots = malloc((size_t)nDigits * sizeof(int));

    /* Initially, every slot contains 1 digit. */
    for (int slotId = 0; slotId < nDigits; slotId++)
        splits[0][0].slots[slotId] = 1;

    for (int groupId = 1; groupId < nGroups; groupId++) {
        int const old_groupId   = groupId - 1;
        int const old_nSplits   = COUNT_SPLITS(nDigits, groupId - 1);
        int const nSplits       = COUNT_SPLITS(nDigits, groupId);
        size_t const sp_sz      = (size_t)nSplits * sizeof(Split);
        splits[groupId]         = malloc(sp_sz);

        int const old_nSlots    = COUNT_SLOTS(nDigits, groupId - 1);
        int const nSlots        = COUNT_SLOTS(nDigits, groupId);
        size_t const sl_sz      = (size_t)nSlots * sizeof(int);
        for (int splitId = 0; splitId < nSplits; splitId++)
            splits[groupId][splitId].slots = malloc(sl_sz);

        for (
            int splitId = 0, old_splitId = 0, mergeId = 0;
            splitId < nSplits;
            splitId++, old_splitId = (old_splitId + 1) % old_nSplits, mergeId = (mergeId + 1) % (old_nSlots - 1)
        ) {
            memcpy(
                splits[groupId][splitId].slots,
                splits[old_groupId][old_splitId].slots,
                (size_t)mergeId * sizeof(int)
            );
            splits[groupId][splitId].slots[mergeId] =
                splits[old_groupId][old_splitId].slots[mergeId] +
                splits[old_groupId][old_splitId].slots[mergeId + 1];
            memcpy(
                splits[groupId][splitId].slots + mergeId + 1,
                splits[old_groupId][old_splitId].slots + mergeId + 2,
                (size_t)(nSlots - mergeId - 1) * sizeof(int)
            );
        }
    }

    return splits;
}

static void computeSumSplit(uint64_t s, Split* const split, int const nSlots) {
    split->sum = 0;
    for (int slotId = 0; slotId < nSlots; slotId++) {
        int const nDigits   = split->slots[slotId];
        uint64_t const base = pow_ten[nDigits];
        split->sum += s % base;
        s /= base;
    }
}

static int countDigits(uint64_t const x) {
    if (x == 0) return 1;

    int l = 0;
    int r = MAX_DIGITS;

    while (l < r) {
        int m = (l + r) >> 1;
        if (pow_ten[m] > x) r = m; else l = m + 1;
    }

    return r;
}

static void dumpProof(uint64_t s, uint64_t const x, int const* const slots, int nDigits, int const nSlots) {
    uint64_t base = pow_ten[(nDigits -= slots[nSlots - 1])];
    printf(
        "√(%*"PRIu64") = %*"PRIu64" = %"PRIu64,
        MAX_DIGITS, s,
        MAX_DIGITS >> 1, x,
        s / base
    );
    s %= base;
    for (int slotId = nSlots - 2; slotId >= 0; slotId--) {
        base = pow_ten[(nDigits -= slots[slotId])];
        printf(" + %"PRIu64, s / base);
        s %= base;
    }
    puts("");
}

static void freeSplits(Split** const splits, int const nDigits) {
    int const nGroups = COUNT_GROUPS(nDigits);
    for (int groupId = 0; groupId < nGroups; groupId++) {
        int const nSplits = COUNT_SPLITS(nDigits, groupId);
        for (int splitId = 0; splitId < nSplits; splitId++)
            free(splits[groupId][splitId].slots);

        free(splits[groupId]);
    }

    free(splits);
}

int main(int argc, char* argv[]) {
    uint64_t s, max_s;
    uint64_t t = 0;

    puts("");

    if (argc < 2) {
        printf(" Usage: %s <N>\n\n", argv[0]);
        return EXIT_SUCCESS;
    }
    if (sscanf(argv[1], "%"SCNu64, &max_s) != 1) {
        printf(" N must be a positive integer!\n\n");
        return EXIT_SUCCESS;
    }

    Split** splits  = NULL;
    int nDigits     = 1;
    for (uint64_t x = 4; (s = x*x) <= max_s; x++) {
        int const new_nDigits = countDigits(s);
        if (new_nDigits > nDigits) {
            freeSplits(splits, nDigits);
            nDigits = new_nDigits;
            splits = computeSplits(nDigits);
        }

        computeSumSplit(s, splits[0], nDigits);
        if (splits[0][0].sum == x) {
            int const nSlots = COUNT_SLOTS(nDigits, 0);
            dumpProof(s, x, splits[0][0].slots, nDigits, nSlots);
            t += s;
            continue;
        }

        int const nGroups   = COUNT_GROUPS(nDigits);
        bool proofFound     = 0;
        for (int groupId = 1; groupId < nGroups && !proofFound; groupId++) {
            int const old_groupId   = groupId - 1;
            int const old_nSplits   = COUNT_SPLITS(nDigits, groupId - 1);
            int const nSplits       = COUNT_SPLITS(nDigits, groupId);

            for (
                int splitId = 0, old_splitId = 0;
                splitId < nSplits;
                splitId++, old_splitId = (old_splitId + 1) % old_nSplits
            ) {
                if (splits[old_groupId][old_splitId].sum > x) {
                    splits[groupId][splitId].sum = splits[old_groupId][old_splitId].sum;
                } else {
                    int const nSlots = COUNT_SLOTS(nDigits, groupId);
                    computeSumSplit(s, splits[groupId] + splitId, nSlots);
                    if ((proofFound = (splits[groupId][splitId].sum == x))) {
                        dumpProof(s, x, splits[groupId][splitId].slots, nDigits, nSlots);
                        t += s;
                        break;
                    }
                }
            }
        }
    }
    freeSplits(splits, nDigits);

    printf("\nT = %"PRIu64"\n\n", t);

    return EXIT_SUCCESS;
}

