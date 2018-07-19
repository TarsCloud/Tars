package com.qq.tars.common;

import java.util.Iterator;
import java.util.List;

import com.qq.tars.net.core.Request;
import com.qq.tars.net.core.Response;

public abstract class AbstractFilterChain<T> implements FilterChain {

	private List<Filter> filters;
	
	protected String servant;
	
	protected FilterKind kind;
	
	protected T target;
	
	private Iterator<Filter> iteator;
	
	public AbstractFilterChain(List<Filter> filters, String servant, FilterKind kind, T target) {
		this.filters = filters;
		this.servant = servant;
		this.kind = kind;
		this.target = target;
	}

	@Override
	public void doFilter(Request request, Response response) throws Throwable {
		Filter filter = getFilter();
		if (filter != null) {
			filter.doFilter(request, response, this);
		} else {
			doRealInvoke(request, response);
		}

	}
	
	private Filter getFilter() {
		if (filters == null || filters.isEmpty()) {
			return null;
		}
		if (iteator == null) {
			iteator = filters.iterator();
		}
		if (iteator.hasNext()) {
			return iteator.next();
		}
		return null;
	}
	
	protected abstract void doRealInvoke(Request request, Response response) throws Throwable;

}
