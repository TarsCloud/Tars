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

namespace Tup
{
    internal class Const
    {
        public static string STATUS_GRID_KEY = "STATUS_GRID_KEY";
        public static string STATUS_DYED_KEY = "STATUS_DYED_KEY";
        public static string STATUS_GRID_CODE = "STATUS_GRID_CODE";
        public static string STATUS_SAMPLE_KEY = "STATUS_SAMPLE_KEY";
        public static string STATUS_RESULT_CODE = "STATUS_RESULT_CODE";
        public static string STATUS_RESULT_DESC = "STATUS_RESULT_DESC";

        public static int INVALID_HASH_CODE = -1;
        public static int INVALID_GRID_CODE = -1;

        public static byte PACKET_TYPE_TARSNORMAL = 0;
        public static byte PACKET_TYPE_TARSONEWAY = 1;
        public static byte PACKET_TYPE_TUP = 2;
        public static byte PACKET_TYPE_TUP3 = 3;
    }
}
