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

package com.qq.tars.rpc.ext;

import com.qq.tars.net.core.IoBuffer;
import com.qq.tars.net.core.Request;
import com.qq.tars.net.core.Response;
import com.qq.tars.net.core.Session;
import com.qq.tars.net.protocol.ProtocolDecoder;
import com.qq.tars.net.protocol.ProtocolEncoder;
import com.qq.tars.net.protocol.ProtocolException;
import com.qq.tars.rpc.protocol.Codec;

public class ExtendedProtocolCodecWrapper implements ProtocolEncoder, ProtocolDecoder {

    protected Codec codec = null;

    public ExtendedProtocolCodecWrapper(Codec codec) {
        this.codec = codec;
    }

    @Override
    public IoBuffer encodeResponse(Response response, Session session) throws ProtocolException {
        return codec.encodeResponse(response, session);
    }

    @Override
    public Request decodeRequest(IoBuffer buffer, Session session) throws ProtocolException {
        return codec.decodeRequest(buffer, session);
    }

    @Override
    public Response decodeResponse(IoBuffer buff, Session session) throws ProtocolException {
        return codec.decodeResponse(buff, session);
    }

    @Override
    public IoBuffer encodeRequest(Request request, Session session) throws ProtocolException {
        return codec.encodeRequest(request, session);
    }
}
