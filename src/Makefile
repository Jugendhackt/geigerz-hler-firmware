NAME=logger

LIBINCLUDE= -Llib
INCLUDE= -Iinclude
LIB= -lnmea -lm -lcurl
COMPILE= gcc $(INCLUDE) $(LIBINCLUDE) $(LIB)

.PHONY: clean

main:	
	$(COMPILE) main.c -o $(NAME) 

all:	main

clean:
	rm -fr *.o $(NAME)
