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

#include "util/tc_dyn_object.h"
#include <string.h>

namespace tars
{
/**********************************TC_DYN_RuntimeClass定义***********************************/

TC_DYN_RuntimeClass* TC_DYN_RuntimeClass::pFirstClass = NULL;

TC_DYN_Object* TC_DYN_RuntimeClass::createObject()
{
    if (m_pfnCreateObject == NULL)
    {
        return NULL;
    }

    TC_DYN_Object* pObject = (*m_pfnCreateObject)();
    {
        return pObject;
    }
}

TC_DYN_RuntimeClass* TC_DYN_RuntimeClass::load(const char *szClassName)
{
    TC_DYN_RuntimeClass* pClass;

    for (pClass = pFirstClass; pClass != NULL; pClass = pClass->m_pNextClass)
    {
        if (strcmp(szClassName, pClass->m_lpszClassName) == 0)
        return pClass;
    }

    return NULL;
}

/**********************************szTC_DYN_Object定义***********************************/

TC_DYN_RuntimeClass TC_DYN_Object::classTC_DYN_Object =
{
    (char*)"TC_DYN_Object", 
    sizeof(TC_DYN_Object), 
    NULL, 
    NULL, 
    NULL 
};

static TC_DYN_Init _init_TC_DYN_Object(&TC_DYN_Object::classTC_DYN_Object);

TC_DYN_RuntimeClass* TC_DYN_Object::GetRuntimeClass() const
{
    return &TC_DYN_Object::classTC_DYN_Object;
}

bool TC_DYN_Object::isKindOf(const TC_DYN_RuntimeClass* pClass) const
{
    TC_DYN_RuntimeClass* pClassThis = GetRuntimeClass();
    while (pClassThis != NULL)
    {
        if (pClassThis == pClass)
        {
            return true;
        }

        pClassThis = pClassThis->m_pBaseClass;
    }
    return false;
}

}
