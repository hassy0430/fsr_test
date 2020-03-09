CC:= gcc
CFLAGS:= -Wall

all: fsr_test.o k2fsr_a.o k2_lib.o
	$(CC) fsr_test.o k2fsr_a.o k2_lib.o -o fsr_test $(CFLAGS)
fsr_test.o:
	$(CC) fsr_test.c -c -o fsr_test.o $(CFLAGS)

k2fsr_a.o:
	$(CC) k2fsr_a.c -c -o k2fsr_a.o $(CFLAGS)

k2_lib.o:
	$(CC) k2_lib.c -c -o k2_lib.o $(CFLAGS)


clean:
	$(RM) *.o fsr_test
