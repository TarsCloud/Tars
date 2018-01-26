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

import static com.qq.tars.maven.parse.TarsLexer.*;

public class TarsPrimitiveType extends TarsType {

    public static enum PrimitiveType {
        VOID, BOOL, BYTE, SHORT, INT, LONG, FLOAT, DOUBLE, STRING
    }

    private final PrimitiveType primitiveType;

    public TarsPrimitiveType(Token token) {
        super(token, toPrimitiveType(token.getType()).name());
        this.primitiveType = toPrimitiveType(token.getType());
    }

    public TarsPrimitiveType(int type) {
        this(new CommonToken(type));
    }

    @Override
    public boolean isPrimitive() {
        return true;
    }

    @Override
    public TarsPrimitiveType asPrimitive() {
        return this;
    }

    public PrimitiveType primitiveType() {
        return primitiveType;
    }

    public boolean isVoid() {
        return primitiveType == PrimitiveType.VOID;
    }

    private static PrimitiveType toPrimitiveType(int tokenType) {
        PrimitiveType type = null;
        switch (tokenType) {
            case TARS_VOID:
                type = PrimitiveType.VOID;
                break;
            case TARS_BOOL:
                type = PrimitiveType.BOOL;
                break;
            case TARS_BYTE:
                type = PrimitiveType.BYTE;
                break;
            case TARS_SHORT:
                type = PrimitiveType.SHORT;
                break;
            case TARS_INT:
                type = PrimitiveType.INT;
                break;
            case TARS_LONG:
                type = PrimitiveType.LONG;
                break;
            case TARS_FLOAT:
                type = PrimitiveType.FLOAT;
                break;
            case TARS_DOUBLE:
                type = PrimitiveType.DOUBLE;
                break;
            case TARS_STRING:
                type = PrimitiveType.STRING;
                break;
        }
        return type;
    }

}
