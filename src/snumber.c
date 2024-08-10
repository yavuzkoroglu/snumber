#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* An 64-bit unsigned integer can have at most 20 digits. */
#define MAX_DIGITS (20)

#define COUNT_SPLITS(nDigits) (1 << (nDigits - 1))

/*
static char const digitCode[] = "0123456789ABCDEFGHIJ";
*/

static uint64_t const pow_10[MAX_DIGITS] = {
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

static uint8_t countDigits(uint64_t const x);
static void dumpSplit(uint64_t s, uint8_t* const split);
int main(int argc, char* argv[]);
static uint64_t sumSplit(uint64_t s, uint8_t* const split);

static uint8_t countDigits(uint64_t const x) {
    uint8_t l = 0, r = MAX_DIGITS;
    while (l < r) {
        uint8_t const m = (l + r) >> 1;
        if (pow_10[m] > x) r = m; else l = m + 1;
    }

    return r;
}

static void dumpSplit(uint64_t s, uint8_t* const split) {
    uint64_t slots[MAX_DIGITS + 1];
    for (uint8_t slotId = 1; slotId <= split[0]; slotId++) {
        slots[slotId] = s % pow_10[split[slotId]];
        s /= pow_10[split[slotId]];
    }
    printf("%0*"PRIu64, split[split[0]], slots[split[0]]);
    for (uint8_t slotId = split[0] - 1; slotId > 0; slotId--)
        printf(" + %0*"PRIu64, split[slotId], slots[slotId]);
    puts("");
}

int main(int argc, char* argv[]) {
    puts("");

    if (argc < 2) {
        printf(" Usage: %s <N>\n\n", argv[0]);
        return EXIT_SUCCESS;
    }

    uint64_t max_s;
    sscanf(argv[1], "%"SCNu64, &max_s);

    uint8_t const max_nDigits = countDigits(max_s);

    /* Allocate enough memory for all splits. */
    uint8_t*** splits = malloc((size_t)(max_nDigits + 1) * sizeof(uint8_t**));

    /* A 0-digit number cannot be splitted. */
    splits[0] = NULL;

    /* A 1-digit number has only one split. */
    splits[1] = malloc(sizeof(uint8_t*));

    /* A split is formatted as the number of slots,
       and then the number of digits located in every slot. */
    splits[1][0] = malloc(sizeof(uint8_t) << 1);

    /* A 1-digit split has only one slot. */
    splits[1][0][0] = 1;

    /* And that slot contains that digit! */
    splits[1][0][1] = 1;

    /* Compute all splits */
    uint32_t prev_nSplits = 1;
    for (uint8_t nDigits = 2; nDigits <= max_nDigits; nDigits++) {
        uint32_t const nSplits = prev_nSplits << 1;

        splits[nDigits] = malloc((size_t)nSplits * sizeof(uint8_t*));
        for (
            uint32_t splitId = 0, splitId2 = prev_nSplits;
            splitId < prev_nSplits;
            splitId++, splitId2++
        ) {
            uint8_t const prev_nSlots   = splits[nDigits - 1][splitId][0];
            splits[nDigits][splitId]    = malloc((size_t)(prev_nSlots + 2) * sizeof(uint8_t));
            splits[nDigits][splitId2]   = malloc((size_t)(prev_nSlots + 1) * sizeof(uint8_t));
            memcpy( splits[nDigits][splitId], splits[nDigits - 1][splitId], prev_nSlots + 1);
            memcpy(splits[nDigits][splitId2], splits[nDigits - 1][splitId], prev_nSlots + 1);
            splits[nDigits][splitId][0]++;
            splits[nDigits][splitId][prev_nSlots + 1] = 1;
            splits[nDigits][splitId2][prev_nSlots]++;
        }

        prev_nSplits = nSplits;
    }

    /*
    for (uint8_t nDigits = 2; nDigits <= max_nDigits; nDigits++) {
        uint32_t const nSplits = COUNT_SPLITS(nDigits);

        printf("nDigits = %"PRIu8"\n", nDigits);
        for (uint32_t splitId = 0; splitId < nSplits; splitId++) {
            for (uint8_t slotId = splits[nDigits][splitId][0]; slotId > 0; slotId--) {
                printf("%c", digitCode[splits[nDigits][splitId][slotId]]);
            }
            puts("");
        }
        puts("");
    }
    */

    /* Check all perfect squares. */
    uint64_t t = 0;
    for (uint64_t s, x = 4; (s = x*x) <= max_s; x++) {
        uint32_t const nDigits  = countDigits(s);
        uint32_t const nSplits  = COUNT_SPLITS(nDigits);

        for (uint32_t splitId = 0; splitId < nSplits - 1; splitId++) {
            if (x != sumSplit(s, splits[nDigits][splitId]))
                continue;

            printf("√%*"PRIu64" = %*"PRIu64" = ", max_nDigits, s, max_nDigits >> 1, x);
            dumpSplit(s, splits[nDigits][splitId]);
            t += s;
            break;
        }
    }

    /* Free all allocated memory. */
    for (uint32_t nDigits = 1, nSplits = 1; nDigits <= max_nDigits; nDigits++, nSplits <<= 1) {
        for (uint32_t splitId = 0; splitId < nSplits; splitId++)
            free(splits[nDigits][splitId]);
        free(splits[nDigits]);
    }
    free(splits);

    printf("\n T = %"PRIu64"\n\n", t);

    return EXIT_SUCCESS;
}

static uint64_t sumSplit(uint64_t s, uint8_t* const split) {
    uint64_t sum = 0;
    for (uint32_t slotId = 1; slotId <= split[0]; slotId++) {
        sum += s % pow_10[split[slotId]];
        s /= pow_10[split[slotId]];
    }
    return sum;
}
