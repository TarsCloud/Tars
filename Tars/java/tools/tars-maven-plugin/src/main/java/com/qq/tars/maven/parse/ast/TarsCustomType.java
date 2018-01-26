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
package com.qq.tars.maven.parse.ast;

public class TarsCustomType extends TarsType {

	private final String namespace;

	public TarsCustomType(int tokenType, String typeName) {
		super(tokenType, typeName);
		this.namespace = null;
	}

	public TarsCustomType(int tokenType, String namespace, String typeName) {
		super(tokenType, typeName);
		this.namespace = namespace;
	}

	@Override
	public boolean isCustom() {
		return true;
	}

	@Override
	public TarsCustomType asCustom() {
		return this;
	}

	public String namespace() {
		return namespace;
	}

}
