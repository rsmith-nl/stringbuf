CFLAGS = -pipe -std=c11 -Wall -Wextra -Wstrict-prototypes -Wpedantic \
                -Wshadow -Wmissing-field-initializers -Wpointer-arith

check:  ## checks if the code builds cleanly. (default)
	$(CC) $(CFLAGS) -c sbuf.c
	rm -f *.o

sbtest: sbtest.c sbuf.c sbuf.h  ## builds the test program.
	$(CC) $(CFLAGS) -o sbtest sbtest.c sbuf.c

.PHONY: clean
clean:
	rm -f *.o sbtest

.PHONY: style
style:  ## Reformat source code using astyle.
	astyle -n *.c *.h

.PHONY: tidy
tidy:  ## Check the code with clang-tidy
	clang-tidy19 --quiet *.c *.h 2>/dev/null

.PHONY: help
help:  ## List available commands
	@echo "make targets:"
	@sed -n -e '/##/s/:.*\#\#/\t/p' Makefile
	@echo
