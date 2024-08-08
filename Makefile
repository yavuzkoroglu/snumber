CC=clang

all: clean bin/snumber

.PHONY: all clean

bin: ; mkdir bin

SILENCED_WARNINGS=-Wno-nullability-completeness -Wno-unsafe-buffer-usage -Wno-declaration-after-statement -Wno-padded

bin/snumber:		\
    bin   			\
	src/snumber.c	\
	; ${CC} -std=c99 -DNDEBUG -Ofast -Weverything ${SILENCED_WARNINGS} src/snumber.c -o bin/snumber

clean: ; rm -rf bin
