package com.qq.tars.support.trace;

import io.opentracing.Tracer;

import java.util.HashMap;
import java.util.Map;

import zipkin2.codec.Encoding;
import zipkin2.reporter.AsyncReporter;
import zipkin2.reporter.Reporter;
import zipkin2.reporter.Sender;
import zipkin2.reporter.urlconnection.URLConnectionSender;
import brave.Tracing;
import brave.opentracing.BraveTracer;

import com.qq.tars.server.config.ConfigurationManager;
import com.qq.tars.server.config.ServerConfig;
import com.qq.tars.support.om.OmConstants;
import com.qq.tars.support.trace.exc.NotSupportedSuchSampleEncodeingException;
import com.qq.tars.support.trace.exc.NotSupportedSuchSampleTypeException;

public class TarsTraceZipkinConfiguration {
	
	private ServerConfig serverConfig = ConfigurationManager.getInstance().getServerConfig();
	
	private volatile boolean isTrace = false;
	
	private Sender sender;
	
	private Reporter<zipkin2.Span> reporter;
	
	private static final TarsTraceZipkinConfiguration instance = new TarsTraceZipkinConfiguration();
	
	private TarsTraceZipkinConfiguration() {}
	
	public static TarsTraceZipkinConfiguration getInstance() {
		return instance;
	}
	
	public void init() {
		isTrace = serverConfig.getSampleRate() > 0;
		if (isTrace) {
			try {
				createSender();
				reporter = AsyncReporter.builder(sender).build();
				Map<String, Tracer> traces = new HashMap<String, Tracer>();
				for (String servant : serverConfig.getServantAdapterConfMap().keySet()) {
					if (!servant.equals(OmConstants.AdminServant)) {
						Tracing tracing = Tracing.newBuilder().localServiceName(servant)
								.spanReporter(reporter).sampler(brave.sampler.Sampler.create(serverConfig.getSampleRate())).build();
						Tracer tracer = BraveTracer.create(tracing);
						traces.put(servant, tracer);
					}
				}
				TraceManager.getInstance().putTracers(traces);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
	}
	
	private void createSender() throws NotSupportedSuchSampleTypeException, NotSupportedSuchSampleEncodeingException {
		if ("http".equals(serverConfig.getSampleType())) {
			String baseurl = serverConfig.getSampleAddress();
			String url = baseurl + (baseurl.endsWith("/") ? "" : "/") + "api/v2/spans";
			Encoding codec = createCodec();
			sender = URLConnectionSender.newBuilder().encoding(codec).endpoint(url).build();
		} else if ("kafka08".equals(serverConfig.getSampleType())) {
			Encoding codec = createCodec();
			sender = zipkin2.reporter.kafka08.KafkaSender.newBuilder().encoding(codec)
					.bootstrapServers(serverConfig.getSampleAddress()).build();
		} else if ("kafka".equals(serverConfig.getSampleType())) {
			Encoding codec = createCodec();
			sender = zipkin2.reporter.kafka11.KafkaSender.newBuilder().encoding(codec)
					.bootstrapServers(serverConfig.getSampleAddress()).build();
		} else {
			throw new NotSupportedSuchSampleTypeException("unsupported sample type");
		}
	}
	
	private Encoding createCodec() throws NotSupportedSuchSampleEncodeingException {
		if ("json".endsWith(serverConfig.getSampleEncoding())) {
			return Encoding.JSON;
		}
		if ("proto".endsWith(serverConfig.getSampleEncoding())) {
			return Encoding.PROTO3;
		}
		throw new NotSupportedSuchSampleEncodeingException("unsupported sample encoding");
	}

}
