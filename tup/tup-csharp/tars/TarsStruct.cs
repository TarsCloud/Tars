/**
 * Tencent is pleased to support the open source community by making Tars available.
 *
 * Copyright (C) 2016THL A29 Limited, a Tencent company. All rights reserved.
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


using System.Text;

using Tup.Tars;

namespace Tup.Tars
{
    internal enum TarsStructType
    {
        BYTE = 0,
        SHORT = 1,
        INT = 2,
        LONG = 3,
        FLOAT = 4,
        DOUBLE = 5,
        STRING1 = 6,
        STRING4 = 7,
        MAP = 8,
        LIST = 9,
        STRUCT_BEGIN = 10,
        STRUCT_END = 11,
        ZERO_TAG = 12,
        SIMPLE_LIST = 13,
    }

    public abstract class TarsStruct
    {
        public static int TARS_MAX_STRING_LENGTH = 100 * 1024 * 1024;

        public abstract void WriteTo(TarsOutputStream _os);
        public abstract void ReadFrom(TarsInputStream _is);
        public virtual void Display(StringBuilder sb, int level)
        {
        }
   }
}
