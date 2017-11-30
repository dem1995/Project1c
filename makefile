1b.out: MainShell.c
	gcc MainShell.c -o 1c.out
all: 1c.out
clean:
	$(RM) 1c.out
