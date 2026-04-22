# vim:fileencoding=utf-8:ft=make
# Use as many jobs as the computer has cores.
.MAKEFLAGS: -j C

CFLAGS = -pipe -g -std=c11 -Wall -Wextra -Wstrict-prototypes -Wpedantic \
         -Wshadow -Wmissing-field-initializers -Wpointer-arith

all: single_header/sbuf.h test

test: test.c single_header/sbuf.h  ## builds the test program (default).
	$(CC) $(CFLAGS) -o test test.c -lm

single_header/sbuf.h: sbuf.c sbuf.h
	cp sbuf.h single_header/sbuf.h
	echo "" >>single_header/sbuf.h
	echo "#ifdef SBUF_IMPLEMENTATION" >>single_header/sbuf.h
	tail -n +11 sbuf.c >>single_header/sbuf.h
	echo "" >>single_header/sbuf.h
	echo "#endif // SBUF_IMPLEMENTATION" >>single_header/sbuf.h

.PHONY: clean
clean:  ## Remove all generated files.
	rm -f *.o test single_header/sbuf.h

.PHONY: style
style:  ## Reformat source code using astyle.
	astyle -n --style=1tbs -s2 -p --indent-switches --delete-empty-lines --add-braces *.c *.h

.PHONY: tidy
tidy:  ## Check the code with clang-tidy
	clang-tidy19 --quiet *.c *.h 2>/dev/null

.PHONY: help
help:  ## List available commands
	@echo "make targets:"
	@sed -n -e '/##/s/:.*\#\#/\t/p' Makefile
	@echo
