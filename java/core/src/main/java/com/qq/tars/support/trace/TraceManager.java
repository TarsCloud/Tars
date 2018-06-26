package com.qq.tars.support.trace;

import io.opentracing.Span;
import io.opentracing.Tracer;

import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import com.qq.tars.client.util.Pair;

public class TraceManager {
	
	public static final String INTERNAL_SERVANT_NAME = "internal_servant";
	
	public static final String PROTOCOL = "protocol";
	
	private Map<String, Tracer> traces = new HashMap<String, Tracer>();
	
	private static final ConcurrentHashMap<Integer, Pair<Tracer, Span>> spanMap = new ConcurrentHashMap<Integer, Pair<Tracer, Span>>();
	
	private static final TraceManager instance = new TraceManager();
	
	private TraceManager() {}
	
	public static TraceManager getInstance() {
		return instance;
	}
	
	public Tracer getCurrentTracer(String servant) {
		return traces.get(servant);
	}
	
	public void putTracers(Map<String, Tracer> _traces) {
		traces.putAll(_traces);
	}
	
	public Pair<Tracer, Span> getCurrentSpan(int ticketNum) {
		return spanMap.get(ticketNum);
	}
	
	public void putSpan(int ticketNum, Tracer tracer, Span span) {
		Pair<Tracer, Span> pair = new Pair<Tracer, Span>(tracer, span);
		spanMap.put(ticketNum, pair);
	}
	
	public void removeSpan(int ticketNum) {
		spanMap.remove(ticketNum);
	}

}
