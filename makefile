# Makefile for TCP project
#we took this from the tirgul

all: sniffer myping

sniffer:sniffer.c
	gcc -o sniffer sniffer.c

myping: myping.c
	gcc -o myping myping.c

clean:
	rm -f *.o sniffer myping

runs:
	./sniffer

runc:
	./myping

runs-strace:
	strace -f ./sniffer

runc-strace:
	strace -f ./myping
