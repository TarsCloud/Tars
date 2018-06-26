package com.qq.tars.support.trace;

import io.opentracing.Scope;
import io.opentracing.Span;
import io.opentracing.Tracer;

import com.qq.tars.client.util.Pair;
import com.qq.tars.common.Filter;
import com.qq.tars.common.FilterChain;
import com.qq.tars.net.core.Request;
import com.qq.tars.net.core.Response;
import com.qq.tars.rpc.exc.TimeoutException;
import com.qq.tars.rpc.protocol.tars.TarsServantRequest;
import com.qq.tars.rpc.protocol.tars.TarsServantResponse;
import com.qq.tars.server.config.ConfigurationManager;

public class TraceCallbackFilter implements Filter {
	
	private boolean isTrace = false;
	
	@Override
	public void init() {
		isTrace = ConfigurationManager.getInstance().getServerConfig().getSampleRate() > 0;
	}

	@Override
	public void doFilter(Request request, Response response, FilterChain chain)
			throws Throwable {		
		if (!isTrace) {
			chain.doFilter(request, response);
			return;
		}
		if (response == null || !(request instanceof TarsServantRequest) || !TraceUtil.checkServant(((TarsServantRequest)request).getServantName())) {
			chain.doFilter(request, response);
			return;
		}
		Pair<Tracer, Span> entry = TraceManager.getInstance().getCurrentSpan(response.getTicketNumber());
		if (entry != null && entry.getFirst() != null && entry.getSecond() != null) {
			try (Scope scope = entry.getFirst().scopeManager().activate(entry.getSecond(), true)) {
					TarsServantResponse tarsServantResponse = (TarsServantResponse)response;
					if (tarsServantResponse.getCause() instanceof TimeoutException) {
						scope.span().log(tarsServantResponse.getCause().getMessage());
					} else {
						scope.span().setTag("tars.retcode", tarsServantResponse.getRet());
					}
					
			}
			TraceManager.getInstance().removeSpan(response.getTicketNumber());
		}
		
		chain.doFilter(request, response);
	}

	@Override
	public void destroy() {
		

	}


}
