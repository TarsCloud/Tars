/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016THL A29 Limited, a Tencent company. All rights reserved.
 *
 * Licensed under the BSD 3-Clause License (the "License"); you may not use this file except 
 * in compliance with the License. You may obtain a copy of the License at
 *
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software distributed 
 * under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR 
 * CONDITIONS OF ANY KIND, either express or implied. See the License for the 
 * specific language governing permissions and limitations under the License.
 */

#ifndef __TC_DYN_OBJECT_H
#define __TC_DYN_OBJECT_H

#include <iostream>

namespace tars
{
/////////////////////////////////////////////////
/** 
 * @file tc_dyn_object.h
 * @brief 动态生成类. 
*/             
/////////////////////////////////////////////////


/**
 * @brief 动态生成类
 */
class TC_DYN_Object;
class TC_DYN_RuntimeClass;

struct TC_DYN_RuntimeClass
{
    const char* m_lpszClassName;
    int m_nObjectSize;
    TC_DYN_Object* (* m_pfnCreateObject)();
    TC_DYN_RuntimeClass* m_pBaseClass;
    TC_DYN_RuntimeClass* m_pNextClass;

    static TC_DYN_RuntimeClass* pFirstClass;

    TC_DYN_Object* createObject();

    static TC_DYN_RuntimeClass* load(const char *szClassName);    
};

struct TC_DYN_Init
{
    TC_DYN_Init(TC_DYN_RuntimeClass* pNewClass)
    {
        pNewClass->m_pNextClass = TC_DYN_RuntimeClass::pFirstClass;
        TC_DYN_RuntimeClass::pFirstClass = pNewClass;
    }
};

class TC_DYN_Object
{
public:
    TC_DYN_Object(){};
    virtual ~TC_DYN_Object(){};

    virtual TC_DYN_RuntimeClass* GetRuntimeClass() const;

    bool isKindOf(const TC_DYN_RuntimeClass* pClass) const;
private:

    TC_DYN_Object(const TC_DYN_Object& objectSrc);
    void operator=(const TC_DYN_Object& objectSrc);

public:
    static TC_DYN_RuntimeClass classTC_DYN_Object;
};

#define RUNTIME_CLASS(class_name) ((TC_DYN_RuntimeClass*)(&class_name::class##class_name))

#define DECLARE_DYNCREATE(class_name) \
public: \
    static TC_DYN_RuntimeClass class##class_name; \
    virtual TC_DYN_RuntimeClass* GetRuntimeClass() const; \
    static TC_DYN_Object* createObject();

#define IMPLEMENT_DYNCREATE(class_name, base_class_name) \
    TC_DYN_Object* class_name::createObject() \
        { return new class_name; } \
    TC_DYN_RuntimeClass class_name::class##class_name = { \
        #class_name, \
        sizeof(class_name), \
        &class_name::createObject, \
        RUNTIME_CLASS(base_class_name), \
        NULL }; \
    static TC_DYN_Init _init_##class_name(&class_name::class##class_name);   \
    TC_DYN_RuntimeClass* class_name::GetRuntimeClass() const \
        { return RUNTIME_CLASS(class_name); } 

#define TC_DYN_CreateObject(class_name) \
(TC_DYN_RuntimeClass::load(class_name) == NULL ? NULL : TC_DYN_RuntimeClass::load(class_name)->createObject())
    
}

#endif

