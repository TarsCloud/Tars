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
import org.antlr.runtime.tree.Tree;

public class TarsParam extends CommonTree {

	private final boolean isOut;
	private final boolean isRouteKey;
	private TarsType paramType;
	private final String paramName;

	public TarsParam(int tokenType, String paramName, Token isOut, Token isRouteKey) {
		super(new CommonToken(tokenType));
		this.isOut = isOut != null;
		this.isRouteKey = isRouteKey != null;
		this.paramName = paramName;
	}

	@Override
	public void addChild(Tree child) {
		super.addChild(child);

		if (child instanceof TarsType) {
			paramType = (TarsType) child;
		}
	}

	public String paramName() {
		return paramName;
	}

	public TarsType paramType() {
		return paramType;
	}

	public boolean isOut() {
		return isOut;
	}

	public boolean isRouteKey() {
		return isRouteKey;
	}

}
