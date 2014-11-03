/*
 * Little Smalltalk, Version 5
 *
 * Copyright (C) 1987-2005 by Timothy A. Budd
 * Copyright (C) 2007 by Charles R. Childers
 * Copyright (C) 2005-2007 by Danny Reinhold
 * Copyright (C) 2010 by Ketmar // Vampire Avalon
 *
 * ============================================================================
 * This license applies to the virtual machine and to the initial image of
 * the Little Smalltalk system and to all files in the Little Smalltalk
 * packages except the files explicitly licensed with another license(s).
 * ============================================================================
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */
#include "k8lst.h"


#ifndef LST_COMPACT_WRITE_WORD

unsigned int lstImgReadWord (FILE *fp) {
  unsigned int value = 0;
  if (fread(&value, sizeof(value), 1, fp) != 1) lstFatal("error reading image file", 0);
  return value;
}


void lstImgWriteWord (FILE *fp, int i) {
  unsigned int value = i;
  if (i < 0) lstFatal("trying to write out negative value", i);
  if (fwrite(&value, sizeof(value), 1, fp) != 1) lstFatal("error writing image file", 0);
}

int32_t lstImgReadDWord (FILE *fp) {
  int32_t value = 0;
  if (fread(&value, sizeof(value), 1, fp) != 1) lstFatal("error reading image file", 0);
  return value;
}


void lstImgWriteDWord (FILE *fp, int32_t i) {
  if (fwrite(&i, sizeof(i), 1, fp) != 1) lstFatal("error writing image file", 0);
}


#else

unsigned int lstImgReadWord (FILE *fp) {
  unsigned int value = 0;
  int bytes, i = fgetc(fp);
  if (i == EOF) lstFatal("unexpected EOF reading image file", 0);
  if (i <= 252) return i;
  /* We read in network byte order */
  bytes = 1 << (i-252);
  for (bytes = 1 << (i-252); bytes > 0; bytes--) {
    i = fgetc(fp);
    if (i == EOF) lstFatal("unexpected EOF reading image file", 0);
    value = (value<<8)+i;
  }
  return value;
}


void lstImgWriteWord (FILE *fp, int i) {
  int num = i, bytes = 0, power = 0;
  if (i < 0) lstFatal("trying to write out negative value", i);
  /* 0 extra bytes: */
  if (i <= 252) {
    if (fputc(i, fp) == EOF) lstFatal("error writing image file", 0);
    return;
  }
  /* Calculate needed bytes */
  while (num) { bytes++; num >>= 8; }
  if (bytes <= 2) power = 1;
  else if (bytes <= 4) power = 2;
  else if (bytes <= 8) power = 3;
  else lstFatal("lstImgWriteWord: value too big?", bytes);
  /* Write the power */
  if (fputc(252+power, fp) == EOF) lstFatal("error writing image file", 0);
  /* i is written in network byte order = big endian */
  for (bytes = (1<<power) - 1; bytes >= 0; bytes--) {
    if (fputc((i>>(bytes<<3)) & 0xFF, fp) == EOF) lstFatal("error writing image file", 0);
  }
}


int32_t lstImgReadDWord (FILE *fp) {
  int f;
  uint32_t value = 0;
  for (f = 0; f < 4; ++f) {
    uint8_t b;
    if (fread(&b, 1, 1, fp) != 1) lstFatal("error reading image file", 0);
    value <<= 8;
    value |= b;
  }
  return (int32_t)value;
}


void lstImgWriteDWord (FILE *fp, int32_t i) {
  int f;
  uint32_t value = (uint32_t)i;
  uint8_t bytes[4];
  for (f = 3; f >= 0; --f) {
    bytes[f] = value&0xff;
    value >>= 8;
  }
  for (f = 0; f < 4; ++f) if (fwrite(&bytes[f], 1, 1, fp) != 1) lstFatal("error writing image file", 0);
}

#endif
