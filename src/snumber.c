#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "padkit/repeat.h"

#define MIN_DIGITS  2
#define MAX_DIGITS  20

#define MIN_SLOTS   MIN_DIGITS
#define MAX_SLOTS   MAX_DIGITS

#define MAX_SPLITS  (1 << (MAX_DIGITS - 1)) - 1

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

typedef struct SplitBody {
    int nSlots;
    int slots[MAX_SLOTS];
} Split;

typedef struct SplitStackBody {
    int     nSplits;
    Split   splits[MAX_SPLITS];
} SplitStack;

static void constructInitial_split(Split* const split, int const nDigits) {
    split->nSlots = nDigits;
    for (int slotId = 0; slotId < split->nSlots; slotId++)
        split->slots[slotId] = 1;
}

static int countDigits(uint64_t x) {
    int count = 0;
    do {
        x /= 10;
        count++;
    } while (x > 0);
    return count;
}

static void dumpProof(uint64_t s, uint64_t const x, Split const* const split, int nDigits) {
    printf("√%"PRIu64" = %"PRIu64" = ", s, x);

    uint64_t base = pow_ten[(nDigits -= split->slots[split->nSlots - 1])];
    printf("%.*"PRIu64, split->slots[split->nSlots - 1], s / base);
    s %= base;
    for (int slotId = split->nSlots - 2; slotId >= 0; slotId--) {
        base = pow_ten[(nDigits -= split->slots[slotId])];
        printf(" + %.*"PRIu64, split->slots[slotId], s / base);
        s %= base;
    }
    puts("");
}

static uint64_t sumSplit(Split const* const split, uint64_t s) {
    uint64_t sum = 0;
    for (int slotId = 0; slotId < split->nSlots; slotId++) {
        uint64_t const base = pow_ten[split->slots[slotId]];
        sum += s % base;
        s /= base;
    }
    return sum;
}

int main(int argc, char* argv[]) {
    puts("");
    if (argc < 2) {
        printf(" Usage: %s <N>\n\n", argv[0]);
        return EXIT_SUCCESS;
    }
    uint64_t max_s;
    if (sscanf(argv[1], "%"SCNu64, &max_s) != 1) {
        printf(" N must be a positive integer!\n\n");
        return EXIT_SUCCESS;
    }

    SplitStack* const stack = malloc(sizeof(SplitStack));
    if (stack == NULL) {
        fputs("malloc() error\n\n", stderr);
        return EXIT_FAILURE;
    }

    uint64_t t = 0;
    uint64_t s;
    for (uint64_t x = 4; (s = x*x) <= max_s; x++) {
        int const nDigits = countDigits(s);

        constructInitial_split(stack->splits, nDigits);
        stack->nSplits = 1;

        while (stack->nSplits > 0) {
            Split* const split = stack->splits + --stack->nSplits;

            uint64_t const sum = sumSplit(split, s);
            if (sum == x) {
                dumpProof(s, x, split, nDigits);
                t += s;
                break;
            } else if (sum < x && split->nSlots > MIN_SLOTS) {
                Split oldSplit[1];
                memcpy(oldSplit, split, sizeof(Split));

                int largestSlotValue    = oldSplit->slots[0];
                int largestSlotId       = 0;
                for (int slotId = 1; slotId < oldSplit->nSlots; slotId++)
                    if (largestSlotValue <= oldSplit->slots[slotId])
                        largestSlotValue = oldSplit->slots[(largestSlotId = slotId)];

                if (largestSlotValue == 1)
                    largestSlotId = 0;

                for (int pairId = largestSlotId; pairId < oldSplit->nSlots - 1; pairId++) {
                    Split* const nextSplit = stack->splits + stack->nSplits++;

                    memcpy(nextSplit, oldSplit, sizeof(Split));
                    nextSplit->nSlots--;
                    nextSplit->slots[pairId] += oldSplit->slots[pairId + 1];
                    memcpy(
                        nextSplit->slots + pairId + 1,
                        oldSplit->slots + pairId + 2,
                        (size_t)(nextSplit->nSlots - pairId) * sizeof(int)
                    );
                }
            }
        }
    }

    free(stack);

    printf("\nT = %"PRIu64"\n\n", t);

    return EXIT_SUCCESS;
}
