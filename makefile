1b.out: MainShell.c
	gcc MainShell.c -o 1b.out
all: 1b.out
clean:
	$(RM) 1b.out
