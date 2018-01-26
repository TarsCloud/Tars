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

package com.qq.tars.rpc.protocol.tars;

import java.util.Map;

import com.qq.tars.net.core.Session;
import com.qq.tars.protocol.tars.TarsInputStream;
import com.qq.tars.protocol.tars.support.TarsMethodInfo;
import com.qq.tars.protocol.util.TarsHelper;
import com.qq.tars.rpc.protocol.Codec;
import com.qq.tars.rpc.protocol.ServantRequest;

public class TarsServantRequest extends ServantRequest implements java.io.Serializable {

    private static final long serialVersionUID = 1L;

    private short version; 
    private byte packetType; 
    private int messageType; 
    private String servantName;
    private String functionName;
    private int timeout; // iTimeout
    private Map<String, String> status;
    private Map<String, String> context;

    private TarsMethodInfo methodInfo;
    private Object[] methodParameters;

    private TarsInputStream inputStream;

    private String charsetName;

    private int ret;

    public TarsServantRequest(Session session) {
        super(session);
    }

    public TarsServantRequest(Session session, int ret) {
        super(session);
        this.ret = ret;
    }

    public void init() {
        ((TarsCodec) this.session.getProtocolFactory().getDecoder()).decodeRequestBody(this);
//        TarsCodecHelper.decodeRequestBody(this);
    }

    public TarsInputStream getInputStream() {
        return inputStream;
    }

    public void setInputStream(TarsInputStream inputStream) {
        this.inputStream = inputStream;
    }

    public short getVersion() {
        return version;
    }

    public void setVersion(short version) {
        this.version = version;
    }

    public byte getPacketType() {
        return packetType;
    }

    public void setPacketType(byte packetType) {
        this.packetType = packetType;
    }

    public int getMessageType() {
        return messageType;
    }

    public void setMessageType(int messageType) {
        this.messageType = messageType;
    }

    public String getServantName() {
        return servantName;
    }

    public void setServantName(String servantName) {
        this.servantName = servantName;
    }

    public String getFunctionName() {
        return functionName;
    }

    public void setFunctionName(String functionName) {
        this.functionName = functionName;
    }

    public int getRequestId() {
        return getTicketNumber();
    }

    public void setRequestId(int requestId) {
        this.setTicketNumber(requestId);
    }

    public int getTimeout() {
        return timeout;
    }

    public void setTimeout(int timeout) {
        this.timeout = timeout;
    }

    public Map<String, String> getStatus() {
        return status;
    }

    public void setStatus(Map<String, String> status) {
        this.status = status;
    }

    public Map<String, String> getContext() {
        return context;
    }

    public void setContext(Map<String, String> context) {
        this.context = context;
    }

    public TarsMethodInfo getMethodInfo() {
        return methodInfo;
    }

    public void setMethodInfo(TarsMethodInfo methodInfo) {
        this.methodInfo = methodInfo;
    }

    public Object[] getMethodParameters() {
        return methodParameters;
    }

    public void setMethodParameters(Object[] methodParameters) {
        this.methodParameters = methodParameters;
    }

    public boolean isAsync() {
        return TarsHelper.isAsync(methodInfo.getMethodName());
    }

    public String getCharsetName() {
        return charsetName;
    }

    public void setCharsetName(String charsetName) {
        this.charsetName = charsetName;
    }

    public int getRet() {
        return ret;
    }

    public void setRet(int ret) {
        this.ret = ret;
    }
}
