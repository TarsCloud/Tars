/*
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

package com.qq.tars.exception;

import com.qq.common.CodeEnableRuntimeException;

public class DBUpdateNothingRuntimeException extends CodeEnableRuntimeException {

    public DBUpdateNothingRuntimeException(Throwable cause) {
        super(cause);
    }

    public DBUpdateNothingRuntimeException() {
    }

    public DBUpdateNothingRuntimeException(String message) {
        super(message);
    }

    public DBUpdateNothingRuntimeException(String message, Throwable cause) {
        super(message, cause);
    }

    @Override
    public String getCode() {
        return "12040000";
    }
}
