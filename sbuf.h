// file: buffer.h
// vim:fileencoding=utf-8:ft=c:tabstop=2
// This is free and unencumbered software released into the public domain.
//
// Author: R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: Unlicense
// Created: 2025-08-28 23:49:02 +0200
// Last modified: 2025-08-29T00:14:09+0200

#pragma once

#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <limits.h> // PATH_MAX

typedef struct {
  ptrdiff_t used;
  char data[PATH_MAX];
} Sbuf;

// Appends at most “len” bytes to a buffer from “str”.
// Returns “true” if succesful, “false” if there is not enough space.
extern bool sbuf_append(Sbuf *buf, const char *str, const ptrdiff_t len);
extern ptrdiff_t sbuf_remaining(Sbuf *buf);
extern void sbuf_fputs(Sbuf *buf, FILE* stream);
