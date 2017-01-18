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

import static com.qq.tars.maven.parse.TarsLexer.*;

public class TarsKey extends CommonTree {

    private final String structName;
    private final List<String> keyList = new ArrayList<String>();

    public TarsKey(int tokenType, String structName) {
        super(new CommonToken(tokenType));
        this.structName = structName;
    }

    @Override
    public void addChild(Tree child) {
        super.addChild(child);
        if (child.getType() == TARS_IDENTIFIER) {
            keyList.add(child.getText());
        }
    }

    public String structName() {
        return structName;
    }

    public List<String> keyList() {
        return keyList;
    }

}
