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

import java.lang.reflect.Method;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;

import com.qq.tars.common.support.ClassLoaderManager;
import com.qq.tars.common.support.Holder;
import com.qq.tars.common.util.CommonUtils;
import com.qq.tars.common.util.StringUtils;
import com.qq.tars.net.core.IoBuffer;
import com.qq.tars.net.core.Response;
import com.qq.tars.net.core.Session;
import com.qq.tars.net.protocol.ProtocolException;
import com.qq.tars.protocol.tars.TarsInputStream;
import com.qq.tars.protocol.tars.TarsOutputStream;
import com.qq.tars.protocol.tars.support.TarsMethodInfo;
import com.qq.tars.protocol.tars.support.TarsMethodParameterInfo;
import com.qq.tars.protocol.util.TarsHelper;
import com.qq.tars.protocol.util.TarsUtil;
import com.qq.tars.rpc.protocol.tars.support.AnalystManager;
import com.qq.tars.rpc.protocol.tup.UniAttribute;

@SuppressWarnings("unchecked")
public class TarsCodecHelper {

/*
    public static IoBuffer encodeRequest(TarsServantRequest request, Session session, String charsetName) throws ProtocolException {
        request.setCharsetName(charsetName);
        TarsOutputStream os = new TarsOutputStream();
        os.setServerEncoding(charsetName);

        os.getByteBuffer().putInt(0);
        os.write(request.getVersion(), 1);
        os.write(request.getPacketType(), 2);
        os.write(request.getMessageType(), 3);
        os.write(request.getTicketNumber(), 4);
        os.write(request.getServantName(), 5);
        os.write(request.getFunctionName(), 6);
        os.write(encodeRequestParams(request, charsetName), 7);
        os.write(request.getTimeout(), 8);
        os.write(request.getContext(), 9);
        os.write(request.getStatus(), 10);

        os.getByteBuffer().flip();

        int length = os.getByteBuffer().remaining();

        os.getByteBuffer().duplicate().putInt(0, length);
        if (length > TarsHelper.PACKAGE_MAX_LENGTH || length <= 0) {
            throw new ProtocolException("the length header of the package must be between 0~10M bytes. data length:" + Integer.toHexString(length));
        }
        return IoBuffer.wrap(os.getByteBuffer());
    }

    private static byte[] encodeRequestParams(TarsServantRequest request, String charsetName) throws ProtocolException {
        TarsOutputStream os = new TarsOutputStream(0);
        os.setServerEncoding(charsetName);

        TarsMethodInfo methodInfo = request.getMethodInfo();
        List<TarsMethodParameterInfo> parameterInfoList = methodInfo.getParametersList();

        Object value = null;
        Object[] parameter = request.getMethodParameters();
        for (TarsMethodParameterInfo parameterInfo : parameterInfoList) {
            if (TarsHelper.isContext(parameterInfo.getAnnotations()) || TarsHelper.isCallback(parameterInfo.getAnnotations())) {
                continue;
            }

            value = parameter[request.isAsync() ? parameterInfo.getOrder() : parameterInfo.getOrder() - 1];
            if (TarsHelper.isHolder(parameterInfo.getAnnotations()) && value != null) {
                try {
                    value = TarsHelper.getHolderValue(value);
                } catch (Exception e) {
                    throw new ProtocolException(e);
                }
                if (value != null) {
                    os.write(value, parameterInfo.getOrder());
                }
            } else if (value != null) {
                os.write(value, parameterInfo.getOrder());
            }
        }
        return os.toByteArray();
    }

    public static Response decodeResponse(IoBuffer buffer, Session session, String charsetName) throws ProtocolException {
        if (buffer.remaining() < TarsHelper.HEAD_SIZE) {
            return null;
        }

        int length = buffer.getInt() - TarsHelper.HEAD_SIZE;
        if (length > TarsHelper.PACKAGE_MAX_LENGTH || length <= 0) {
            throw new ProtocolException("the length header of the package must be between 0~10M bytes. data length:" + Integer.toHexString(length));
        }
        if (buffer.remaining() < length) {
            return null;
        }

        byte[] bytes = new byte[length];
        buffer.get(bytes);

        TarsServantResponse response = new TarsServantResponse(session);
        response.setCharsetName(charsetName);

        TarsInputStream is = new TarsInputStream(bytes);
        is.setServerEncoding(charsetName);

        response.setVersion(is.read((short) 0, 1, true));
        response.setPacketType(is.read((byte) 0, 2, true));
        response.setRequestId(is.read((int) 0, 3, true));
        response.setMessageType(is.read((int) 0, 4, true));
        response.setRet(is.read((int) 0, 5, true));
        if (response.getRet() == TarsHelper.SERVERSUCCESS) {
            response.setInputStream(is);
        }

        return response;
    }

    public static void decodeResponseBody(TarsServantResponse response) throws ProtocolException {
        TarsServantRequest request = response.getRequest();
        if (request.isAsync()) {
            return;
        }
        TarsInputStream is = response.getInputStream();

        TarsInputStream jis = new TarsInputStream(is.read(new byte[] {}, 6, true));
        jis.setServerEncoding(response.getCharsetName());

        TarsMethodInfo methodInfo = request.getMethodInfo();
        TarsMethodParameterInfo returnInfo = methodInfo.getReturnInfo();
        if (returnInfo != null && Void.TYPE != returnInfo.getType()) {
            response.setResult(jis.read(returnInfo.getStamp(), returnInfo.getOrder(), true));
        }

        List<TarsMethodParameterInfo> list = methodInfo.getParametersList();
        for (TarsMethodParameterInfo info : list) {
            if (!TarsHelper.isHolder(info.getAnnotations())) {
                continue;
            }
            try {
                TarsHelper.setHolderValue(request.getMethodParameters()[info.getOrder() - 1], jis.read(info.getStamp(), info.getOrder(), true));
            } catch (Exception e) {
                throw new ProtocolException(e);
            }
        }
        response.setStatus((HashMap<String, String>) is.read(TarsHelper.STAMP_MAP, 7, false));
    }

    public static Object[] decodeCallbackArgs(TarsServantResponse response) throws ProtocolException {
        TarsInputStream jis = new TarsInputStream(response.getInputStream().read(new byte[] {}, 6, true));
        jis.setServerEncoding(response.getCharsetName());

        TarsServantRequest request = response.getRequest();

        TarsMethodInfo methodInfo = null;
        Map<Method, TarsMethodInfo> map = AnalystManager.getInstance().getMethodMap(request.getServantName());
        for (Iterator<Entry<Method, TarsMethodInfo>> it = map.entrySet().iterator(); it.hasNext();) {
            Entry<Method, TarsMethodInfo> entry = it.next();
            if (entry.getKey().getName().equals(request.getFunctionName())) {
                methodInfo = entry.getValue();
            }
        }

        List<Object> list = new ArrayList<Object>();
        TarsMethodParameterInfo returnInfo = methodInfo.getReturnInfo();
        if (returnInfo != null && Void.TYPE != returnInfo.getType()) {
            list.add(jis.read(returnInfo.getStamp(), returnInfo.getOrder(), true));
        }

        List<TarsMethodParameterInfo> parameterInfoList = methodInfo.getParametersList();
        for (TarsMethodParameterInfo info : parameterInfoList) {
            if (TarsHelper.isContext(info.getAnnotations()) || TarsHelper.isCallback(info.getAnnotations())) {
                continue;
            }

            if (TarsHelper.isHolder(info.getAnnotations())) {
                list.add(jis.read(info.getStamp(), info.getOrder(), false));
            }
        }
        return list.toArray();
    }

    public static TarsServantRequest decodeRequest(IoBuffer buffer, Session session, String charsetName) throws ProtocolException {
        if (buffer.remaining() < 4) {
            return null;
        }
        int length = buffer.getInt() - TarsHelper.HEAD_SIZE;
        if (length > TarsHelper.PACKAGE_MAX_LENGTH || length <= 0) {
            throw new ProtocolException("the length header of the package must be between 0~10M bytes. data length:" + Integer.toHexString(length));
        }
        if (buffer.remaining() < length) {
            return null;
        }

        byte[] reads = new byte[length];
        buffer.get(reads);
        TarsInputStream jis = new TarsInputStream(reads);
        TarsServantRequest request = new TarsServantRequest(session);
        try {
            short version = jis.read(TarsHelper.STAMP_SHORT.shortValue(), 1, true);
            byte packetType = jis.read(TarsHelper.STAMP_BYTE.byteValue(), 2, true);
            int messageType = jis.read(TarsHelper.STAMP_INT.intValue(), 3, true);
            int requestId = jis.read(TarsHelper.STAMP_INT.intValue(), 4, true);
            String servantName = jis.readString(5, true);
            String methodName = jis.readString(6, true);
            request.setVersion(version);
            request.setPacketType(packetType);
            request.setMessageType(messageType);
            request.setRequestId(requestId);
            request.setServantName(servantName);
            request.setFunctionName(methodName);
            request.setInputStream(jis);
            request.setCharsetName(charsetName);
        } catch (Exception e) {
            System.err.println(e);
            request.setRet(TarsHelper.SERVERDECODEERR);
        }
        return request;

    }

    public static TarsServantRequest decodeRequestBody(TarsServantRequest request) {
        if (request.getRet() != TarsHelper.SERVERSUCCESS) {
            return request;
        }
        if (TarsHelper.isPing(request.getFunctionName())) {
            return request;
        }

        TarsInputStream jis = request.getInputStream();
        ClassLoader oldClassLoader = null;
        try {
            oldClassLoader = Thread.currentThread().getContextClassLoader();
            Thread.currentThread().setContextClassLoader(resolveProtocolClassLoader());
            String methodName = request.getFunctionName();
            byte[] data = jis.read(TarsHelper.STAMP_BYTE_ARRAY, 7, true);//数据
            int timeout = jis.read(TarsHelper.STAMP_INT.intValue(), 8, true);//超时时间
            Map<String, String> context = (Map<String, String>) jis.read(TarsHelper.STAMP_MAP, 9, true);//Map<String, String> context
            Map<String, String> status = (Map<String, String>) jis.read(TarsHelper.STAMP_MAP, 10, true);

            request.setTimeout(timeout);
            request.setContext(context);
            request.setStatus(status);

            String servantName = request.getServantName();
            Map<String, TarsMethodInfo> methodInfoMap = AnalystManager.getInstance().getMethodMapByName(servantName);

            if (methodInfoMap == null || methodInfoMap.isEmpty()) {
                request.setRet(TarsHelper.SERVERNOSERVANTERR);
                throw new ProtocolException("no found methodInfo, the context[ROOT], serviceName[" + servantName + "], methodName[" + methodName + "]");
            }
            TarsMethodInfo methodInfo = methodInfoMap.get(methodName);
            if (methodInfo == null) {
                request.setRet(TarsHelper.SERVERNOFUNCERR);
                throw new ProtocolException("no found methodInfo, the context[ROOT], serviceName[" + servantName + "], methodName[" + methodName + "]");
            }

            request.setMethodInfo(methodInfo);
            List<TarsMethodParameterInfo> parametersList = methodInfo.getParametersList();
            if (!CommonUtils.isEmptyCollection(parametersList)) {
                Object[] parameters = new Object[parametersList.size()];
                int i = 0;
                if (TarsHelper.VERSION == request.getVersion()) {//request
                    jis = new TarsInputStream(data);
                    jis.setServerEncoding(request.getCharsetName());//set decode charset name
                    Object value = null;
                    for (TarsMethodParameterInfo parameterInfo : parametersList) {
                        if (TarsHelper.isHolder(parameterInfo.getAnnotations())) {
                            value = new Holder<Object>(jis.read(parameterInfo.getStamp(), parameterInfo.getOrder(), false));
                        } else {
                            value = jis.read(parameterInfo.getStamp(), parameterInfo.getOrder(), false);
                        }
                        parameters[i++] = value;
                    }
                } else if (TarsHelper.VERSION2 == request.getVersion() || TarsHelper.VERSION3 == request.getVersion()) {
                    //wup request
                    UniAttribute unaIn = new UniAttribute();
                    unaIn.setEncodeName(request.getCharsetName());

                    if (request.getVersion() == TarsHelper.VERSION2) {
                        unaIn.decodeVersion2(data);
                    } else if (request.getVersion() == TarsHelper.VERSION3) {
                        unaIn.decodeVersion3(data);
                    }

                    Object value = null;
                    for (TarsMethodParameterInfo parameterInfo : parametersList) {
                        if (TarsHelper.isHolder(parameterInfo.getAnnotations())) {
                            String holderName = TarsHelper.getHolderName(parameterInfo.getAnnotations());
                            if (!StringUtils.isEmpty(holderName)) {
                                value = new Holder<Object>(unaIn.getByClass(holderName, parameterInfo.getStamp()));
                            } else {
                                value = new Holder<Object>();
                            }
                        } else {
                            value = unaIn.getByClass(parameterInfo.getName(), parameterInfo.getStamp());
                        }
                        parameters[i++] = value;
                    }
                } else {
                    request.setRet(TarsHelper.SERVERDECODEERR);
                    System.err.println("un supported protocol, ver=" + request.getVersion());
                }
                request.setMethodParameters(parameters);
            }
        } catch (Throwable ex) {
            if (request.getRet() == TarsHelper.SERVERSUCCESS) {
                request.setRet(TarsHelper.SERVERDECODEERR);
            }
            System.err.println(TarsUtil.getHexdump(jis.getBs()));
        } finally {
            if (oldClassLoader != null) {
                Thread.currentThread().setContextClassLoader(oldClassLoader);
            }
        }
        return request;
    }

    public static ClassLoader resolveProtocolClassLoader() {
        ClassLoader classLoader = ClassLoaderManager.getInstance().getClassLoader("");
        if (classLoader == null) {
            classLoader = Thread.currentThread().getContextClassLoader();
        }
        return classLoader;
    }

    public static IoBuffer encodeResponse(TarsServantResponse response, String charsetName) throws ProtocolException {
        if (response.getPacketType() == TarsHelper.ONEWAY) {
            return null;
        }

        TarsOutputStream jos = new TarsOutputStream();
        jos.setServerEncoding(charsetName);
        try {
            jos.getByteBuffer().putInt(0);
            jos.write(response.getVersion(), 1);
            jos.write(response.getPacketType(), 2);

            if (response.getVersion() == TarsHelper.VERSION) {
                jos.write(response.getRequestId(), 3);
                jos.write(response.getMessageType(), 4);
                jos.write(response.getRet(), 5);
                jos.write(encodeResult(response, charsetName), 6);
                if (response.getStatus() != null) {
                    jos.write(response.getStatus(), 7);
                }
                if (response.getRet() != TarsHelper.SERVERSUCCESS) {
                    jos.write(StringUtils.isEmpty(response.getRemark()) ? "" : response.getRemark(), 8);
                }
            } else if (TarsHelper.VERSION2 == response.getVersion() || TarsHelper.VERSION3 == response.getVersion()) {
                jos.write(response.getMessageType(), 3);
                jos.write(response.getTicketNumber(), 4);
                String servantName = response.getRequest().getServantName();
                jos.write(servantName, 5);
                jos.write(response.getRequest().getFunctionName(), 6);
                jos.write(encodeWupResult(response, charsetName), 7);
                jos.write(response.getTimeout(), 8);
                if (response.getStatus() != null) {
                    jos.write(response.getStatus(), 9);
                }
                if (response.getContext() != null) {
                    jos.write(response.getContext(), 10);
                }
            } else {
                response.setRet(TarsHelper.SERVERENCODEERR);
                System.err.println("un supported protocol, ver=" + response.getVersion());
            }
        } catch (Exception ex) {
            if (response.getRet() == TarsHelper.SERVERSUCCESS) {
                response.setRet(TarsHelper.SERVERENCODEERR);
            }
        }
        ByteBuffer buffer = jos.getByteBuffer();
        int datalen = buffer.position();
        buffer.position(0);
        buffer.putInt(datalen);
        buffer.position(datalen);
        return IoBuffer.wrap(jos.toByteArray());
    }

    private static byte[] encodeResult(TarsServantResponse response, String charsetName) {
        TarsServantRequest request = response.getRequest();
        if (TarsHelper.isPing(request.getFunctionName())) {
            return new byte[] {};
        }

        TarsOutputStream ajos = new TarsOutputStream();
        ajos.setServerEncoding(charsetName);

        int ret = response.getRet();
        Map<String, TarsMethodInfo> methodInfoMap = AnalystManager.getInstance().getMethodMapByName(request.getServantName());
        if (ret == TarsHelper.SERVERSUCCESS && methodInfoMap != null) {
            TarsMethodInfo methodInfo = methodInfoMap.get(request.getFunctionName());
            TarsMethodParameterInfo returnInfo = methodInfo.getReturnInfo();
            if (returnInfo != null && returnInfo.getType() != Void.TYPE && response.getResult() != null) {
                try {
                    ajos.write(response.getResult(), methodInfo.getReturnInfo().getOrder());
                } catch (Exception e) {
                    System.err.println("server encodec response result:" + response.getResult() + " with ex:" + e);
                }
            }

            Object value = null;
            List<TarsMethodParameterInfo> parametersList = methodInfo.getParametersList();
            for (TarsMethodParameterInfo parameterInfo : parametersList) {
                if (TarsHelper.isHolder(parameterInfo.getAnnotations())) {
                    value = request.getMethodParameters()[parameterInfo.getOrder() - 1];
                    if (value != null) {
                        try {
                            ajos.write(TarsHelper.getHolderValue(value), parameterInfo.getOrder());
                        } catch (Exception e) {
                            System.err.println("server encodec response holder:" + value + " with ex:" + e);
                        }
                    }
                }
            }
        }
        return ajos.toByteArray();
    }

    private static byte[] encodeWupResult(TarsServantResponse response, String charsetName) {
        TarsServantRequest request = response.getRequest();
        UniAttribute unaOut = new UniAttribute();
        unaOut.setEncodeName(charsetName);
        if (response.getVersion() == TarsHelper.VERSION3) {
            unaOut.useVersion3();
        }

        int ret = response.getRet();
        Map<String, TarsMethodInfo> methodInfoMap = AnalystManager.getInstance().getMethodMapByName(request.getServantName());
        if (ret == TarsHelper.SERVERSUCCESS && methodInfoMap != null) {
            TarsMethodInfo methodInfo = methodInfoMap.get(request.getFunctionName());
            TarsMethodParameterInfo returnInfo = methodInfo.getReturnInfo();
            if (returnInfo != null && returnInfo.getType() != Void.TYPE && response.getResult() != null) {
                unaOut.put(TarsHelper.STAMP_STRING, response.getResult());
            }

            Object value = null;
            List<TarsMethodParameterInfo> parametersList = methodInfo.getParametersList();
            for (TarsMethodParameterInfo parameterInfo : parametersList) {
                if (TarsHelper.isHolder(parameterInfo.getAnnotations())) {
                    value = request.getMethodParameters()[parameterInfo.getOrder() - 1];
                    if (value != null) {
                        try {
                            String holderName = TarsHelper.getHolderName(parameterInfo.getAnnotations());
                            if (!StringUtils.isEmpty(holderName)) {
                                unaOut.put(holderName, TarsHelper.getHolderValue(value));
                            }
                        } catch (Exception e) {
                            System.err.println("server encodec response holder:" + value + " with ex:" + e);
                        }
                    }
                }
            }
        }
        return unaOut.encode();
    }
*/
}
