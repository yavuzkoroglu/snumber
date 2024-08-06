include padkit/compile.mk

INCLUDES=-Iinclude -Ipadkit/include
OBJECTS=obj/snumber.o
MODE=release

all: bin/snumber

.PHONY: all clean cleanobjects cleanpadkit documentation objects

bin: ; mkdir bin

bin/snumber:                      		\
    bin                                 \
    objects                             \
    padkit/compile.mk                   \
	; ${COMPILE} ${OBJECTS} -o bin/snumber

clean: ; rm -rf obj bin padkit *.gcno *.gcda *.gcov html latex

cleanlibpadkit: ; rm -rf padkit/obj padkit/lib/libpadkit.a

cleanobjects: ; rm -rf obj

documentation: ; doxygen

obj: ; mkdir obj

obj/snumber.o: obj           			\
    padkit/include/padkit/debug.h       \
    src/snumber.c                 		\
    ; ${COMPILE} ${INCLUDES} src/snumber.c -c -o obj/snumber.o

objects: cleanobjects ${OBJECTS}

padkit: ; git clone https://github.com/yavuzkoroglu/padkit.git

padkit/compile.mk: padkit; $(make padkit/compile.mk)

padkit/include/padkit.h: padkit; make -C padkit include/padkit.h

padkit/lib/libpadkit.a: cleanlibpadkit padkit; make -C padkit lib/libpadkit.a
