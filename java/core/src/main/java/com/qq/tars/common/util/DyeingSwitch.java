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

import com.qq.tars.context.DistributedContext;
import com.qq.tars.context.DistributedContextManager;


public class DyeingSwitch {
	
	public static final String BDYEING = "bDyeing";
	public static final String DYEINGKEY = "dyeingKey";
	public static final String SERVANT = "servantName";
	public static final String FILENAME = "dyeFileName";
	public static final String REQ = "request";
	public static final String RES = "response";
	public static final String STATUS_DYED_KEY = "STATUS_DYED_KEY";
	public static final String STATUS_DYED_FILENAME = "STATUS_DYED_FILENAME";
	
	public static void enableActiveDyeing(final String servant) {
		DistributedContext context = DistributedContextManager.getDistributedContext();
		context.put(BDYEING, true);
		context.put(DYEINGKEY, null);
		context.put(FILENAME, servant == null ? "default" : servant);
	}
	
	public static void enableUnactiveDyeing(final String sDyeingKey, final String servant) {
		DistributedContext context = DistributedContextManager.getDistributedContext();
		context.put(BDYEING, true);
		context.put(DYEINGKEY, sDyeingKey);
		context.put(FILENAME, servant);
	}
	
	public static void closeActiveDyeing() {
		DistributedContext context = DistributedContextManager.getDistributedContext();
		context.put(BDYEING, false);
		context.put(DYEINGKEY, null);
		context.put(FILENAME, null);
	}

}
