/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#ifndef ltm_h
#define ltm_h


#include "lobject.h"


/*
* WARNING: if you change the order of this enumeration,
* grep "ORDER TM" and "ORDER OP"
*/
typedef enum {
  TM_INDEX,
  TM_NEWINDEX,
  TM_GC,
  TM_MODE,
  TM_LEN,
  TM_EQ,  /* last tag method with fast access */
  TM_ADD,
  TM_SUB,
  TM_MUL,
  TM_MOD,
  TM_POW,
  TM_DIV,
  TM_IDIV,
  TM_BAND,
  TM_BOR,
  TM_BXOR,
  TM_SHL,
  TM_SHR,
  TM_UNM,
  TM_BNOT,
  TM_LT,
  TM_LE,
  TM_CONCAT,
  TM_CALL,
  TM_N		/* number of elements in the enum */
} TMS;



#define gfasttm(g,et,e) ((et) == NULL ? NULL : \
  ((et)->flags & (1u<<(e))) ? NULL : luaT_gettm(et, e, (g)->tmname[e]))

#define fasttm(l,et,e)	gfasttm(G(l), et, e)

#define ttypename(x)	luaT_typenames_[(x) + 1]
#define objtypename(x)	ttypename(ttnov(x))

LUAI_DDEC const char *const luaT_typenames_[LUA_TOTALTAGS];


LUAI_FUNC const TValue *luaT_gettm (Table *events, TMS event, TString *ename);
LUAI_FUNC const TValue *luaT_gettmbyobj (lua_State *L, const TValue *o,
                                                       TMS event);
LUAI_FUNC void luaT_init (lua_State *L);

LUAI_FUNC void luaT_callTM (lua_State *L, const TValue *f, const TValue *p1,
                            const TValue *p2, TValue *p3, int hasres);
LUAI_FUNC int luaT_callbinTM (lua_State *L, const TValue *p1, const TValue *p2,
                              StkId res, TMS event);
LUAI_FUNC void luaT_trybinTM (lua_State *L, const TValue *p1, const TValue *p2,
                              StkId res, TMS event);
LUAI_FUNC int luaT_callorderTM (lua_State *L, const TValue *p1,
                                const TValue *p2, TMS event);



#endif
