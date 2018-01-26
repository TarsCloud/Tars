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

import static com.qq.tars.maven.parse.TarsLexer.*;

public class TarsStructMember extends CommonTree {

    private final int tag;
    private final boolean isRequire;
    private final String memberName;
    private final String defaultValue;

    private TarsType memberType;

    public TarsStructMember(int tokenType, String tag, Token isRequire, String memberName, String defaultValue) {
        super(new CommonToken(tokenType));
        this.tag = Integer.parseInt(tag);
        if (isRequire.getType() == TARS_REQUIRE) {
            this.isRequire = true;
        } else if (isRequire.getType() == TARS_OPTIONAL) {
            this.isRequire = false;
        } else {
            throw new RuntimeException("Error Struct require/optional");
        }
        this.memberName = memberName;
        this.defaultValue = defaultValue;
    }

    @Override
    public void addChild(Tree child) {
        super.addChild(child);

        if (child instanceof TarsType) {
            memberType = (TarsType) child;
        }
    }

    public int tag() {
        return tag;
    }

    public boolean isRequire() {
        return isRequire;
    }

    public TarsType memberType() {
        return memberType;
    }

    public String memberName() {
        return memberName;
    }

    public String defaultValue() {
        return defaultValue;
    }

}
