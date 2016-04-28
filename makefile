all:
	gcc -m32 -Wall y86emul.c
	gcc y86emul.c -o y86emul
clean:
	\rm -fr y86emul
	\rm -fr a.out
