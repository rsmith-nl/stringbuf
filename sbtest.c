// file: sbtest.c
// vim:fileencoding=utf-8:ft=c:tabstop=2
// This is free and unencumbered software released into the public domain.
//
// Author: R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: Unlicense
// Created: 2026-02-22 10:49:54 +0100
// Last modified: 2026-02-22T11:17:54+0100

#include "sbuf.h"

#include <stdio.h>
#include <string.h>

void pass_fail(Sbuf *buf)
{
  if (buf->error == false) {
    puts("PASS.");
  } else {
    puts("FAIL.");
  }
}

int main(int argc, char *argv[])
{
  (void)argc;
  (void)argv;
  Sbuf buf = {0};
  char *first = "fixed string.\n";
  char *second = "another fixed string.\n";
  char *third = "format string. Remaining: %d bytes\n";
  printf("Using sbuf_append... ");
  sbuf_append(&buf, first, strlen(first));
  pass_fail(&buf);
  printf("Using sbuf_appends... ");
  sbuf_appends(&buf, second);
  pass_fail(&buf);
  printf("Using sbuf_printf... ");
  sbuf_printf(&buf, third, (int)sbuf_remaining(&buf));
  pass_fail(&buf);
  puts("----------");
  sbuf_fputs(&buf, stdout);
  return 0;
}
