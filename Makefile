CC=clang

all: bin/snumber

.PHONY: all clean

bin: ; mkdir bin

SILENCED_WARNINGS=-Wno-nullability-completeness -Wno-unsafe-buffer-usage

bin/snumber:		\
    bin   			\
	src/snumber.c	\
	; ${CC} -std=c99 -Ofast -DNDEBUG -Weverything ${SILENCED_WARNINGS} src/snumber.c -o bin/snumber

clean: ; rm -rf obj bin
