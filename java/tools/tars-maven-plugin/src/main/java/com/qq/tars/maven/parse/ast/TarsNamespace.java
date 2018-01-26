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
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.antlr.runtime.CommonToken;
import org.antlr.runtime.tree.CommonTree;
import org.antlr.runtime.tree.Tree;

public class TarsNamespace extends CommonTree {

	private final String namespace;
	private final List<TarsStruct> structList = new ArrayList<TarsStruct>();
	private final List<TarsInterface> interfaceList = new ArrayList<TarsInterface>();
	private final List<TarsEnum> enumList = new ArrayList<TarsEnum>();
	private final List<TarsConst> constList = new ArrayList<TarsConst>();
	private final Map<String, TarsKey> keyMap = new HashMap<String, TarsKey>();

	public TarsNamespace(int tokenType, String identifier) {
		super(new CommonToken(tokenType));
		this.namespace = identifier;
	}

	public String namespace() {
		return namespace;
	}

	@Override
	public void addChild(Tree child) {
		super.addChild(child);

		if (child instanceof TarsStruct) {
			structList.add((TarsStruct) child);
		} else if (child instanceof TarsInterface) {
			interfaceList.add((TarsInterface) child);
		} else if (child instanceof TarsEnum) {
			enumList.add((TarsEnum) child);
		} else if (child instanceof TarsConst) {
			constList.add((TarsConst) child);
		} else if (child instanceof TarsKey) {
			TarsKey jk = (TarsKey) child;
			keyMap.put(jk.structName(), jk);
		}
	}

	public List<TarsStruct> structList() {
		return structList;
	}

	public List<TarsInterface> interfaceList() {
		return interfaceList;
	}

	public List<TarsEnum> enumList() {
		return enumList;
	}

	public List<TarsConst> constList() {
		return constList;
	}

	public Map<String, TarsKey> keyMap() {
		return keyMap;
	}

}
