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

#ifndef __TC_CONSISTENT_HASH_NEW_H_
#define __TC_CONSISTENT_HASH_NEW_H_

#include "util/tc_md5.h"
#include "util/tc_autoptr.h"
#include "util/tc_hash_fun.h"

using namespace tars;

namespace tars
{

struct node_T_new
{
    /**
     *节点hash值
     */
    long iHashCode;

    /**
     *节点下标
     */
    unsigned int iIndex;
};

enum TC_HashAlgorithmType
{
    E_TC_CONHASH_KETAMAHASH = 0,
    E_TC_CONHASH_DEFAULTHASH = 1
};

/**
 *  @brief hash 算法虚基类
 */
class TC_HashAlgorithm : public TC_HandleBase
{
public:
    virtual long hash(const string & sKey) = 0;
    virtual TC_HashAlgorithmType getHashType() = 0;

protected:
    long subTo32Bit(long hash)
    {
        return (hash & 0xFFFFFFFFL);
    }

};

typedef TC_AutoPtr<TC_HashAlgorithm> TC_HashAlgorithmPtr;

/**
 *  @brief ketama hash 算法
 */
class TC_KetamaHashAlg : public TC_HashAlgorithm
{
public:
    virtual long hash(const string & sKey)
    {
        string sMd5 = TC_MD5::md5bin(sKey);
        const char *p = (const char *) sMd5.c_str();

        long hash = ((long)(p[3] & 0xFF) << 24)
            | ((long)(p[2] & 0xFF) << 16)
            | ((long)(p[1] & 0xFF) << 8)
            | ((long)(p[0] & 0xFF));

        return subTo32Bit(hash);
    }

    virtual TC_HashAlgorithmType getHashType()
    {
        return E_TC_CONHASH_KETAMAHASH;
    }
};

/**
 *  @brief 默认的 hash 算法
 */
class TC_DefaultHashAlg : public TC_HashAlgorithm
{
public:
    virtual long hash(const string & sKey)
    {
        string sMd5 = TC_MD5::md5bin(sKey);
        const char *p = (const char *) sMd5.c_str();

        long hash = (*(int*)(p)) ^ (*(int*)(p+4)) ^ (*(int*)(p+8)) ^ (*(int*)(p+12));

        return subTo32Bit(hash);
    }

    virtual TC_HashAlgorithmType getHashType()
    {
        return E_TC_CONHASH_DEFAULTHASH;
    }
};

/**
 *  @brief hash alg 工厂
 */
class TC_HashAlgFactory
{
public:
    static TC_HashAlgorithm *getHashAlg()
    {
        TC_HashAlgorithm *ptrHashAlg = new TC_DefaultHashAlg();

        return ptrHashAlg;
    }

    static TC_HashAlgorithm *getHashAlg(TC_HashAlgorithmType hashType)
    {
        TC_HashAlgorithm *ptrHashAlg = NULL;

        switch(hashType)
        {
            case E_TC_CONHASH_KETAMAHASH:
            {
                ptrHashAlg = new TC_KetamaHashAlg();
                break;
            }
            case E_TC_CONHASH_DEFAULTHASH:
            default:
            {
                ptrHashAlg = new TC_DefaultHashAlg();
                break;
            }
        }

        return ptrHashAlg;
    }
};

/**
 *  @brief 一致性hash算法类
 */
class  TC_ConsistentHashNew
{
public:

    /**
     *  @brief 构造函数
     */
    TC_ConsistentHashNew()
    {
        _ptrHashAlg = TC_HashAlgFactory::getHashAlg();
    }

    /**
     *  @brief 构造函数
     */
    TC_ConsistentHashNew(TC_HashAlgorithmType hashType)
    {
        _ptrHashAlg = TC_HashAlgFactory::getHashAlg(hashType);
    }

    /**
     * @brief 节点比较.
     *
     * @param m1 node_T_new类型的对象，比较节点之一
     * @param m2 node_T_new类型的对象，比较节点之一
     * @return less or not 比较结果，less返回ture，否则返回false
     */
    static bool less_hash(const node_T_new & m1, const node_T_new & m2)
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
    int sortNode()
    {
        sort(_vHashList.begin(), _vHashList.end(), less_hash);

        return 0;
    }

    /**
     * @brief 打印节点信息
     *
     */
    void printNode()
    {
        map<unsigned int, unsigned int> mapNode;
        size_t size = _vHashList.size();

        for (size_t i = 0; i < size; i++)
        {
            if (i == 0)
            {
                unsigned int value = 0xFFFFFFFF - _vHashList[size - 1].iHashCode + _vHashList[0].iHashCode;
                mapNode[_vHashList[0].iIndex] = value;
            }
            else
            {
                unsigned int value = _vHashList[i].iHashCode - _vHashList[i - 1].iHashCode;

                if (mapNode.find(_vHashList[i].iIndex) != mapNode.end())
                {
                    value += mapNode[_vHashList[i].iIndex];
                }

                mapNode[_vHashList[i].iIndex] = value;
            }

            cout << "printNode: " << _vHashList[i].iHashCode << "|" << _vHashList[i].iIndex << "|" << mapNode[_vHashList[i].iIndex] << endl;
        }

        map<unsigned int, unsigned int>::iterator it = mapNode.begin();
        double avg = 100;
        double sum = 0;

        while (it != mapNode.end())
        {
            double tmp = it->second;
            cerr << "result: " << it->first << "|" << it->second << "|" << (tmp * 100 * mapNode.size() / 0xFFFFFFFF - avg) << endl;
            sum += (tmp * 100 * mapNode.size() / 0xFFFFFFFF - avg) * (tmp * 100 * mapNode.size() / 0xFFFFFFFF - avg);
            it++;
        }

        cerr << "variance: " << sum / mapNode.size() << ", size: " << _vHashList.size() << endl;
    }

    /**
     * @brief 增加节点.
     *
     * @param node  节点名称
     * @param index 节点的下标值
     * @param weight 节点的权重，默认为1
     * @return      是否成功
     */
    int addNode(const string & node, unsigned int index, int weight = 1)
    {
        if (_ptrHashAlg.get() == NULL)
        {
            return -1;
        }

        node_T_new stItem;
        stItem.iIndex = index;

        for (int j = 0; j < weight; j++)
        {
            string virtualNode = node + "_" + TC_Common::tostr<int>(j);

            // TODO: 目前写了2 种hash 算法，可以根据需要选择一种，
            // TODO: 其中KEMATA 为参考memcached client 的hash 算法，default 为原有的hash 算法，测试结论在表格里有
            if (_ptrHashAlg->getHashType() == E_TC_CONHASH_KETAMAHASH)
            {
                string sMd5 = TC_MD5::md5bin(virtualNode);
                char *p = (char *) sMd5.c_str();

                for (int i = 0; i < 4; i++)
                {
                    stItem.iHashCode = ((long)(p[i * 4 + 3] & 0xFF) << 24)
                        | ((long)(p[i * 4 + 2] & 0xFF) << 16)
                        | ((long)(p[i * 4 + 1] & 0xFF) << 8)
                        | ((long)(p[i * 4 + 0] & 0xFF));
                    stItem.iIndex = index;
                    _vHashList.push_back(stItem);
                }
            }
            else
            {
                stItem.iHashCode = _ptrHashAlg->hash(virtualNode);
                _vHashList.push_back(stItem);
            }
        }

        return 0;
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
        if(_ptrHashAlg.get() == NULL || _vHashList.size() == 0)
        {
            iIndex = 0;
            return -1;
        }

        long iCode = _ptrHashAlg->hash(TC_MD5::md5bin(key));

        return getIndex(iCode, iIndex);
    }

    /**
     * @brief 获取某hashcode对应到的节点node的下标.
     *
     * @param hashcode      hashcode
     * @param iIndex  对应到的节点下标
     * @return        0:获取成功   -1:没有被添加的节点
     */
    int getIndex(long hashcode, unsigned int & iIndex)
    {
        if(_ptrHashAlg.get() == NULL || _vHashList.size() == 0)
        {
            iIndex = 0;
            return -1;
        }

        // 只保留32位
        long iCode = (hashcode & 0xFFFFFFFFL);

        int low = 0;
        int high = _vHashList.size();

        if(iCode <= _vHashList[0].iHashCode || iCode > _vHashList[high-1].iHashCode)
        {
            iIndex = _vHashList[0].iIndex;
            return 0;
        }

        while (low < high - 1)
        {
            int mid = (low + high) / 2;
            if (_vHashList[mid].iHashCode > iCode)
            {
                high = mid;
            }
            else
            {
                low = mid;
            }
        }
        iIndex = _vHashList[low+1].iIndex;
        return 0;
    }

    /**
     * @brief 获取当前hash列表的长度.
     *
     * @return        长度值
     */
    size_t size()
    {
        return _vHashList.size();
    }

    /**
     * @brief 清空当前的hash列表.
     *
     */
    void clear()
    {
        _vHashList.clear();
    }

protected:
    vector<node_T_new>    _vHashList;
    TC_HashAlgorithmPtr _ptrHashAlg;

};

}
#endif
