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

package com.qq.tars.support.log;

import com.qq.tars.support.log.Logger.Level;
import com.qq.tars.support.log.util.Utils;

public final class LogItem {

    private long time;

    private Level level;

    private String str;
    
    private boolean dyeFlag;
    
    private String dyeFileName;
    
    private String dyeServantName;

    private ThrowableInfo throwInfo = null;

    public LogItem(Level level, String str, boolean dyeFlag, String dyeFileName, String dyeServantName, Throwable th) {
        this.time = System.currentTimeMillis();
        this.level = level;
        this.str = str;
        this.dyeFlag = dyeFlag;
        this.dyeFileName = dyeFileName;
        this.dyeServantName = dyeServantName;
        if (th != null) {
            this.throwInfo = new ThrowableInfo(th);
        }
    }

    public long getTime() {
        return this.time;
    }

    public String getStr() {
        return this.str;
    }
    
    public boolean getDyeFlag() {
    	return this.dyeFlag;
    }
    
    public String getDyeFileName() {
    	return this.dyeFileName;
    }
    
    public String getDyeServantName() {
    	return this.dyeServantName;
    }

    public ThrowableInfo getThrowInfo() {
        return this.throwInfo;
    }

    String toStringNoEndReturn() {
        StringBuilder strBuf = new StringBuilder();
        strBuf.append(Utils.getDateAllInfo(this.time)).append("\t");
        strBuf.append(this.level.name()).append("\t");
        strBuf.append(this.str);
        if (this.throwInfo != null) {
            strBuf.append(this.throwInfo.getThrowableStr());
        }

        return strBuf.toString();
    }
    
    public String toDyeingString() {
        StringBuilder strBuf = new StringBuilder();
        strBuf.append(this.dyeServantName).append("|");
        strBuf.append(Utils.getDateAllInfo(this.time)).append("\t");
        strBuf.append(this.level.name()).append("\t");
        strBuf.append(this.str).append("\n");
        if (this.throwInfo != null) {
            strBuf.append(this.throwInfo.getThrowableStr());
        }
        return strBuf.toString();
    }

    @Override
    public String toString() {
        StringBuilder strBuf = new StringBuilder();
        strBuf.append(Utils.getDateAllInfo(this.time)).append("\t");
        strBuf.append(this.level.name()).append("\t");
        strBuf.append(this.str).append("\n");
        if (this.throwInfo != null) {
            strBuf.append(this.throwInfo.getThrowableStr());
        }
        return strBuf.toString();
    }
}
