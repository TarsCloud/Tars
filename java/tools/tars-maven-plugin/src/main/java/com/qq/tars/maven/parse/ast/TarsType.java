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

import org.antlr.runtime.CommonToken;
import org.antlr.runtime.Token;
import org.antlr.runtime.tree.CommonTree;

public abstract class TarsType extends CommonTree {

	private final String typeName;

	public TarsType(int tokenType, String typeName) {
		super(new CommonToken(tokenType));
		this.typeName = typeName;
	}

	public TarsType(Token token, String typeName) {
		super(token);
		this.typeName = typeName;
	}

	public String typeName() {
		return typeName;
	}

	public boolean isPrimitive() {
		return false;
	}

	public boolean isVector() {
		return false;
	}

	public boolean isMap() {
		return false;
	}

	public boolean isCustom() {
		return false;
	}

	public TarsPrimitiveType asPrimitive() {
		return null;
	}

	public TarsVectorType asVector() {
		return null;
	}

	public TarsMapType asMap() {
		return null;
	}

	public TarsCustomType asCustom() {
		return null;
	}

}
