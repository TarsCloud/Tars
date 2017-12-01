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
package com.qq.tars.context;

import java.util.Map;

import com.qq.tars.context.exc.CodecException;
import com.qq.tars.context.impl.DistributedContextImpl;

public class DistributedContextManager {
	
    private static final DistributedContext context = new DistributedContextImpl();
	
	private static DistributedContextCodec _codec;
	
	private static boolean canCodec() {
		return _codec == null ? false : true;
	}
	
	public static final DistributedContext getDistributedContext() {
		return context;
	}
	
	public static final void releaseDistributedContext() {
		getDistributedContext().clear();
	}
	
	public static void setDistributedContextCodec(DistributedContextCodec codec) {
		_codec = codec;
	}
	
	public static byte[] serializeContext2Bytes() throws CodecException {
		if (canCodec()) {
			return _codec.context2Bytes(getDistributedContext().getAttrbuites());
		} else {
			throw new CodecException("codec is null");
		}
	}
	
	public static void deserializeBytes2Context(byte[] src) throws CodecException {
		if (canCodec()) {
			Map<String, Object> attributes = _codec.loadBytes(src);
			for (Map.Entry<String, Object> attribute : attributes.entrySet()) {
				getDistributedContext().put(attribute.getKey(), attribute.getValue());
			}
		} else {
			throw new CodecException("codec is null");
		}
	}
	
	public static String seriallizeContext2String() throws CodecException {
		if (canCodec()) {
			return _codec.context2String(getDistributedContext().getAttrbuites());
		} else {
			throw new CodecException("codec is null");
		}
	}
	
	public static void deserializeString2Context(String src) throws CodecException {
		if (canCodec()) {
			Map<String, Object> attributes = _codec.loadString(src);
			for (Map.Entry<String, Object> attribute : attributes.entrySet()) {
				getDistributedContext().put(attribute.getKey(), attribute.getValue());
			}
		} else {
			throw new CodecException("codec is null");
		}
	}

}
