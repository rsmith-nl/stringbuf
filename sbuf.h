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
