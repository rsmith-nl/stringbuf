// file: sbuf.h
// vim:fileencoding=utf-8:ft=c:tabstop=2
// This is free and unencumbered software released into the public domain.
//
// Author: R.F. Smith <rsmith@xs4all.nl>
// SPDX-License-Identifier: Unlicense
// Created: 2025-08-28 23:49:02 +0200
// Last modified: 2026-10-25T02:17:45+0200

// Simple string buffer.
// Mostly conceived for assembling strings.
// Change the definition of SBUF_SIZE if you need longer strings.
// For general allocation, use an arena instead!

#pragma once

#include <stddef.h>  // for ptrdiff_t
#include <stdbool.h> // for bool
#include <stdint.h>
#include <stdio.h>   // for FILE*

#define SBUF_SIZE 1024
#define SBUF_MAX (SBUF_SIZE-1)

typedef struct {
  ptrdiff_t used;   // Amount of characters used.
  bool error;  // Use “error” instead of “ok” so a zerod-out Sbuf is valid.
  char decsep; // Decimal separator for sbuf_appendd. If 0, then "." is used.
  char data[SBUF_SIZE];
} Sbuf;

#ifdef __cplusplus
extern "C" {
#endif

// All appends set “error” to “true” if there is not enough space.
// All appends immediately return if “error” is “true”.

// Appends at most “len” bytes to the buffer from “str”.
extern void sbuf_append(Sbuf *buf, char *str, ptrdiff_t len);

// Appends a character to the buffer.
extern void sbuf_appendc(Sbuf *buf, char c);

// Macro to append a space to the buffer.
#define sbuf_append_space(buf) sbuf_appendc((buf), ' ')

// Appends a null-terminated string to the buffer.
extern void sbuf_appends(Sbuf *buf, char *str);

// Format and append an integer number to “buf”.
extern void sbuf_appendi(Sbuf *buf, int64_t i);

// Format and append a double to buf.
extern void sbuf_appendd(Sbuf *buf, double f);

// Append using snprintf.
extern void sbuf_printf(Sbuf *buf, char *fmt, ...);

// Returns how much space remains in the buffer.
extern ptrdiff_t sbuf_remaining(Sbuf *buf);

// Writes the buffer to the designated stream, and flushes the stream.
extern void sbuf_fputs(Sbuf *buf, FILE* stream);

// Empty the buffer, reset errors.
extern void sbuf_reset(Sbuf *buf);

// Returns the offset of the first occurence of “str” in the buffer
// after the index “start”.
// In case of error or if “str” is not found it returns -1.
extern ptrdiff_t sbuf_lfind(Sbuf *buf, char *str, ptrdiff_t start);

// Returns the offset of the last occurence of “str” in the buffer
// before the index “end”. If “end” is 0, it will be set to the end of the
// buffer.
// In case of error or if “str” is not found it returns -1.
extern ptrdiff_t sbuf_rfind(Sbuf *buf, char *str, ptrdiff_t end);

// Replace a character in the whole buffer.
extern void sbuf_replace_char(Sbuf *buf, char from, char to);

// Replace a string in the whole buffer.
extern void sbuf_replace_string(Sbuf *buf, char *from, char *to);

// Switch ascii characters to lowercase
extern void sbuf_ascii_lower(Sbuf *buf);

// Strip white space on the left, the beginning of the buffer.
extern void sbuf_lstrip(Sbuf *buf);

// Strip white space on the right, the end of the buffer.
extern void sbuf_rstrip(Sbuf *buf);

// Strip white space on both ends of the buffer.
extern void sbuf_strip(Sbuf *buf);

#ifdef __cplusplus
}
#endif

#ifdef SBUF_IMPLEMENTATION
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

void sbuf_append(Sbuf *buf, char *str, ptrdiff_t len)
{
  assert(buf != 0);
  assert(str != 0);
  assert(len >= 0);
  if (buf->error == true) {
    return;
  }
  ptrdiff_t alen = strnlen(str, len);
  ptrdiff_t remaining = SBUF_MAX - buf->used;
  if (len < remaining) {
    memcpy(buf->data + buf->used, str, alen);
    buf->used += alen;
    buf->error = false;
    buf->data[buf->used] = 0;
    buf->data[SBUF_MAX] = 0;
  } else {
    buf->error = true;
  }
}

void sbuf_appendc(Sbuf *buf, char c)
{
  assert(buf != 0);
  if (buf->error == true) {
    return;
  }
  ptrdiff_t remaining = SBUF_MAX - buf->used;
  if (remaining >= 1) {
    buf->data[buf->used++] = c;
    buf->error = false;
    buf->data[buf->used] = 0;
    buf->data[SBUF_MAX] = 0;
  } else {
    buf->error = true;
  }
}

inline void sbuf_appends(Sbuf *buf, char *str)
{
  sbuf_append(buf, str, strnlen(str, SBUF_MAX));
}

extern void sbuf_appendi(Sbuf *buf, int64_t i)
{
  assert(buf != 0);
  if (buf->error == true) {
    return;
  }
#define BUFLENI 32
#define ORD0 48
  int64_t work = 0;
  bool negative = false;
  if (i < 0) {
    work = -i;
    negative = true;
  } else {
    work = i;
  }
  char tmpbuf[BUFLENI] = {0};
  int index = sizeof(tmpbuf) - 1;
  do {
    ldiv_t result = ldiv(work, 10);
    tmpbuf[index--] = ORD0 + result.rem;
    work = result.quot;
  } while (work > 0 && index > 1);
  if (negative) {
    assert(index > 0);
    tmpbuf[index--] = '-';
  }
  index++;
  sbuf_append(buf, tmpbuf + index, strnlen(tmpbuf + index, BUFLENI - index));
}

static double frexp10(double arg, int *exp)
{
  *exp = (arg == 0) ? 0 : (int)floor(log10(fabs(arg)));
  return arg * pow(10, -(*exp));
}

extern void sbuf_appendd(Sbuf *buf, double f)
{
#define BUFLEND 30
#define EXPLEN (BUFLEND-4-1)
  assert(buf != 0);
  if (buf->error != false) {
    return;
  }
  int exp;
  double mantissa = frexp10(f, &exp);
  double am = fabs(mantissa);
  char tbuf[BUFLEND] = {0};
  int bufused = 0;
  if (signbit(f)) {
    tbuf[bufused++] = '-';
  }
  if (isinf(f)) {
    char inf[4] = {0xE2, 0x88, 0x9E, 0}; // UTF-8 for infinity symbol.
    sbuf_append(buf, inf, 4);
    return;
  } else if (mantissa == 0 && exp == 0) {
    tbuf[bufused++] = '0';
    sbuf_append(buf, tbuf, BUFLEND);
    return;
  } else if (isnan(f)) {
    sbuf_append(buf, "NaN", 3);
    return;
  }
  if (exp > 0 && exp < 10) {
    // Format as dddd.dddddd, with 6 decimal places.
    // Format integer part
    int value = (int)f;
    int frac = (int)round((f - (double)value) * 1e6);
    char intbuf[11] = {0};
    int index = sizeof(intbuf) - 2;
    do {
      div_t result = div(value, 10);
      intbuf[index--] = ORD0 + result.rem;
      value = result.quot;
    } while (value > 0 && index > 0);
    strncpy(tbuf + bufused, intbuf + index + 1, BUFLEND - bufused - 1);
    bufused += strlen(intbuf + index + 1);
    if (frac == 0) {
      sbuf_append(buf, tbuf, BUFLEND);
      return;
    }
    if (buf->decsep == 0) {
      tbuf[bufused++] = '.';
    } else {
      tbuf[bufused++] = buf->decsep;
    }
    char fracbuf[8] = {0};
    index = sizeof(fracbuf) - 2;
    do {
      div_t result = div(frac, 10);
      fracbuf[index--] = ORD0 + result.rem;
      frac = result.quot;
    } while (frac > 0 && index > 0);
    // Remove trailing 0 decimals
    int ridx = sizeof(fracbuf) - 3;
    while (fracbuf[ridx] == '0') {
      fracbuf[ridx--] = 0;
    }
    strncpy(tbuf + bufused, fracbuf + index + 1, BUFLEND - bufused - 1);
    sbuf_append(buf, tbuf, BUFLEND);
    return;
  }
  int decimal = (int)floor(am);
  tbuf[bufused++] = ORD0 + decimal;
  am = (am - decimal) * 10.0;
  if (buf->decsep != 0) {
    tbuf[bufused++] = buf->decsep;
  } else {
    tbuf[bufused++] = '.';
  }
  int loopexp = 0;
  do {
    decimal = (int)floor(am);
    tbuf[bufused++] = ORD0 + decimal;
    am = (am - decimal) * 10.0;
    frexp10(am, &loopexp);
    if (decimal == 9 && am > 9.5) {
      break;
    }
  } while (am > 0 && loopexp == 0 && bufused < EXPLEN);
  tbuf[bufused++] = 'e';
  if (exp < 0) {
    tbuf[bufused++] = '-';
    exp = -exp;
  } else {
    tbuf[bufused++] = '+';
  }
  if (exp > 100) {
    tbuf[bufused++] = ORD0 + 1;
    exp -= 100;
  }
  if (exp > 10) {
    tbuf[bufused++] = ORD0 + exp / 10;
    exp -= 10;
  }
  tbuf[bufused++] = ORD0 + exp;
  sbuf_append(buf, tbuf, BUFLEND);
}

void sbuf_printf(Sbuf *buf, char *fmt, ...)
{
  assert(buf != 0);
  assert(fmt != 0);
  if (buf->error == true) {
    return;
  }
  ptrdiff_t remaining = SBUF_SIZE - buf->used - 1;
  va_list ap;
  va_start(ap, fmt);
  ptrdiff_t used = vsnprintf(buf->data + buf->used, remaining, fmt, ap);
  va_end(ap);
  if (used > remaining) { // discard
    memset(buf->data + buf->used, 0, remaining);
    buf->error = true;
  } else {
    buf->error = false;
    buf->used += used;
    buf->data[buf->used] = 0;
    buf->data[SBUF_MAX] = 0;
  }
}

ptrdiff_t sbuf_remaining(Sbuf *buf)
{
  assert(buf != 0);
  ptrdiff_t remaining = SBUF_MAX - buf->used;
  return remaining;
}

void sbuf_fputs(Sbuf *buf, FILE* stream)
{
  assert(buf != 0);
  assert(stream != 0);
  fputs(buf->data, stream);
  fflush(stream);
}

void sbuf_reset(Sbuf *buf)
{
  assert(buf != 0);
  memset(buf->data, 0, SBUF_SIZE);
  buf->used = 0;
  buf->error = false;
}

ptrdiff_t sbuf_lfind(Sbuf *buf, char *str, ptrdiff_t start)
{
  assert(buf != 0);
  if (buf->error == true) {
    return -1;
  }
  if (start < 0) {
    start = 0;
  }
  ptrdiff_t fl = strnlen(str, SBUF_MAX);
  if (buf->used == 0 || fl == 0 || fl > buf->used) {
    return -1;
  }
  for (ptrdiff_t j = start; j < buf->used - fl; j++) {
    if (memcmp(buf->data + j, str, fl) == 0) {
      return j;
    }
  }
  return -1;
}

ptrdiff_t sbuf_rfind(Sbuf *buf, char *str, ptrdiff_t end)
{
  assert(buf != 0);
  if (buf->error == true) {
    return -1;
  }
  ptrdiff_t fl = strnlen(str, SBUF_MAX);
  if (buf->used == 0 || fl == 0 || fl > buf->used) {
    return -1;
  }
  if (end < 0) {
    end = 0;
  } else if (end == 0) {
    end = buf->used - 1 - fl;
  }
  for (ptrdiff_t j = end; j > 0; j--) {
    if (memcmp(buf->data + j, str, fl) == 0) {
      return j;
    }
  }
  return -1;
}

void sbuf_replace_char(Sbuf *buf, char from, char to)
{
  assert(buf != 0);
  if (buf->error == true) {
    return;
  }
  for (ptrdiff_t j = 0; j < buf->used; j++) {
    if (buf->data[j] == from) {
      buf->data[j] = to;
    }
  }
  buf->data[buf->used] = 0;
  buf->data[SBUF_MAX] = 0;
}

void sbuf_replace_string(Sbuf *buf, char *from, char *to)
{
  assert(buf != 0);
  if (buf->error == true) {
    return;
  }
  ptrdiff_t fromlen = strnlen(from, SBUF_MAX);
  if (fromlen > buf->used) {
    return;
  }
  ptrdiff_t tolen = strnlen(to, SBUF_MAX);
  ptrdiff_t offset = tolen - fromlen;
  for (ptrdiff_t j = 0; j <= buf->used - fromlen; j++) {
    if (buf->data[j] == from[0] && strncmp(buf->data + j, from, fromlen) == 0) {
      // Found “from”.
      // Make space if “to” is longer than “from”.
      if (tolen > fromlen) {
        // Return if not enough room is available.
        if (sbuf_remaining(buf) < (tolen - fromlen)) {
          buf->error = true;
          return;
        }
        for (ptrdiff_t k = buf->used + offset; k > j + fromlen; k--) {
          buf->data[k] = buf->data[k - 1];
        }
        buf->used += offset;
      }
      // Replace
      strncpy(buf->data + j, to, tolen);
      // Move remainder if “to” is shorter than “from”.
      if (fromlen > tolen) {
        for (ptrdiff_t k = j + tolen; k < buf->used; k++) {
          buf->data[k] = buf->data[k - offset];
        };
      }
      // Move to past replacement.
      j += tolen;
    }
  }
  buf->data[buf->used] = 0;
  buf->data[SBUF_MAX] = 0;
}


void sbuf_ascii_lower(Sbuf *buf)
{
  assert(buf != 0);
  if (buf->error == true) {
    return;
  }
  for (ptrdiff_t j = 0; j < buf->used; j++) {
    if (buf->data[j] > 64 && buf->data[j] < 91) {
      buf->data[j] += 32;
    }
  }
  buf->data[buf->used] = 0;
  buf->data[SBUF_MAX] = 0;
}

void sbuf_lstrip(Sbuf *buf)
{
  assert(buf != 0);
  if (buf->error == true) {
    return;
  }
  ptrdiff_t to = 0, from = 0;
  for (ptrdiff_t j = 0; j < buf->used; j++) {
    // Check for whitespace.
    if (buf->data[j] == ' ' || buf->data[j] == '\t' || buf->data[j] == '\n' ||
        buf->data[j] == '\r' || buf->data[j] == '\v') {
    } else {
      from = j;
      break;
    }
  }
  ptrdiff_t offset = from - to;
  if (from == to) {
    return;
  }
  for (; from < buf->used; from++, to++) {
    buf->data[to] = buf->data[from];
  }
  for (; to < buf->used; to++) {
    buf->data[to] = 0;
  }
  buf->used -= offset;
  buf->data[buf->used] = 0;
  buf->data[SBUF_MAX] = 0;
}

void sbuf_rstrip(Sbuf *buf)
{
  assert(buf != 0);
  if (buf->error == true) {
    return;
  }
  for (ptrdiff_t j = buf->used - 1; j > 0; j--) {
    // Check for whitespace.
    if (buf->data[j] == ' ' || buf->data[j] == '\t' || buf->data[j] == '\n' ||
        buf->data[j] == '\r' || buf->data[j] == '\v') {
      buf->data[j] = 0;
      buf->used--;
    } else {
      break;
    }
  }
}

void sbuf_strip(Sbuf *buf)
{
  sbuf_rstrip(buf);
  sbuf_lstrip(buf);
}


#endif // SBUF_IMPLEMENTATION
