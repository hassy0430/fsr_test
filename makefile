CC := gcc
CFLAGS := -g -Wall -Wextra

all:
	$CC fsr_test.o k2fsr_a.o k2_lib.o -o fsr_test $CFLAGS

fsr_test.o:
	$CC fsr_test.c -c -o fsr_test.o

k2fsr_a.o:
	$CC k2fsr_a.c -c -o k2fsr_a.o

k2_lib.o:
	$CC k2_lib.c -c -o k2_lib.o

clean:
	$(RM) *.o fsr_test
