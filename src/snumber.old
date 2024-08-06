#include <inttypes.h>
#include "padkit/repeat.h"
#include "padkit/stack.h"

#define MIN_DIGITS  2
#define MAX_DIGITS  20

#define MIN_SLOTS   MIN_DIGITS
#define MAX_SLOTS   MAX_DIGITS

#define MAX_SPLITS  ((uint32_t)1 << (MAX_DIGITS - 1)) - 1

typedef struct SplitBody {
    uint64_t sum;
    uint64_t nSlots;
    uint64_t slots[MAX_SLOTS];
} Split;

static uint32_t countDigits(uint64_t x) {
    uint32_t count = 0;
    do {
        x /= 10;
        count++;
    } while (x > 0);
    return count;
}

static uint64_t concat(uint64_t a, uint64_t b) {
    uint32_t const nDigits = countDigits(b);
    REPEAT(nDigits) a *= 10;
    return a + b;
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

    CREATE_EMPTY_STACK(Split, splits, MAX_SPLITS)

    uint64_t t = 0;
    uint64_t s;
    for (uint64_t x = 4; (s = x*x) <= max_s; x++) {
        uint32_t const nDigits = countDigits(s);
        DEBUG_ASSERT(MIN_DIGITS <= nDigits && nDigits <= MAX_DIGITS)

        PUSH_STACK_N(Split, Split* split, splits)

        split->nSlots = nDigits;
        for (uint64_t slotId = 0; slotId < split->nSlots; slotId++) {
            split->sum += (split->slots[slotId] = s % 10);
            s /= 10;
        }

        while (splits_size > 0) {
            POP_STACK_V(split, splits)
            DEBUG_ASSERT(MIN_SLOTS <= split->nSlots && split->nSlots <= MAX_SLOTS)

            if (split->sum == x) {
                printf("√%"PRIu64" = %"PRIu64, (s=x*x), split->slots[split->nSlots - 1]);
                for (uint64_t slotId = split->nSlots - 2; slotId != UINT64_MAX; slotId--) {
                    printf(" + %"PRIu64, split->slots[slotId]);
                }
                puts("");
                t += s;
                break;
            } else if (split->sum < x && split->nSlots > MIN_SLOTS) {
                Split oldSplit[1];
                memcpy(oldSplit, split, sizeof(Split));

                for (uint64_t pairId = 0; pairId < oldSplit->nSlots - 1; pairId++) {
                    PUSH_STACK_N(Split, Split* const nextSplit, splits)

                    nextSplit->nSlots = oldSplit->nSlots - 1;
                    for (uint64_t i = 0, j = 0; i < oldSplit->nSlots && j < nextSplit->nSlots; i++, j++) {
                        if (i == pairId) {
                            nextSplit->sum += (nextSplit->slots[j] = concat(oldSplit->slots[i + 1], oldSplit->slots[i]));
                            i++;
                        } else {
                            nextSplit->sum += (nextSplit->slots[j] = oldSplit->slots[i]);
                        }
                    }
                }
            }
        }

        FLUSH_STACK(splits)
    }

    FREE_STACK(splits)

    printf("\nT = %"PRIu64"\n\n", t);

    return EXIT_SUCCESS;
}
