# snumber
Finds perfect squares whose digits, when they are splitted and added, gives its square root.

Thanks to my friend Lars for the original problem.

## The Problem

An S-number is a perfect square whose root could be found by splitting it into smaller integers and summing them up.

### Examples

81 is an S-number because 81 = 9^2 = (8 + 1)^2

100 is an S-number because 100 = 10^2 = (10 + 0)^2

6724 is an S-number because 6724 = 82^2 = (6 + 72 + 4)^2

8281 is an S-number because 8281 = 91^2 = (82 + 8 + 2)^2

9801 is an S-number because 9801 = 99^2 = (98 + 0 + 1)^2

### The Question

Let T(N) be the sum of all S-numbers up to N. T(10^(12)) = ? 

### The Answer

This implementation proved to be too slow to compute the answer.

## How to Build

```make```

## Example Run at N = 10^4

```bin/snumbers 10000```

## The Output at N = 10^4

```

√81 = 9 = 8 + 1
√100 = 10 = 10 + 0
√1296 = 36 = 1 + 29 + 6
√2025 = 45 = 20 + 25
√3025 = 55 = 30 + 25
√6724 = 82 = 6 + 72 + 4
√8281 = 91 = 82 + 8 + 1
√9801 = 99 = 98 + 0 + 1
√10000 = 100 = 100 + 0 + 0

T = 41333

```

