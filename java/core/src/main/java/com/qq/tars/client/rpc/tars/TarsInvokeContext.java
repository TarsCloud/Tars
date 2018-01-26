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

package com.qq.tars.client.rpc.tars;

import java.lang.annotation.Annotation;
import java.lang.reflect.Method;
import java.util.Map;

import com.qq.tars.client.rpc.ServantInvokeContext;
import com.qq.tars.protocol.util.TarsHelper;

@SuppressWarnings("serial")
public class TarsInvokeContext extends ServantInvokeContext {

    @SuppressWarnings("unchecked")
    public TarsInvokeContext(Method method, Object[] arguments, Map<String, String> attachments) {
        super(method, arguments, attachments);

        Annotation[][] as = method.getParameterAnnotations();
        for (int i = 0, length = as.length; i < length; i++) {
            if (TarsHelper.isContext(as[i])) {
                getAttachments().putAll((Map<String, String>) arguments[i]);
            }
        }
    }
}
