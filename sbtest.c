// file: sbtest.c
// vim:fileencoding=utf-8:ft=c:tabstop=2
// This is free and unencumbered software released into the public domain.
//
// Author: R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: Unlicense
// Created: 2026-02-22 10:49:54 +0100
// Last modified: 2026-03-16T23:00:28+0100

#include "sbuf.h"

#include <stdio.h>
#include <string.h>

#define BOLD_WHITE "\033[1;37m"
#define CYAN "\033[0;36m"
#define GREEN "\033[0;32m"
#define BOLD_GREEN "\033[1;32m"
#define PURPLE "\033[0;35m"
#define BOLD_RED "\033[1;31m"
#define RED "\033[0;31m"
#define YELLOW "\033[0;33m"
#define BOLD_YELLOW "\033[1;33m"
#define RESET "\033[0m"  // No Color

#define pass_fail(cond) \
  do \
    if ((cond)) { \
      printf("\033[0;32mPASSED:\033[0m " #cond "\n"); \
    } else { \
      printf("\033[1;31mFAILED:\033[0m " #cond "\n"); \
      failcount++; \
    } \
  while (0)

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  int failcount = 0;
  Sbuf buf = {0};
  char *first = "fixed string.\n";
  char *second = "another fixed string.\n";
  char *third = "format string. Remaining: %d bytes\n";
  puts(PURPLE"Starting the tests."RESET);
  puts(CYAN"Using sbuf_append... "RESET);
  sbuf_append(&buf, first, strlen(first));
  pass_fail(buf.error==false);
  puts(CYAN"Using sbuf_appends... "RESET);
  sbuf_appends(&buf, second);
  pass_fail(buf.error==false);
  puts(CYAN"Using sbuf_printf... "RESET);
  sbuf_printf(&buf, third, (int)sbuf_remaining(&buf));
  pass_fail(buf.error==false);
  puts(CYAN"Contents of the stringbuf:"RESET);
  sbuf_fputs(&buf, stdout);
  puts(CYAN"End of the contents."RESET);
  puts(PURPLE"*** Result ***"RESET);
  if (failcount == 0) {
    puts(GREEN"+++ All tests PASSED! +++"RESET);
  } else {
    printf("\033[1;31m--- %d failing tests.---\033[0m\n", failcount);
  }
  return failcount;
}
