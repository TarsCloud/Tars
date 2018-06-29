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

#ifndef __LOCKFREEQUEUE_H__
#define __LOCKFREEQUEUE_H__

#include <stdint.h>

template<class item>
class LockFreeQueue
{
public:
    enum
    {
        RT_OK                   = 0,    //成功
        RT_FULL                 = 1,    //满了
        RT_EMPTY                = 2,   //没数据
        RT_ERR                  = -1,   //出错
    };
	
	enum
    {
        FLAG_NULL               = 0,
        FLAG_SETTOOK            = 1,
        FLAG_SETED              = 2,
		FLAG_GETTOOK			= 3,
    };

    LockFreeQueue():_bInit(false), _dataCount(0)
    {
    }

    /**
	 * @brief 初始化 
	 *  
     * @param size：元素个数 
     *  
     */
    int init(const uint32_t size)
    {
        if (_bInit) 
            return RT_OK;

        uint32_t totalSize = sizeof(Head) + (size + 1) * sizeof(Entry);

        _head = (Head*)new char[totalSize];
        memset(_head, 0 , totalSize);

        _head->head = _head->tail = 0;

        _head->size = size;

        _bInit = true;

        return RT_OK;
    }

    /**
	 * @brief 插入数据
     * 
     * @param val：value数据 
     * @param bBlock：是否阻塞  
     *  
     */
    int enqueue(const item& val, bool bBlock)
    {
        if (!_bInit) 
            return RT_ERR;

        bool bsuccess = false;

        uint64_t old_head, old_tail;
        do 
        {
    		old_head = _head->head;
			old_tail = _head->tail;
            __sync_synchronize();

			if(old_head >= old_tail)
			{				
				if (_head->size > (old_head - old_tail)) 
				{
					bsuccess = __sync_bool_compare_and_swap(&_head->head,
								old_head, old_head + 1);
				}
				else
				{				
					if (bBlock)
						continue;
					else
						return RT_FULL;
				}
			}
    	} while (false == bsuccess);
		
		uint64_t pos = old_head % _head->size;

		do
		{
			bsuccess = __sync_bool_compare_and_swap(&(((Entry*)&(_head[1]))[pos].flag), FLAG_NULL, FLAG_SETTOOK);
		}while(false == bsuccess);
		
        ((Entry*)&(_head[1]))[pos].data = val;
		
		((Entry*)&(_head[1]))[pos].flag = FLAG_SETED;

        __sync_synchronize();

        return RT_OK;
    }

    /**
	 * @brief 取数据
     *  
     * @param val：value数据 
     * @param bBlock：是否阻塞  
     *  
     */
    int dequeue(item& val, bool bBlock)
    {
        if (!_bInit) 
            return RT_ERR;

        bool bsuccess = false;

        uint64_t old_head, old_tail;
        do 
        {
			old_head = _head->head;
    		old_tail = _head->tail;
            __sync_synchronize();

            if (old_tail < old_head) 
            {
				bsuccess = __sync_bool_compare_and_swap(&_head->tail,
							old_tail, old_tail + 1);
            }
			else
			{
                if (bBlock)
                    continue;
                else
                    return RT_EMPTY;				
			}
    	} while (false == bsuccess);
		
		uint64_t pos = old_tail % _head->size;
		
		do
		{
			bsuccess = __sync_bool_compare_and_swap(&(((Entry*)&(_head[1]))[pos].flag), FLAG_SETED, FLAG_GETTOOK);
		}while(false == bsuccess);
		
		val = ((Entry*)&(_head[1]))[pos].data;
		
		((Entry*)&(_head[1]))[pos].flag = FLAG_NULL;

        __sync_synchronize();
        return RT_OK;
    }

    unsigned int size() const
    {
		uint64_t old_head, old_tail;
		old_head = _head->head;
    	old_tail = _head->tail;
		if(old_head > old_tail)
			return old_head - old_tail;
		else
			return 0;
    }

private:
	
	struct Entry
	{
		volatile uint8_t flag;
		item data;
	};

    struct Head
    {
        volatile uint64_t head;

        volatile uint64_t tail;

        uint32_t size;
    } *_head;
	
    //是否已初始化
    bool _bInit;

    volatile unsigned int _dataCount;
	
};



#endif
