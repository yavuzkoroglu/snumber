OBJECTS=obj/snumber.o
CC=cc

all: bin/snumber

.PHONY: all clean cleanobjects cleanpadkit objects

bin: ; mkdir bin

SILENCED_WARNINGS=-Wno-nullability-completeness

bin/snumber:	\
    bin   		\
    objects		\
	; ${CC} -std=c99 -Ofast -DNDEBUG -Wall -Wextra ${SILENCED_WARNINGS} ${OBJECTS} -o bin/snumber

clean: ; rm -rf obj bin

cleanobjects: ; rm -rf obj

obj: ; mkdir obj

obj/snumber.o: obj	\
    src/snumber.c   \
    ; ${CC} -std=c99 -Ofast -DNDEBUG -Wall -Wextra ${SILENCED_WARNINGS} src/snumber.c -c -o obj/snumber.o

objects: cleanobjects ${OBJECTS}
