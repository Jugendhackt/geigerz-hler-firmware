OBJECTS = src/main.o

LIBINCLUDE = -Lsrc/lib
LIB = -lcurl -lnmea -lm
INCLUDE = -Isrc/include
COMPILE = gcc

all:	main

%.o:	%.c
	$(COMPILE) -c $< -o $@ $(LIBINCLUDE) $(LIB) $(INCLUDE)

clean:
	rm -f main $(OBJECTS)

main: $(OBJECTS)
	$(COMPILE) -o main $(OBJECTS) $(LIBINCLUDE) $(LIB) $(INCLUDE)
