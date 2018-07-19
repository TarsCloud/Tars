package com.qq.tars.common;

import com.qq.tars.net.core.Request;
import com.qq.tars.net.core.Response;

public interface FilterChain {
	
	public void doFilter(Request request, Response response) throws Throwable;
}
