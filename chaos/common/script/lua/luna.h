    //
    //  luna.hpp
    //  Luatest
    //
    //  Created by Claudio Bisegni on 10/05/16.
    //  Copyright © 2016 Claudio Bisegni. All rights reserved.
    //

#ifndef luna_h
#define luna_h

#include <chaos/common/script/lua/core/lua.hpp>

namespace chaos {
    namespace common {
        namespace script {
            namespace lua {
                template<class T> class Luna;

                template<class T>
                class LunaAllocationHandler {
                public:
                    virtual void allocationOf(T *newAllocatedClass) = 0;
                    virtual void deallocationOf(T *deallocatedClass) = 0;
                };

                template<class T> class Luna {
                    static LunaAllocationHandler<T> *handler;
                public:
                    static void setHandler(LunaAllocationHandler<T> *_handler) {
                        handler = _handler;
                    }

                    static void Register(lua_State *L) {
                        lua_pushcclosure(L, &Luna<T>::constructor, 0);
                        lua_setglobal(L, T::className);

                        luaL_newmetatable(L, T::className);
                        lua_pushstring(L, "__gc");
                        lua_pushcclosure(L, &Luna<T>::gc_obj, 0);
                        lua_settable(L, -3);
                    }

                    static int constructor(lua_State *L) {
                        T* obj = new T(L);

                        lua_newtable(L);
                        lua_pushnumber(L, 0);
                        T** a = (T**)lua_newuserdata(L, sizeof(T*));
                        *a = obj;
                        luaL_getmetatable(L, T::className);
                        lua_setmetatable(L, -2);
                        lua_settable(L, -3); // table[0] = obj;

                        for (int i = 0; T::Register[i].name; i++) {
                            lua_pushstring(L, T::Register[i].name);
                            lua_pushnumber(L, i);
                            lua_pushcclosure(L, &Luna<T>::thunk, 1);
                            lua_settable(L, -3);
                        }
                        return 1;
                    }

                    static int thunk(lua_State *L) {
                        int i = (int)lua_tonumber(L, lua_upvalueindex(1));
                        lua_pushnumber(L, 0);
                        lua_gettable(L, 1);
                        
                        T** obj = static_cast<T**>(luaL_checkudata(L, -1, T::className));
                        if(handler)handler->allocationOf(*obj);
                        lua_remove(L, -1);
                        return ((*obj)->*(T::Register[i].mfunc))(L);
                    }
                    
                    static int gc_obj(lua_State *L) {
                        T** obj = static_cast<T**>(luaL_checkudata(L, -1, T::className));
                        if(handler)handler->deallocationOf(*obj);
                        delete (*obj);
                        return 0;
                    }
                    
                    struct RegType {
                        const char *name;
                        int(T::*mfunc)(lua_State*);
                    };
                };

                    //define static variable
                template<class T>
                LunaAllocationHandler<T> *Luna<T>::handler = NULL;
            }
        }
    }
}
#endif /* luna_h */
