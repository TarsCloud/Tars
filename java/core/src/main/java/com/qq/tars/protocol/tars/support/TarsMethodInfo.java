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

package com.qq.tars.protocol.tars.support;

import java.lang.reflect.Method;
import java.util.List;

public class TarsMethodInfo {

    private Method method;
    private TarsMethodParameterInfo returnInfo;
    private List<TarsMethodParameterInfo> parametersList;

    private String serviceName;
    private String comment;
    
    private int routeKeyIndex = -1;

    public String getServiceName() {
        return serviceName;
    }

    public void setServiceName(String serviceName) {
        this.serviceName = serviceName;
    }

    public String getMethodName() {
        return method == null ? null : method.getName();
    }

    public List<TarsMethodParameterInfo> getParametersList() {
        return parametersList;
    }

    public void setParametersList(List<TarsMethodParameterInfo> parametersList) {
        this.parametersList = parametersList;
    }

    public TarsMethodParameterInfo getReturnInfo() {
        return returnInfo;
    }

    public void setReturnInfo(TarsMethodParameterInfo returnInfo) {
        this.returnInfo = returnInfo;
    }

    public Method getMethod() {
        return method;
    }

    public void setMethod(Method method) {
        this.method = method;
    }

    public String getComment() {
        return comment;
    }

    public void setComment(String comment) {
        this.comment = comment;
    }
    
	public int getRouteKeyIndex() {
		return routeKeyIndex;
	}

	public void setRouteKeyIndex(int routeKeyIndex) {
		this.routeKeyIndex = routeKeyIndex;
	}
}
