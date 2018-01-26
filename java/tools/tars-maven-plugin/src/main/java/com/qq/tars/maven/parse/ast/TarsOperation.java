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

import java.util.ArrayList;
import java.util.List;

import org.antlr.runtime.CommonToken;
import org.antlr.runtime.tree.CommonTree;
import org.antlr.runtime.tree.Tree;

public class TarsOperation extends CommonTree {

	private TarsType retType;
	private final String oprationName;
	private final List<TarsParam> paramList = new ArrayList<TarsParam>();

	public TarsOperation(int tokenType, String oprationName) {
		super(new CommonToken(tokenType));
		this.oprationName = oprationName;
	}

	@Override
	public void addChild(Tree child) {
		super.addChild(child);

		if (child instanceof TarsType) {
			retType = (TarsType) child;
		} else if (child instanceof TarsParam) {
			paramList.add((TarsParam) child);
		}
	}

	public TarsType retType() {
		return retType;
	}

	public String oprationName() {
		return oprationName;
	}

	public List<TarsParam> paramList() {
		return paramList;
	}

}
