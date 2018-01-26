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

package com.qq.tars.rpc.protocol.tars.support;

import java.lang.reflect.Method;

import com.qq.tars.protocol.tars.annotation.TarsCallback;
import com.qq.tars.rpc.exc.ClientException;
import com.qq.tars.rpc.protocol.Codec;
import com.qq.tars.rpc.protocol.tars.TarsCodec;
import com.qq.tars.rpc.protocol.tars.TarsCodecHelper;
import com.qq.tars.rpc.protocol.tars.TarsServantRequest;
import com.qq.tars.rpc.protocol.tars.TarsServantResponse;

@TarsCallback(comment = "Callback")
public abstract class TarsAbstractCallback implements com.qq.tars.net.client.Callback<TarsServantResponse> {

    @Override
    public final void onCompleted(TarsServantResponse response) {
        TarsServantRequest request = response.getRequest();
        try {
            Method callback = getCallbackMethod("callback_".concat(request.getFunctionName()));
            callback.setAccessible(true);
            callback.invoke(this, ((TarsCodec) response.getSession().getProtocolFactory().getDecoder()).decodeCallbackArgs(response));
        } catch (Throwable ex) {
            throw new ClientException(ex);
        }
    }

    private Method getCallbackMethod(String methodName) throws NoSuchMethodException {
        Method[] methods = getClass().getDeclaredMethods();
        for (Method method : methods) {
            if (methodName.equals(method.getName())) {
                return method;
            }
        }
        throw new NoSuchMethodException("no such method " + methodName);
    }

    @Override
    public final void onException(Throwable ex) {
        callback_exception(ex);
    }

    @Override
    public final void onExpired() {
        callback_expired();
    }

    public abstract void callback_exception(Throwable ex);

    public abstract void callback_expired();
}
