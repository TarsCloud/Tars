package com.qq.tars.support.trace;

import io.opentracing.Scope;
import io.opentracing.Tracer;

import java.io.Closeable;
import java.io.IOException;

public class TraceContext implements Closeable {
	
	private final ThreadLocal<Tracer> currentTracer = new ThreadLocal<Tracer>();
	
	private static final TraceContext instance = new TraceContext();
	
	private TraceContext() {}
	
	public static TraceContext getIntance() {
		return instance;
	}
	
	public TraceContext initCurrentTrace(String servantName) {
		Tracer tracer = null;
		if (servantName == null || servantName.isEmpty()) {
			return instance;
		}
		tracer = TraceManager.getInstance().getCurrentTracer(servantName);
		currentTracer.set(tracer);
		return instance;
	}
	
	public void clear() {
		currentTracer.remove();
	}
	
	public Tracer getCurrentTracer() {
		return currentTracer.get();
	}	
	
	public void tag(String key, String value) {
		Tracer tracer = currentTracer.get();
		if (tracer != null) {
			Scope scope = tracer.scopeManager().active();
			if (scope != null) {
				scope.span().setTag(key, value);
			}
		}
	}
	
	public void log(String event, long timestamp) {
		Tracer tracer = currentTracer.get();
		if (tracer != null) {
			Scope scope = tracer.scopeManager().active();
			if (scope != null) {
				scope.span().log(timestamp, event);
			}
		}
	}
	
	public void  log(String event) {
		Tracer tracer = currentTracer.get();
		if (tracer != null) {
			Scope scope = tracer.scopeManager().active();
			if (scope != null) {
				scope.span().log(event);
			}
		}
	}

	@Override
	public void close() throws IOException {
		clear();		
	}

}
