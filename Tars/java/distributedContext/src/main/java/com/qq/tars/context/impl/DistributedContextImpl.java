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
package com.qq.tars.context.impl;

import java.util.Map;
import java.util.Set;

import com.qq.tars.context.DistributedContext;

public class DistributedContextImpl implements DistributedContext {

	private static ThreadLocal<ContextItem> contexts = new ThreadLocal<ContextItem>() {
		protected ContextItem  initialValue() {
			return new ContextItem();
		}
	};

	public void clear() {
		contexts.remove();
	}

	@SuppressWarnings("unchecked")
	public <T> T get(String name) {
		try {
			ContextItem item = getItem();
			Object value = item.getAttribute(name);
			return value != null ? (T)value : null;
		} catch (Exception e) {
			e.printStackTrace();
		}
		return null;
	}

	public Set<String> keySet() {
		return getItem().getMap().keySet();
	}

	public <T> void put(String name, T value) {
		getItem().setAttribute(name, value);
	}

	public int size() {
		return getItem().getMap().size();
	}
	
	private ContextItem getItem() {
		ContextItem item = contexts.get();
		if (item == null) {
			item = new ContextItem();
			contexts.set(item);
		}
		return item;
	}

	public Map<String, Object> getAttrbuites() {
		return getItem().getMap();
	}

}
