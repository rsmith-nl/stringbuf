// file: buffer.c
// vim:fileencoding=utf-8:ft=c:tabstop=2
// This is free and unencumbered software released into the public domain.
//
// Author: R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: Unlicense
// Created: 2025-08-28 23:49:02 +0200
// Last modified: 2025-08-29T00:14:10+0200

#include "buffer.h"
#include <assert.h>
#include <string.h>

bool sbuf_append(Sbuf *buf, const char *str, const ptrdiff_t len)
{
  assert(buf);
  ptrdiff_t alen = strnlen(str, len);
  ptrdiff_t remaining = PATH_MAX - buf->used - 1;
  if (len < remaining) {
    memcpy(buf->data+buf->used, str, alen);
    buf->used += alen;
    return true;
  }
  return false;
}

ptrdiff_t sbuf_remaining(Sbuf *buf)
{
  assert(buf);
  ptrdiff_t remaining = PATH_MAX - buf->used - 1;
  return remaining;
}

void sbuf_fputs(Sbuf *buf, FILE* stream)
{
  assert(buf);
  fputs(buf->data, stream);
  fflush(stream);
}
