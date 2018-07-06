package com.qq.tars.client.rpc.tars;

import java.util.List;

import com.qq.tars.common.AbstractFilterChain;
import com.qq.tars.common.Filter;
import com.qq.tars.common.FilterKind;
import com.qq.tars.net.client.Callback;
import com.qq.tars.net.core.Request;
import com.qq.tars.net.core.Response;
import com.qq.tars.protocol.util.TarsHelper;
import com.qq.tars.rpc.exc.ServerException;
import com.qq.tars.rpc.exc.TarsException;
import com.qq.tars.rpc.protocol.tars.TarsServantRequest;
import com.qq.tars.rpc.protocol.tars.TarsServantResponse;

public class TarsCallbackFilterChain extends AbstractFilterChain<Callback<TarsServantResponse>> {
	
	private int expireFlag;

	public TarsCallbackFilterChain(List<Filter> filters, String servant,
			FilterKind kind, Callback<TarsServantResponse> target, int expireFlag) {
		super(filters, servant, kind, target);
		this.expireFlag = expireFlag;
	}

	@Override
	protected void doRealInvoke(Request request, Response response)
			throws Throwable {
		if (request instanceof TarsServantRequest) {
			TarsServantResponse tarsServantResponse = (TarsServantResponse)response;
			if (expireFlag == 0) {
				try {
		            if (tarsServantResponse.getCause() != null) {
		                throw new TarsException(tarsServantResponse.getCause());
		            }
		            if (tarsServantResponse.getRet() != TarsHelper.SERVERSUCCESS) {
		                throw ServerException.makeException(tarsServantResponse.getRet());
		            }
		            if (target != null) {
		                this.target.onCompleted(tarsServantResponse);
		            }
		        } catch (Throwable ex) {
		        	if (target != null) {
		        		target.onException(ex);
		        	}
		            throw ex;
		        }
			} else if (expireFlag == 1) {
	            if (target != null) {
	                this.target.onExpired();
	            }
			}
		}
		
	}

}
