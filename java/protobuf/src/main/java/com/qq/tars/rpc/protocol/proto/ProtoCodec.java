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

package com.qq.tars.rpc.protocol.proto;

import com.google.protobuf.GeneratedMessage;
import com.qq.tars.common.util.Constants;
import com.qq.tars.net.protocol.ProtocolException;
import com.qq.tars.protocol.tars.support.TarsMethodInfo;
import com.qq.tars.protocol.tars.support.TarsMethodParameterInfo;
import com.qq.tars.protocol.util.TarsHelper;
import com.qq.tars.rpc.protocol.tars.TarsCodec;
import com.qq.tars.rpc.protocol.tars.TarsServantRequest;
import com.qq.tars.rpc.protocol.tars.TarsServantResponse;

import java.util.List;

public class ProtoCodec extends TarsCodec {
    public ProtoCodec(String charsetName) {
        super(charsetName);
    }

    @Override
    protected byte[] encodeResult(TarsServantResponse response, String charsetName) {
        TarsServantRequest request = response.getRequest();
        byte[] result = new byte[] {};
        if (TarsHelper.isPing(request.getFunctionName())) {
            return result;
        }

        result = ((GeneratedMessage) response.getResult()).toByteArray();
        return result;
    }

    @Override
    protected byte[] encodeRequestParams(TarsServantRequest request, String charsetName) throws ProtocolException {
        Object[] parameter = request.getMethodParameters();
        if (TarsHelper.isCallback(request.getMethodInfo().getParametersList().get(0).getAnnotations())) {
            return ((GeneratedMessage) parameter[1]).toByteArray();
        }
        return ((GeneratedMessage) parameter[0]).toByteArray();
    }

    @Override
    protected Object[] decodeRequestBody(byte[] data, String charset,  TarsMethodInfo methodInfo) throws Exception {
        List<TarsMethodParameterInfo> parametersList = methodInfo.getParametersList();
        return new Object[]{MethodCache.getInstance().parseFrom((Class) parametersList.get(0).getStamp(), data)};
    }

    @Override
    protected Object[] decodeResponseBody(byte[] data, String charset, TarsMethodInfo methodInfo) throws Exception {
        TarsMethodParameterInfo returnInfo = methodInfo.getReturnInfo();
        return new Object[]{MethodCache.getInstance().parseFrom((Class) returnInfo.getStamp(), data)};
    }

    @Override
    protected Object[] decodeCallbackArgs(byte[] data, String charset, TarsMethodInfo methodInfo) throws Exception {
        TarsMethodParameterInfo returnInfo = methodInfo.getReturnInfo();
        return new Object[]{MethodCache.getInstance().parseFrom((Class) returnInfo.getStamp(), data)};
    }

    public String getProtocol() {
        return Constants.PROTO_PROTOCOL;
    }
}
