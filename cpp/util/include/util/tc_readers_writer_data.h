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

#ifndef __TC_READERS_WRITER_DATA_H__
#define __TC_READERS_WRITER_DATA_H__


namespace tars
{

/////////////////////////////////////////////////
/** 
 * @file tc_reader_writer_data.h 
 * @brief 双buff类，多读一写数据的封装
 * 常用于数据量比较小的字典数据、配置数据.
 */       
/////////////////////////////////////////////////
template<typename T>
class TC_ReadersWriterData
{
public:
    TC_ReadersWriterData() 
    : reader_using_inst0_(true)
    {
    }

    virtual ~TC_ReadersWriterData() 
    {
    }

    //获取读取的数据
    virtual T& getReaderData() 
    {
        if(reader_using_inst0_) 
        {
            return inst0_;
        }
        else 
        {
            return inst1_;
        }
    }

    //获取可写的数据
    virtual T& getWriterData() 
    {
        if(reader_using_inst0_) 
        {
            return inst1_;
        }
        else 
        {
            return inst0_;
        }
    }

    //写完数据以后，将读写数据交换
    virtual void swap() 
    {
        reader_using_inst0_ = !reader_using_inst0_;
    }

private:
    T inst0_;
    T inst1_;
    bool reader_using_inst0_;
};

}
#endif
