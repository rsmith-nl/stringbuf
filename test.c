// file: test.c
// vim:fileencoding=utf-8:ft=c:tabstop=2
// This is free and unencumbered software released into the public domain.
//
// Author: R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: Unlicense
// Created: 2026-02-22 10:49:54 +0100
// Last modified: 2026-10-24T21:52:25+0200

#include "sbuf.h"

#include <stdbool.h>
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
      fprintf(stderr, "DEBUG: buf.data = \"%s\"\n", buf.data); \
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
  puts(PURPLE"Starting the tests."RESET);
  puts(CYAN"Using sbuf_append. "RESET);
  sbuf_append(&buf, first, strlen(first));
  pass_fail(buf.error == false);
  pass_fail(strcmp(buf.data, first) == 0);
  puts(CYAN"Using sbuf_reset. "RESET);
  sbuf_reset(&buf);
  pass_fail(buf.error == false);
  pass_fail(buf.used == 0);
  puts(CYAN"Using sbuf_appendc. "RESET);
  sbuf_reset(&buf);
  sbuf_appendc(&buf, '!');
  pass_fail(buf.error == false);
  pass_fail(strcmp(buf.data, "!") == 0);
  puts(CYAN"Using sbuf_appends. "RESET);
  sbuf_reset(&buf);
  sbuf_appends(&buf, second);
  pass_fail(buf.error == false);
  pass_fail(strcmp(buf.data, second) == 0);
  puts(CYAN"Using sbuf_appendi. "RESET);
  sbuf_reset(&buf);
  sbuf_appendi(&buf, 42371);
  pass_fail(buf.error == false);
  pass_fail(strcmp(buf.data, "42371") == 0);
  printf("buf.data[0] = %d\n", buf.data[0]);
  sbuf_reset(&buf);
  sbuf_appendi(&buf, 9223372036854775807L);
  pass_fail(buf.error == false);
  pass_fail(strcmp(buf.data, "9223372036854775807") == 0);
  puts(CYAN"Using sbuf_appendd. "RESET);
  sbuf_reset(&buf);
  sbuf_appendd(&buf, 2.27e-4);
  pass_fail(buf.error == false);
  pass_fail(strcmp(buf.data, "2.27e-4") == 0);
  sbuf_reset(&buf);
  sbuf_appendd(&buf, 1.23456789e+12);
  pass_fail(buf.error == false);
  pass_fail(strcmp(buf.data, "1.234567889e+12") == 0);
  sbuf_reset(&buf);
  sbuf_appendd(&buf, 457.6);
  pass_fail(strcmp(buf.data, "457.6") == 0);
  sbuf_reset(&buf);
  buf.decsep = ',';
  sbuf_appendd(&buf, 31.5);
  pass_fail(strcmp(buf.data, "31,5") == 0);
  puts(CYAN"Using sbuf_find. "RESET);
  sbuf_reset(&buf);
  sbuf_appends(&buf, "this is a test");
  pass_fail(sbuf_find(&buf, "is", 0) == 2);
  pass_fail(sbuf_find(&buf, "is", 4) == 5);
  pass_fail(sbuf_find(&buf, "a", 0) == 8);
  puts(CYAN"Using sbuf_replace_char. "RESET);
  sbuf_replace_char(&buf, ' ', '_');
  pass_fail(buf.data[4] == '_' && buf.data[7] == '_' && buf.data[9] == '_');
  puts(CYAN"Using sbuf_ascii_lower. "RESET);
  sbuf_reset(&buf);
  sbuf_appends(&buf, "UPPER ñó ãßçìî");
  sbuf_ascii_lower(&buf);
  pass_fail(strcmp(buf.data, "upper ñó ãßçìî") == 0);
  puts(CYAN"Using sbuf_replace_string. "RESET);
  sbuf_reset(&buf);
  sbuf_appends(&buf, "one two three");
  sbuf_replace_string(&buf, "two", "f00");
  pass_fail(strcmp(buf.data, "one f00 three") == 0);
  sbuf_reset(&buf);
  sbuf_appends(&buf, "one two three");
  sbuf_replace_string(&buf, "one", "f00");
  pass_fail(strcmp(buf.data, "f00 two three") == 0);
  sbuf_reset(&buf);
  sbuf_appends(&buf, "one two three");
  sbuf_replace_string(&buf, "three", "f00");
  pass_fail(strcmp(buf.data, "one two f00") == 0);
  sbuf_reset(&buf);
  sbuf_appends(&buf, "one two three");
  sbuf_replace_string(&buf, "one", "test");
  pass_fail(strcmp(buf.data, "test two three") == 0);
  sbuf_reset(&buf);
  sbuf_appends(&buf, "one two three");
  sbuf_replace_string(&buf, "three", "test");
  sbuf_reset(&buf);
  sbuf_appends(&buf, "one two three");
  sbuf_replace_string(&buf, "two", "2");
  pass_fail(strcmp(buf.data, "one 2 three") == 0);
  sbuf_reset(&buf);
  sbuf_appends(&buf, "one two three");
  sbuf_replace_string(&buf, "one", "1");
  pass_fail(strcmp(buf.data, "1 two three") == 0);
  sbuf_reset(&buf);
  sbuf_appends(&buf, "one two three");
  sbuf_replace_string(&buf, "three", "3");
  pass_fail(strcmp(buf.data, "one two 3") == 0);
  puts(CYAN"Using sbuf_*strip. "RESET);
  sbuf_reset(&buf);
  sbuf_appends(&buf, "right  ");
  sbuf_rstrip(&buf);
  pass_fail(strcmp(buf.data, "right") == 0);
  sbuf_reset(&buf);
  sbuf_appends(&buf, "  left");
  sbuf_lstrip(&buf);
  pass_fail(strcmp(buf.data, "left") == 0);
  sbuf_reset(&buf);
  sbuf_appends(&buf, "  both    ");
  sbuf_strip(&buf);
  pass_fail(strcmp(buf.data, "both") == 0);
  puts(PURPLE"*** Result ***"RESET);
  if (failcount == 0) {
    puts(GREEN"+++ All tests PASSED! +++"RESET);
  } else {
    printf("\033[1;31m--- %d failing tests.---\033[0m\n", failcount);
  }
  return failcount;
}
