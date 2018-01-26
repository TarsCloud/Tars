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
package com.qq.tars.common.util;

import java.util.LinkedHashMap;
import java.util.Map;

public class TableUtil<R, C, V> {
	
	private final Map<R, Map<C, V>> table;
	
	private TableUtil(Map<R, Map<C, V>> _table) {
		table = _table;
	}
	
	public static <R, C, V> TableUtil<R, C,V> getTable() {
		Map<R, Map<C, V>> newTable = new LinkedHashMap<R, Map<C,V>>();
		return new TableUtil<R, C, V>(newTable);
	}
	
	public V get(R rowKey, C columnKey) {
		if (rowKey == null || columnKey == null) {
			return null;
		}
		Map<C, V> columns = table.get(rowKey);
		if (columns == null) {
			return null;
		}
		V value = columns.get(columnKey);
		return value;
	}
	
	public void set(R rowKey, C columnKey, V value) {
		if (rowKey == null || columnKey == null) {
			return;
		}
		Map<C, V> columns = table.get(rowKey);
		if (columns == null) {
			columns = new LinkedHashMap<C, V>();
			table.put(rowKey, columns);
		}
		columns.put(columnKey, value);
	}

}
