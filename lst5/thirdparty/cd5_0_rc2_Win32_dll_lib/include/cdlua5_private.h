/** \file
* \brief Private Lua 5 Binding Functions
* 
* See Copyright Notice in cd.h
*/

#ifndef __CDLUA5_PRIVATE_H
#define __CDLUA5_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif


/* context management */

typedef struct _cdluaCallback {
  int lock;
  const char* name;
  cdCallback func;
} cdluaCallback;

typedef struct _cdluaContext {
  int id;
  const char* name;
  cdContext* (*ctx)(void);
  void* (*checkdata)(lua_State* L,int param);
  cdluaCallback* cb_list;
  int cb_n;
} cdluaContext;

typedef struct _cdluaLuaState {
  cdCanvas* void_canvas;            /* the VOID canvas to avoid a NULL active canvas */
  cdluaContext* drivers[50];  /* store the registered drivers, map integer values to cdContext */
  int numdrivers;
} cdluaLuaState;

/* tag management */

typedef struct _cdluaCanvas {
  cdCanvas* canvas;
} cdluaCanvas;

typedef struct _cdluaState {
  cdState* state;
} cdluaState;

typedef struct _cdluaStipple {
  unsigned char* stipple;
  int width;
  int height;
  long int size;
} cdluaStipple;

typedef struct _cdluaPattern {
  long int* pattern;
  int width;
  int height;
  long int size;
} cdluaPattern;

/* this is the same declaration used in the IM toolkit for imPalette in Lua */
typedef struct _cdluaPalette {
  long int* palette;
  int size;
} cdluaPalette;

typedef struct _cdluaImage {
  cdImage* image;
} cdluaImage;

typedef struct _cdluaImageRGB {
  unsigned char* red;
  unsigned char* green;
  unsigned char* blue;
  int width;
  int height;
  long int size;
} cdluaImageRGB;

typedef struct _cdluaImageRGBA {
  unsigned char* red;
  unsigned char* green;
  unsigned char* blue;
  unsigned char* alpha;
  int width;
  int height;
  long int size;
} cdluaImageRGBA;
         
typedef struct _cdluaImageMap {
  unsigned char* index;
  int width;
  int height;
  long int size;
} cdluaImageMap;

typedef struct _cdluaChannel {
  unsigned char* channel;
  long int size;
} cdluaChannel;

typedef struct _cdluaBitmap {
  cdBitmap* bitmap;
} cdluaBitmap;


cdluaLuaState* cdlua_getstate(lua_State* L);
cdluaContext* cdlua_getdriver(lua_State* L, int driver);

lua_State* cdlua_getplaystate(void);
void cdlua_setplaystate(lua_State* L);

void cdlua_update_active(lua_State* L, cdCanvas* canvas);
int cdlua_open_active(lua_State* L, cdluaLuaState* cdL);

int cdlua_open_canvas(lua_State* L);

void cdlua_addcontext(lua_State* L, cdluaLuaState* cdL, cdluaContext* luactx);
void cdlua_initdrivers(lua_State* L, cdluaLuaState* cdL);

cdluaPalette* cdlua_checkpalette(lua_State* L, int param);
cdluaStipple* cdlua_checkstipple(lua_State* L, int param);
cdluaPattern* cdlua_checkpattern(lua_State* L, int param);
cdluaImageRGB* cdlua_checkimagergb(lua_State* L, int param);
cdluaImageRGBA* cdlua_checkimagergba(lua_State* L, int param);
cdluaImageMap* cdlua_checkimagemap(lua_State* L, int param);
cdluaChannel* cdlua_checkchannel(lua_State* L, int param);

cdImage* cdlua_checkimage(lua_State* L, int param);
cdState* cdlua_checkstate(lua_State* L, int param);
cdBitmap* cdlua_checkbitmap(lua_State* L, int param);

void cdlua_pushpalette(lua_State* L, long* palette, int size);
void cdlua_pushstipple(lua_State* L, unsigned char* stipple, int width, int height);
void cdlua_pushpattern(lua_State* L, long int* pattern, int width, int height);
void cdlua_pushimagergb(lua_State* L, unsigned char* red, unsigned char* green, unsigned char* blue, int width, int height);
void cdlua_pushimagergba(lua_State* L, unsigned char* red, unsigned char* green, unsigned char* blue, unsigned char* alpha, int width, int height);
void cdlua_pushimagemap(lua_State* L, unsigned char* index, int width, int height);
void cdlua_pushchannel(lua_State* L, unsigned char* channel, int size);

void cdlua_pushimage(lua_State* L, cdImage* image);
void cdlua_pushstate(lua_State* L, cdState* state);
void cdlua_pushbitmap(lua_State* L, cdBitmap* bitmap);

#ifdef __cplusplus
}
#endif

#endif
