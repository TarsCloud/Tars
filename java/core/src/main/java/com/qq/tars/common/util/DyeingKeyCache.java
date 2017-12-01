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

import java.util.concurrent.locks.ReentrantReadWriteLock;

public class DyeingKeyCache {
	
	private static final DyeingKeyCache instance = new DyeingKeyCache();
	
	private static final TableUtil<String, String, String> cache_table = TableUtil.getTable();
	
	private final ReentrantReadWriteLock readWriteLock = new ReentrantReadWriteLock();
	
	private DyeingKeyCache() {
	}
	
	public static DyeingKeyCache getInstance() {
		return instance;
	}
	
	public String get(String servantName, String interfaceName) {
		try {
			readWriteLock.readLock().lock();
			return cache_table.get(servantName, interfaceName);
		} finally {
			readWriteLock.readLock().unlock();
		}
	}
	
	public void set(String servantName, String interfaceName, String routeKey) {
		try {
			readWriteLock.writeLock().lock();
			cache_table.set(servantName, interfaceName, routeKey);
		} finally {
			readWriteLock.writeLock().unlock();
		}
	}

}
