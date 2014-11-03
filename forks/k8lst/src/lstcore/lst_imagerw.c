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
/* file in and file out of Smalltalk images */
/*#define EXPLOADER*/


static int indirtop;
static lstObject **indirArray;


static lstObject *objectRead (FILE *fp) {
  lstObject *newObj = 0;
  lstByteObject *bnewObj;
  int size, f, val, bcode = 0;
  int type = lstImgReadWord(fp);
  switch (type) {
    case LST_IMGOT_NULL: /* nil obj */
      lstFatal("read in a null object", (intptr_t)newObj);
    case LST_IMGOT_OBJECT: /* ordinary object */
    case LST_IMGOT_WEAK: /* weak object */
      size = lstImgReadWord(fp);
      if (type == LST_IMGOT_WEAK) {
        /* weak objects will be in 'normal' space */
        newObj = lstMemAlloc(size);
        LstFinLink *fi = calloc(1, sizeof(LstFinLink));
        fi->obj = newObj;
        newObj->fin = fi;
        lstAddToFList(&stWeakListHead, fi);
      } else {
#ifndef EXPLOADER
        newObj = lstStaticAlloc(size);
#else
        newObj = lstMemAlloc(size);
#endif
      }
      indirArray[indirtop++] = newObj;
      newObj->stclass = objectRead(fp);
      for (f = 0; f < size; f++) newObj->data[f] = objectRead(fp);
      break;
    case LST_IMGOT_SMALLINT: /* integer */
      val = lstImgReadDWord(fp);
      newObj = lstNewInt(val);
      break;
    case LST_IMGOT_BCODE: /* bytecode byte array */
      /*fprintf(stderr, "BCODE!\n");*/
      bcode = 1;
    case LST_IMGOT_BARRAY: /* byte arrays */
      size = lstImgReadWord(fp);
#ifndef EXPLOADER
      bnewObj = lstStaticAllocBin(size);
#else
      bnewObj = lstMemAllocBin(size);
#endif
      indirArray[indirtop++] = (lstObject *)bnewObj;
      if (size > 0) {
        if (fread(lstBytePtr(bnewObj), size, 1, fp) != 1) lstFatal("error reading byte array", (intptr_t)bnewObj);
      }
      if (bcode) {
        /* read patches */
        int primRC = lstImgReadWord(fp);
        if (primRC) {
          /*fprintf(stderr, "reading %d patches...\n", primRC);*/
          for (; primRC > 0; --primRC) {
            char nbuf[257];
            uint8_t len;
            if (fread(&len, 1, 1, fp) != 1) lstFatal("error reading bytecodes", (intptr_t)bnewObj);
            if (fread(nbuf, len, 1, fp) != 1) lstFatal("error reading bytecodes", (intptr_t)bnewObj);
            nbuf[len] = '\0';
            int idx = lstFindPrimitiveIdx(nbuf);
            if (idx < 0) {
              fprintf(stderr, "\nERROR: unknown primitive name: [%s]\n", nbuf);
              lstFatal("unknown primitive name in image", (intptr_t)bnewObj);
            }
            /*fprintf(stderr, "#%d: [%s]\n", pd->idx, nbuf);*/
            int pcc = lstImgReadWord(fp);
            if (pcc < 1) lstFatal("invalid primitive patch record", (intptr_t)bnewObj);
            for (; pcc > 0; --pcc) {
              int pc = lstImgReadWord(fp);
              if (pc < 0 || pc >= size) lstFatal("invalid primitive patch record", (intptr_t)bnewObj);
              /*
              if (bnewObj->bytes[pc] != idx) {
                fprintf(stderr, "  %04X: %d patched to %d [%s]\n", pc, bnewObj->bytes[pc], idx, nbuf);
              }
              */
              bnewObj->bytes[pc] = idx;
            }
          }
        }
      }
      bnewObj->stclass = objectRead(fp);
      newObj = (lstObject *)bnewObj;
      break;
    case LST_IMGOT_REF: /* previous object */
      f = lstImgReadWord(fp);
      /*fprintf(stderr, "REF: %d (of %d)\n", f, indirtop);*/
      if (f >= indirtop) lstFatal("invalid LST image (bad object reference)", f);
      newObj = indirArray[f];
      break;
    case LST_IMGOT_NIL: /* object 0 (nil object) */
      newObj = indirArray[0];
      break;
    default:
      lstFatal("invalid object type in image", type);
  }
  return newObj;
}


int lstReadImage (FILE *fp) {
  char sbuf[128];
  uint8_t ver;
  int f;
  if (fread(sbuf, strlen(imgSign)+1, 1, fp) != 1) lstFatal("invalid LST image signature: ", 0);
  if (memcmp(sbuf, imgSign, strlen(imgSign)+1)) lstFatal("invalid LST image signature: ", 1);
  if (fread(&ver, sizeof(ver), 1, fp) != 1) lstFatal("can't read LST image", 0);
  if (ver != LST_IMAGE_VERSION) lstFatal("invalid LST image version: ", ver);
  /* clear spaces */
  memSpaces[0].cur = memSpaces[0].start;
  memSpaces[1].cur = memSpaces[1].start;
  memSpaces[2].cur = memSpaces[2].start;
  lstRootTop = 0;
  staticRootTop = 0;
  curSpaceNo = 1;
  /* use the currently unused space for the indir pointers */
  indirArray = (void *)memSpaces[0].start;
  indirtop = 0;
  /* read in the method from the image file */
  lstNilObj = objectRead(fp);
  /*fprintf(stderr, "NIL: %p\n", lstNilObj);*/
  lstTrueObj = objectRead(fp);
  /*fprintf(stderr, "true: %p\n", lstTrueObj);*/
  lstFalseObj = objectRead(fp);
  lstGlobalObj = objectRead(fp);
  lstBadMethodSym = objectRead(fp);
  f = lstImgReadWord(fp);
  if (f != LST_MAX_BIN_MSG) lstFatal("invalid number of binary messages in image", f);
  for (f = 0; f < LST_MAX_BIN_MSG; f++) lstBinMsgs[f] = objectRead(fp);
  {
    const EPInfo *e;
    for (e = clInfo; e->name; ++e) {
      char buf[128];
      lstObject *o = lstFindGlobal(e->name);
      sprintf(buf, "class not found: %s", e->name);
      if (!o) lstFatal(buf, 0);
      *(e->eptr) = o;
    }
  }
  {
    const EPInfo *e;
    lstObject *u = lstFindGlobal("UndefinedObject");
    if (!u) lstFatal("no UndefinedObject in in image", 0);
    for (f = 0, e = epInfo; e->name; ++f, ++e) {
      lstObject *o = lstFindMethod(u, e->name);
      if (!o) lstFatal("missing entry point in image", f);
      *(e->eptr) = o;
    }
  }
#ifdef INLINE_SOME_METHODS
  lstMetaCharClass = lstCharClass->stclass;
  {
    int f;
    for (f = 0; lstInlineMethodList[f].name; ++f) {
      lstObject *m = lstFindMethod((*lstInlineMethodList[f].mtclass), lstInlineMethodList[f].name);
      if (!m) {
        fprintf(stderr, "missing method %d: %s\n", f, lstInlineMethodList[f].name);
        lstFatal("missing method", 0);
      }
      (*lstInlineMethodList[f].method) = m;
    }
  }
#endif
#ifdef EXPLOADER
  fprintf(stderr, "%d static bytes used\n", (intptr_t)(memSpaces[STATIC_SPACE].cur-memSpaces[STATIC_SPACE].start));
#endif
  return indirtop;
}


static int lstImgWriteNoSrc = 0;

static void objectWrite (FILE *fp, lstObject *obj) {
  int f, size;
  if (obj == 0) {
    /* *((char *)0) = '\0'; */
    lstFatal("writing out a null object", (intptr_t)obj);
  }
  if (LST_IS_SMALLINT(obj)) {
    /* SmallInt */
    int val;
    lstImgWriteWord(fp, LST_IMGOT_SMALLINT);
    val = lstIntValue(obj);
    /*fwrite(&val, sizeof(val), 1, fp);*/
    lstImgWriteDWord(fp, val);
    return;
  }
  /* see if already written */
  for (f = 0; f < indirtop; f++) {
    if (obj == indirArray[f]) {
      if (f == 0) {
        lstImgWriteWord(fp, LST_IMGOT_NIL);
      } else {
        lstImgWriteWord(fp, LST_IMGOT_REF);
        lstImgWriteWord(fp, f);
      }
      return;
    }
  }
  int isThisWeak = 0;
  if (obj->fin) {
    /* it's object with finalizer; write 'nil' instead */
    /*fprintf(stderr, "FIN->NIL\n");*/
    if (LST_IS_BYTES_EX(obj)) {
      objectWrite(fp, lstNilObj);
      return;
    }
    /* this is either weak or st-fin object */
    isThisWeak = LST_IS_WEAK(obj);
  }
  /* not written, do it now */
  indirArray[indirtop++] = obj;
  /* byte objects */
  /*
  if ((void *)obj < (void *)0x10000) {
    fprintf(stderr, "obj: %p\n", obj);
    abort();
  }
  */
  if (LST_IS_BYTES(obj)) {
    /*
    if (obj->stclass == lstStringClass) fprintf(stderr, "%p: BYTES: string\n", obj);
    else if (obj->stclass == lstByteArrayClass) fprintf(stderr, "%p: BYTES: byte array\n", obj);
    else if (obj->stclass == lstByteCodeClass) fprintf(stderr, "%p: BYTES: bytecode\n", obj);
    else if (obj->stclass == lstSymbolClass) fprintf(stderr, "%p: BYTES: symbol\n", obj);
    else fprintf(stderr, "%p: BYTES: unknown\n", obj);
    */
    lstByteObject *bobj = (lstByteObject *)obj;
    int type = lstIsKindOf(obj, lstByteCodeClass) ? LST_IMGOT_BCODE : LST_IMGOT_BARRAY;
    size = LST_SIZE(obj);
    lstImgWriteWord(fp, type);
    lstImgWriteWord(fp, size);
    if (size > 0) {
      if (fwrite(lstBytePtr(bobj), size, 1, fp) != 1) lstFatal("error writing byte array", (intptr_t)obj);
    }
    if (type == LST_IMGOT_BCODE) lstWritePrimPatches(fp, bobj);
    objectWrite(fp, obj->stclass);
    return;
  }
  /* ordinary objects */
  size = LST_SIZE(obj);
  lstImgWriteWord(fp, isThisWeak ? LST_IMGOT_WEAK : LST_IMGOT_OBJECT);
  lstImgWriteWord(fp, size);
  /*if (!obj->stclass) lstFatal("SHITFUCKDIE!", (intptr_t)obj);*/
  objectWrite(fp, obj->stclass);
  for (f = 0; f < size; f++) {
    if (lstImgWriteNoSrc && f == lstIVtextInMethod && lstIsKindOf(obj, lstMethodClass)) {
      /*fprintf(stderr, "omiting sources!\n");*/
      /* nil? true? false? */
      objectWrite(fp, lstFalseObj);
    } else {
      if (obj->data[f] == 0) {
        fprintf(stderr, "ERROR: stclass=%p; f=%d\n", obj->stclass, f);
        exit(1);
      }
      {
        lstObject *o = obj->data[f];
        if (!LST_IS_SMALLINT(o) && (void *)o < (void *)0x10000) {
          fprintf(stderr, "ERROR: %p: uninitialized shit detected at %d of %d (%p)!\n", obj, f, size, o);
          fprintf(stderr, "  class: [%s]\n", lstBytePtr(obj->stclass->data[lstIVnameInClass]));
        }
      }
      objectWrite(fp, obj->data[f]);
    }
  }
}


int lstWriteImage (FILE *fp, int noSrc) {
  uint8_t ver = LST_IMAGE_VERSION;
  int f;
  fwrite(imgSign, strlen(imgSign)+1, 1, fp);
  if (fwrite(&ver, sizeof(ver), 1, fp) != 1) lstFatal("can't write LST image", 0);
  /* use the currently unused space for the indir pointers */
  indirArray = (void *)memSpaces[curSpaceNo^1].start;
  indirtop = 0;
  lstImgWriteNoSrc = noSrc;
  /* write out the roots of the image file */
  objectWrite(fp, lstNilObj);
  objectWrite(fp, lstTrueObj);
  objectWrite(fp, lstFalseObj);
  objectWrite(fp, lstGlobalObj);
  objectWrite(fp, lstBadMethodSym);
  lstImgWriteWord(fp, LST_MAX_BIN_MSG);
  for (f = 0; f < LST_MAX_BIN_MSG; f++) objectWrite(fp, lstBinMsgs[f]);
  return indirtop;
}
