package com.qq.tars.common;

import com.qq.tars.net.core.Request;
import com.qq.tars.net.core.Response;

public interface Filter {
	
	public void init();
	
	public void doFilter(Request request, Response response, FilterChain chain) throws Throwable;
	
	public void destroy();

}
