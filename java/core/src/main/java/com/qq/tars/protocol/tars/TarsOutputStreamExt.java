/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.
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

package com.qq.tars.protocol.tars;

import java.util.List;

import com.qq.tars.common.util.BeanAccessor;
import com.qq.tars.common.util.CommonUtils;
import com.qq.tars.protocol.tars.exc.TarsEncodeException;
import com.qq.tars.protocol.tars.support.TarsStructInfo;
import com.qq.tars.protocol.tars.support.TarsStrutPropertyInfo;
import com.qq.tars.protocol.util.TarsHelper;

public class TarsOutputStreamExt {

    public static void write(Object e, int tag, TarsOutputStream jos) {
        TarsStructInfo info = TarsHelper.getStructInfo(e.getClass());
        if (info == null) {
            throw new TarsEncodeException("the JavaBean[" + e.getClass().getSimpleName() + "] no annotation Struct");
        }

        jos.reserve(2);
        jos.writeHead(TarsStructBase.STRUCT_BEGIN, tag);
        List<TarsStrutPropertyInfo> propertysList = info.getPropertyList();
        if (!CommonUtils.isEmptyCollection(propertysList)) {
            for (TarsStrutPropertyInfo propertyInfo : propertysList) {
                Object value = null;
                try {
                    value = BeanAccessor.getBeanValue(e, propertyInfo.getName());
                } catch (Exception ex) {
                    throw new TarsEncodeException(ex.getLocalizedMessage());
                }

                if (value == null && propertyInfo.isRequire()) {
                    throw new TarsEncodeException(propertyInfo.getName() + " is require tag=" + propertyInfo.getOrder());
                }

                if (value != null) {
                    jos.write(value, propertyInfo.getOrder());
                }
            }
        }

        jos.reserve(2);
        jos.writeHead(TarsStructBase.STRUCT_END, 0);
    }
}
