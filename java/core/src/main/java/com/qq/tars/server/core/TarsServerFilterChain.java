package com.qq.tars.server.core;

import java.util.List;

import com.qq.tars.common.AbstractFilterChain;
import com.qq.tars.common.Filter;
import com.qq.tars.common.FilterKind;
import com.qq.tars.net.core.Request;
import com.qq.tars.net.core.Response;
import com.qq.tars.rpc.protocol.tars.TarsServantRequest;
import com.qq.tars.rpc.protocol.tars.TarsServantResponse;

public class TarsServerFilterChain extends AbstractFilterChain<ServantHomeSkeleton> {
	
	public TarsServerFilterChain(List<Filter> filters, String servant,
			FilterKind kind, ServantHomeSkeleton target) {
		super(filters, servant, kind, target);	
	}

	@Override
	protected void doRealInvoke(Request request, Response response)
			throws Throwable {
		if (request instanceof TarsServantRequest && target != null) {
			TarsServantRequest tarsServantRequest = (TarsServantRequest)request;
			Object value = target.invoke(tarsServantRequest.getMethodInfo().getMethod(), tarsServantRequest.getMethodParameters());
			TarsServantResponse tarsServantResponse = (TarsServantResponse) response;
			tarsServantResponse.setResult(value);
		}
	}

}
