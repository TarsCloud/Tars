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

import org.antlr.runtime.Token;
import org.antlr.runtime.tree.Tree;

import com.qq.tars.maven.parse.ast.TarsPrimitiveType.PrimitiveType;

public class TarsVectorType extends TarsType {

	private TarsType subType;

	public TarsVectorType(Token token) {
		super(token, "vector");
	}

	@Override
	public boolean isVector() {
		return true;
	}

	@Override
	public TarsVectorType asVector() {
		return this;
	}

	@Override
	public void addChild(Tree child) {
		super.addChild(child);
		if (child instanceof TarsType) {
			subType = (TarsType) child;
		}
	}

	public TarsType subType() {
		return subType;
	}

	public boolean isByteArray() {
		return subType.isPrimitive() && subType.asPrimitive().primitiveType() == PrimitiveType.BYTE;
	}

}
