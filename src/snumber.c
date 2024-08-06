#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "padkit/repeat.h"

#define MIN_DIGITS  2
#define MAX_DIGITS  20

#define MIN_SLOTS   MIN_DIGITS
#define MAX_SLOTS   MAX_DIGITS

#define MAX_SPLITS  (1 << (MAX_DIGITS - 1)) - 1

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

static void dumpProof(uint64_t s, uint64_t const x, Split const* const split, int const nDigits) {
    printf("√%"PRIu64" = %"PRIu64" = ", s, x);

    uint64_t base = 1;
    REPEAT(nDigits) base *= 10;
    REPEAT(split->slots[split->nSlots - 1]) base /= 10;
    printf("%"PRIu64, s / base);
    s %= base;
    for (int slotId = split->nSlots - 2; slotId >= 0; slotId--) {
        REPEAT(split->slots[slotId]) base /= 10;
        printf(" + %.*"PRIu64, split->slots[slotId], s / base);
        s %= base;
    }
    puts("");
}

static uint64_t sumSplit(Split const* const split, uint64_t s) {
    uint64_t sum = 0;
    for (int slotId = split->nSlots - 1; slotId >= 0; slotId--) {
        uint64_t base = 1;
        REPEAT(split->slots[slotId]) base *= 10;
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

                for (int pairId = 0; pairId < oldSplit->nSlots - 1; pairId++) {
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
