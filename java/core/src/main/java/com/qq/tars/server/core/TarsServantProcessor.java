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

package com.qq.tars.server.core;

import java.util.Random;

import com.qq.tars.common.support.Endpoint;
import com.qq.tars.common.util.Constants;
import com.qq.tars.common.util.DyeingKeyCache;
import com.qq.tars.common.util.DyeingSwitch;
import com.qq.tars.context.DistributedContext;
import com.qq.tars.context.DistributedContextManager;
import com.qq.tars.net.core.Processor;
import com.qq.tars.net.core.Request;
import com.qq.tars.net.core.Response;
import com.qq.tars.net.core.Session;
import com.qq.tars.protocol.tars.support.TarsMethodInfo;
import com.qq.tars.protocol.util.TarsHelper;
import com.qq.tars.rpc.exc.TarsException;
import com.qq.tars.rpc.protocol.tars.TarsServantRequest;
import com.qq.tars.rpc.protocol.tars.TarsServantResponse;
import com.qq.tars.server.apps.AppContextImpl;
import com.qq.tars.server.config.ConfigurationManager;
import com.qq.tars.server.config.ServantAdapterConfig;
import com.qq.tars.server.config.ServerConfig;
import com.qq.tars.support.log.Logger;
import com.qq.tars.support.log.Logger.LogType;
import com.qq.tars.support.om.OmServiceMngr;
import com.qq.tars.support.stat.InvokeStatHelper;

public class TarsServantProcessor extends Processor {

    private Logger flowLogger = Logger.getLogger("tarsserver.log", LogType.LOCAL);

    private static final String FLOW_SEP_FLAG = "|";

    private static final Random rand = new Random(System.currentTimeMillis());

    @Override
    public Response process(Request req, Session session) {
        AppContainer container = null;
        TarsServantRequest request = null;
        TarsServantResponse response = null;
        ServantHomeSkeleton skeleton = null;
        Object value = null;
        AppContext appContext = null;
        ClassLoader oldClassLoader = null;
        int waitingTime = -1;
        long startTime = req.getProcessTime();
        String remark = "";

        try {
            oldClassLoader = Thread.currentThread().getContextClassLoader();
            request = (TarsServantRequest) req;
            response = createResponse(request, session);
            response.setTicketNumber(req.getTicketNumber());

            if (response.getRet() != TarsHelper.SERVERSUCCESS || TarsHelper.isPing(request.getFunctionName())) {
                return response;
            }
            int maxWaitingTimeInQueue = ConfigurationManager.getInstance().getServerConfig().getServantAdapterConfMap().get(request.getServantName()).getQueueTimeout();
            waitingTime = (int) (startTime - req.getBornTime());
            if (waitingTime > maxWaitingTimeInQueue) {
                throw new TarsException("Wait too long, server busy.");
            }

            container = ContainerManager.getContainer(AppContainer.class);
            Context<?, ?> context = ContextManager.registerContext(request, response);
            context.setAttribute(Context.INTERNAL_START_TIME, startTime);
            context.setAttribute(Context.INTERNAL_CLIENT_IP, session.getRemoteIp());
            context.setAttribute(Context.INTERNAL_APP_NAME, container.getDefaultAppContext().name());
            context.setAttribute(Context.INTERNAL_SERVICE_NAME, request.getServantName());
            context.setAttribute(Context.INTERNAL_METHOD_NAME, request.getFunctionName());
            context.setAttribute(Context.INTERNAL_SESSION_DATA, session);
            
            DistributedContext distributedContext = DistributedContextManager.getDistributedContext();
            distributedContext.put(DyeingSwitch.REQ, request);
            distributedContext.put(DyeingSwitch.RES, response);

            appContext = container.getDefaultAppContext();
            if (appContext == null) throw new RuntimeException("failed to find the application named:[ROOT]");

//            Thread.currentThread().setContextClassLoader(appContext.getAppContextClassLoader());
            preInvokeSkeleton();
            skeleton = appContext.getCapHomeSkeleton(request.getServantName());
            if (skeleton == null) throw new RuntimeException("failed to find the servant named[" + request.getServantName() + "]");

            value = skeleton.invoke(request.getMethodInfo().getMethod(), request.getMethodParameters());
            response.setResult(value);
        } catch (Throwable cause) {
            System.out.println("ERROR: " + cause.getMessage());

            if (response.isAsyncMode()) {
                try {
                    Context<TarsServantRequest, TarsServantResponse> context = ContextManager.getContext();
                    AsyncContext aContext = context.getAttribute(AsyncContext.PORTAL_CAP_ASYNC_CONTEXT_ATTRIBUTE);
                    if (aContext != null) aContext.writeException(cause);
                } catch (Exception ex) {
                    System.out.println("ERROR: " + ex.getMessage());
                }
            } else {
                response.setResult(null);
                response.setCause(cause);
                response.setRet(TarsHelper.SERVERUNKNOWNERR);
                remark = cause.toString();
            }
        } finally {
            if (oldClassLoader != null) {
                Thread.currentThread().setContextClassLoader(oldClassLoader);
            }
            ContextManager.releaseContext();
            if (!response.isAsyncMode()) {
                printServiceFlowLog(flowLogger, request, response.getRet(), (System.currentTimeMillis() - startTime), remark);
            }
            postInvokeSkeleton();
            OmServiceMngr.getInstance().reportWaitingTimeProperty(waitingTime);
            reportServerStat(request, response, startTime);
        }
        return response;
    }

    private void reportServerStat(TarsServantRequest request, TarsServantResponse response, long startTime) {
        if (request.getVersion() == TarsHelper.VERSION2 || request.getVersion() == TarsHelper.VERSION3) {
            reportServerStat(Constants.TARS_TUP_CLIENT, request, response, startTime);
        } else if (request.getMessageType() == TarsHelper.ONEWAY) {
            reportServerStat(Constants.TARS_ONE_WAY_CLIENT, request, response, startTime);
        }
    }

    private void reportServerStat(String moduleName, TarsServantRequest request, TarsServantResponse response,
                                  long startTime) {
        ServerConfig serverConfig = ConfigurationManager.getInstance().getServerConfig();
        ServantAdapterConfig servantAdapterConfig = serverConfig.getServantAdapterConfMap().get(request.getServantName());
        if (servantAdapterConfig == null) {
            return;
        }
        Endpoint serverEndpoint = servantAdapterConfig.getEndpoint();
        String masterIp = request.getIoSession().getRemoteIp();
        int result = response.getRet() == TarsHelper.SERVERSUCCESS ? Constants.INVOKE_STATUS_SUCC : Constants.INVOKE_STATUS_EXEC;
        InvokeStatHelper.getInstance().addProxyStat(request.getServantName()).addInvokeTime(moduleName, request.getServantName(), serverConfig.getCommunicatorConfig().getSetDivision(), request.getFunctionName(), (masterIp == null ? "0.0.0.0" : masterIp), serverEndpoint.host(), serverEndpoint.port(), result, (System.currentTimeMillis() - startTime));
    }

    public static void printServiceFlowLog(Logger logger, TarsServantRequest request, int status, long cost,
                                           String remark) {
        if (status == TarsHelper.SERVERSUCCESS && !isFlowLogEnable()) return;

        StringBuilder sb = new StringBuilder();
        Object args[] = request.getMethodParameters();
        int len = 25;

        sb.append(FLOW_SEP_FLAG);
        sb.append(request.getIoSession().getRemoteIp()).append(FLOW_SEP_FLAG);
        sb.append(request.getFunctionName()).append(FLOW_SEP_FLAG);

        if (null != args) {
            StringBuilder sbArgs = new StringBuilder();
            for (int i = 0; i < args.length; i++) {
                if (args[i] == null) {
                    sbArgs.append("NULL").append(",");
                } else if (args[i] instanceof Number || args[i] instanceof Boolean) {
                    sbArgs.append(args[i]).append(",");
                } else {
                    sbArgs.append(encodeStringParam(args[i].toString(), len)).append(",");
                }
            }
            sbArgs = sbArgs.length() >= 1 ? sbArgs.deleteCharAt(sbArgs.length() - 1) : sbArgs;
            sb.append(sbArgs);
        }

        sb.append(FLOW_SEP_FLAG);
        sb.append(status).append(FLOW_SEP_FLAG).append(cost);
        sb.append(FLOW_SEP_FLAG).append(remark);

        logger.info(sb.toString());
    }

    private static boolean isFlowLogEnable() {
        return ConfigurationManager.getInstance().getServerConfig().getLogRate() - rand.nextInt(100) > 0;
    }

    private static String encodeStringParam(String longParam, int len) {
        if (longParam == null || longParam.length() == 0) return "";
        String shortParam = longParam;

        if (len > 0) {
            shortParam = longParam.length() > len ? longParam.substring(0, len) + "..(" + longParam.length() + ")" : longParam;
        }

        return shortParam.replaceAll(" ", "_").replaceAll(" ", "_").replaceAll("\n", "+").replace(',', '，').replace('(', '（').replace(')', '）');
    }

    private TarsServantResponse createResponse(TarsServantRequest request, Session session) {
        TarsServantResponse response = new TarsServantResponse(session);
        response.setRet(request.getRet());
        response.setVersion(request.getVersion());
        response.setPacketType(request.getPacketType());
        response.setMessageType(request.getMessageType());
        response.setStatus(request.getStatus());
        response.setRequest(request);
        response.setCharsetName(request.getCharsetName());
        response.setTimeout(request.getTimeout());
        response.setContext(request.getContext());
        return response;
    }
    
    public void preInvokeSkeleton() {
    	DistributedContext distributedContext = DistributedContextManager.getDistributedContext();
    	Request request = distributedContext.get(DyeingSwitch.REQ);
    	if (request instanceof TarsServantRequest) {
    		TarsServantRequest tarsServantRequest = (TarsServantRequest)request;
    		initDyeing(tarsServantRequest);
    	}
    }

    public void postInvokeSkeleton() {
    	DistributedContext distributedContext = DistributedContextManager.getDistributedContext();
    	distributedContext.clear();
    }
    
    private void initDyeing(TarsServantRequest request) {
    	String routeKey;
    	String fileName;
    	if ((request.getMessageType() & TarsHelper.MESSAGETYPEDYED) == TarsHelper.MESSAGETYPEDYED) {
    		routeKey = request.getStatus().get(DyeingSwitch.STATUS_DYED_KEY);
    		fileName = request.getStatus().get(DyeingSwitch.STATUS_DYED_FILENAME);
    		DyeingSwitch.enableUnactiveDyeing(routeKey, fileName);
    		return;
    	}
    	String cache_routeKey = DyeingKeyCache.getInstance().get(request.getServantName(), request.getFunctionName());
    	if (cache_routeKey == null) {
    		cache_routeKey =  DyeingKeyCache.getInstance().get(request.getServantName(), "DyeingAllFunctionsFromInterface");
    	}
    	if (cache_routeKey == null) {
    		return;
    	}
    	TarsMethodInfo methodInfo = request.getMethodInfo();
    	if (methodInfo.getRouteKeyIndex() != -1) {
    		Object[] paramters = request.getMethodParameters();
    		Object value = paramters[methodInfo.getRouteKeyIndex()];
    		if (cache_routeKey.equals(value.toString())) {
    			routeKey = cache_routeKey;
    			fileName = ConfigurationManager.getInstance().getServerConfig().getServerName();
    			DyeingSwitch.enableUnactiveDyeing(routeKey, fileName);
    		}
    	}
    }
}
