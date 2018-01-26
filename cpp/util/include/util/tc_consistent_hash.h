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

#ifndef __CONSISTENT_HASH__
#define __CONSISTENT_HASH__

#include "util/tc_md5.h"

using namespace std;

namespace tars
{

/////////////////////////////////////////////////
/**
 * @file tc_consistent_hash.h
 * @brief 一致性hash算法类.
 */
/////////////////////////////////////////////////

struct node_T
{
    /**
     *节点hash值
     */
    unsigned int iHashCode;

    /**
     *节点下标
     */
    unsigned int iIndex;
};



/**
 *  @brief 一致性hash算法类
 */
class  TC_ConsistentHash
{
    public:

        /**
         *  @brief 构造函数
         */
        TC_ConsistentHash()
        {
        }

        /**
         * @brief 节点比较.
         *
         * @param m1 node_T类型的对象，比较节点之一
         * @param m2 node_T类型的对象，比较节点之一
         * @return less or not 比较结果，less返回ture，否则返回false
         */
        static bool less_hash(const node_T & m1, const node_T & m2)
        {
            return m1.iHashCode < m2.iHashCode;
        }

        /**
         * @brief 增加节点.
         *
         * @param node  节点名称
         * @param index 节点的下标值
         * @return      节点的hash值
         */
        unsigned addNode(const string & node, unsigned int index)
        {
            node_T stItem;
            stItem.iHashCode = hash_md5(TC_MD5::md5bin(node));
            stItem.iIndex = index;
            vHashList.push_back(stItem);

            sort(vHashList.begin(), vHashList.end(), less_hash);

            return stItem.iHashCode;
        }

        /**
         * @brief 删除节点.
         *
         * @param node  节点名称
         * @return       0 : 删除成功  -1 : 没有对应节点
         */
        int removeNode(const string & node)
        {
            unsigned iHashCode = hash_md5(TC_MD5::md5bin(node));
            vector<node_T>::iterator it;
            for(it=vHashList.begin() ; it!=vHashList.end(); it++)
            {
                if(it->iHashCode == iHashCode)
                {
                    vHashList.erase(it);
                    return 0;
                }
            }
            return -1;
        }

        /**
         * @brief 获取某key对应到的节点node的下标.
         *
         * @param key      key名称
         * @param iIndex  对应到的节点下标
         * @return        0:获取成功   -1:没有被添加的节点
         */
        int getIndex(const string & key, unsigned int & iIndex)
        {
            unsigned iCode = hash_md5(TC_MD5::md5bin(key));
            if(vHashList.size() == 0)
            {
                iIndex = 0;
                return -1;
            }

            unsigned low = 0;
            unsigned high = vHashList.size();

            if(iCode <= vHashList[0].iHashCode || iCode > vHashList[high-1].iHashCode)
            {
                iIndex = vHashList[0].iIndex;
                return 0;
            }

            while (low < high - 1)
            {
                unsigned mid = (low + high) / 2;
                if (vHashList[mid].iHashCode > iCode)
                {
                    high = mid;
                }
                else
                {
                    low = mid;
                }
            }
            iIndex = vHashList[low+1].iIndex;
            return 0;
        }

   protected:
        /**
         * @brief 计算md5值的hash，分布范围在 0 -- 2^32-1.
         *
         * @param  sMd5 md5值
         * @return      hash值
         */
        unsigned int hash_md5(const string & sMd5)
        {
            char *p = (char *) sMd5.c_str();
            return (*(int*)(p)) ^ (*(int*)(p+4)) ^ (*(int*)(p+8)) ^ (*(int*)(p+12));
        }

        vector<node_T> vHashList;

};

}
#endif
