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
#ifndef __TARS_ANALYZER_H_
#define __TARS_ANALYZER_H_

#include "tarsparse/parse.h"
#include "util/tc_md5.h"
#include "util/tc_file.h"
#include "util/tc_common.h"
#include <arpa/inet.h>
#include "tup/Tars.h"

using namespace tars;

#define chars_one_line 16
#define chars_buf_reserve 1


#define CHAR_WIDTH_TAG 2
#define CHAR_WIDTH_ID  12
#define CHAR_WIDTH_TYPE 23

#ifndef TARS_MAX_STRING_LENGTH
#define TARS_MAX_STRING_LENGTH   (100 * 1024 * 1024)
#endif

class TarsAnalyzer
{
public:
    TarsAnalyzer()
    {
        _startPos = 0;
        _itab = 0;
        _buf_len = 0;
        _buf = NULL;
        _bTup = false;
        _bIsDumpTup = false;
        _tupVersion = 2;
        _tup2_helper.clear();
        _tup3_helper.clear();
    }
    /**
     * 设置待解析的结构体名
     * @param name
     */
    void setStructName(const string &name)
    {
        _StructName = name;
    }
    /**
     *tup打包时put的属性名,eg.UniPacket<> pack; pack.put<xx>(name,xxx);
     *@param name
     */
    void setPutName(const string &name)
    {
        _putName = name;
    }
    /**
     * 设置待解析的数据流的文件名
     *@param sFile
     */
    void setStreamFileName(const string &sFile)
    {
        _streamFileName = sFile;
    }

    void setStartPos(int p)
    {
        _startPos = p;
    }

    bool isTup()
    {
        return _bTup;
    }
    void setTup(bool b)
    {
        _bTup = b;
    }

    enum
    {
        eOK = 0,
        eNeedBackDataHead = 1,
        eNeedSkipTag = 2
    };

protected:
    /**
     * 根据命名空间获取文件路径
     * @param ns 命名空间
     *
     * @return string
     */
    string getFilePath(const string &ns) const
    {
        return  "/" + tars::TC_Common::replace(_packagePrefix, ".", "/") + "/" + ns + "/";
    }

    /**
     * 生成某类型的字符串描述源码
     * @param pPtr
     *
     * @return string
     */
    string tostr(const TypePtr &pPtr) ;

    /**
     * 生成内建类型的字符串描述源码,eg,int,short,float,double
     * @param pPtr
     *
     * @return string
     */
    string tostrBuiltin(const BuiltinPtr &pPtr) ;
    /**
     * 生成vector的字符串描述
     * @param pPtr
     *
     * @return string
     */
    string tostrVector(const VectorPtr &pPtr) ;

    /**
     * 生成map的字符串描述
     * @param pPtr
     *
     * @return string
     */
    string tostrMap(const MapPtr &pPtr, bool bNew = false) ;

    /**
     * 生成某种结构的符串描述
     * @param pPtr
     *
     * @return string
     */
    string tostrStruct(const StructPtr &pPtr) const;
    string tostrEnum(const EnumPtr &pPtr) ;

protected:
    /**
     * 从编码buffer中读取struct类型
     * @param pPtr
     *
     * @return string
     */

    int readStruct(const StructPtr & pPtr, const NamespacePtr & nPtr) ;

    /**
     * 解析名字空间中相对应的struct
     * @param pPtr
     *
     * @return string
     */
    void analyzeNS(const NamespacePtr &pPtr)
    {
        vector<StructPtr>& vStruct = pPtr->getAllStructPtr();
        size_t i;
        for ( i= 0; i < vStruct.size(); i++)
        {
            if (vStruct[i]->getId() == _StructName || _bTup)
            {
                readStruct(vStruct[i], pPtr);
                break;
            }
        }
        if (i>= vStruct.size())
        {
            cout<< "not find  structName: " << _StructName << endl;
        }
    }

    /**
     * 生成每个tars文件的java文件源码
     * @param pPtr
     *
     * @return string
     */
    void analyzeContext(const ContextPtr &pPtr)
    {
        vector<NamespacePtr> namespaces = pPtr->getNamespaces();

        for(size_t i = 0; i < namespaces.size(); i++)
        {
            analyzeNS(namespaces[i]);
        }
    }

public:
    
    void skip(size_t len);
    void skipToStructEnd();
    
void skipField();


/// 跳过一个字段，不包含头信息
void skipField(uint8_t type);
    void backToLast();
    void peekBuf(void * buf, size_t len, size_t offset = 0);
    void readBuf(void * buf, size_t len);
    template<typename T>
    T readByType();

    void read(double& n, uint8_t expecttag, int& realType, bool isRequire);

    void read(Int32& n, uint8_t& tag, int& type, bool isRequire = true);

    void read(Int64& n, uint8_t& tag, int& type, bool isRequire = true);

    void read(string& s, uint8_t&    tag, int& type, bool isRequire = true );
    
    template<typename K, typename V, typename Cmp, typename Alloc>
    void read(std::map<K, V, Cmp, Alloc>& m, uint8_t tag, bool isRequire = true);


    
    template<typename Alloc>
    void read(std::vector<char, Alloc>& v,  uint8_t&  tag, int & realType, bool isRequire = true);

    int readBuiltin(const BuiltinPtr &pPtr, int tag, bool isRequire,  TypeIdPtr pType) ;
    
    int readType(const TypePtr &pPtr, int tag, bool isRequire);
    
    int readVector(const VectorPtr &pPtr, int tag,  bool isRequire, TypeIdPtr pType);

    int readMap(MapPtr pType, int tag, bool isRequire);
    int readEnum(EnumPtr pPtr, int expectTag, bool isRequire);
    void analyzeFile(const string &file, string streamFile);

    /**
     *解析tup数据包中的sBuffer数据
     */
    void analyzeWithTupBuffer(const string &file, const string& putName);
    void checkTupBuffer(void);
    int compareTag(int tag, int expectTag, bool bIsRequire = true);

    string getTab();
    string getSep();
    /**
     * 增加tab数
     */
    void incTab() ;

    /**
     * 减少tab数
     */
    void decTab() ;

    void  addTab(int count) ;
    void eraseTab(int count) ;
    void setBuffer(const char * buf, size_t len)
    {
        _buf = buf;
        _buf_len = len +chars_buf_reserve; //+1的目的是兼容没有结构结束标志字节
        _cur = 0;
        _last_pos =0;
    }
    /**
     * 输出tup包的内容
     */
    void setDumpTup(bool isDump){_bIsDumpTup = isDump;}
    void dumpTup(const map<string, map<string, vector<char> > >& tupData);
    void dumpTup(const map<string, vector<char> >& tupData);

private:
    string _packagePrefix;
    string _StructName;
    string _putName;  // tup put 接口传人的name
    string _streamFileName;

    //起始分析位置
    int    _startPos;
    int _itab;

    const char *        _buf;        ///< 缓冲区
    size_t                _buf_len;    ///< 缓冲区长度

    size_t                _cur;        ///< 当前位置

    size_t                _last_pos;        ///< 当前位置
    size_t              _tup_sbuffer_pos;

    //是否在解析tup数据包
    bool _bTup;
    bool _bIsDumpTup;

    //tup的版本号，2或者3
    short _tupVersion;

    //tup 版本2对应的数据类型
    map<string, map<string, vector<char> > > _tup2_helper;

    //tup 版本3对应的数据类型
    map<string, vector<char> >  _tup3_helper;

};

class AnalyzerDataHead
{

public:
    uint8_t _type;
    uint8_t _tag;
    enum
    {
        eChar = 0,
        eShort = 1,
        eInt32 = 2,
        eInt64 = 3,
        eFloat = 4,
        eDouble = 5,
        eString1 = 6,
        eString4 = 7,
        eMap = 8,
        eList = 9,
        eStructBegin = 10,
        eStructEnd = 11,
        eZeroTag = 12,
        eSimpleList = 13,
    };

    static string tostrType(int type)
    {
        string s;
        
        switch(type)
        {
        case eChar:
            s =  " char ";
            break;
        case eShort:
            s =  " short ";
            break;
        case eInt32:
            s =  " int ";
            break;
        case eInt64:
            s =  " int64 ";
            break;
        case eFloat:
            s =  " float ";
            break;
        case eDouble:
            s =  " double ";
            break;
        case eString1:
            s =  " string1 ";
            break;
        case eString4:
            s =  " string4 ";
            break;
        case eMap:
            s =  " map ";
            break;
        case eList:
            s =  " vector ";
            break;
        case eStructBegin:
            s =  " struct beg ";
            break;
        case eStructEnd:
            s =  " struct end ";
            break;
        case eZeroTag:
            s =  " zero ";
            break;
        case eSimpleList:
            s =  " vector byte ";
            break;
            
        default:
            cout << "ERROR, tostrType failed!" << type << endl;
            exit(-1);
            break;

        }
        return s;

    }

    struct helper
    {
        unsigned int    type : 4;
        unsigned int    tag : 4;
    }__attribute__((packed));

public:
    AnalyzerDataHead() : _type(0), _tag(0) {}
    AnalyzerDataHead(uint8_t type, uint8_t tag) : _type(type), _tag(tag) {}

    uint8_t getTag() const        { return _tag;}
    void setTag(uint8_t t)        { _tag = t;}
    uint8_t getType() const     { return _type;}
    void setType(uint8_t t)     { _type = t;}

    /// 读取数据头信息
    void readFrom(TarsAnalyzer& is)
    {
        size_t n = peekFrom(is);
        is.skip(n);
    }

    /// 读取头信息，但不前移流的偏移量
    size_t peekFrom(TarsAnalyzer& is)
    {
        helper h;
        size_t n = sizeof(h);
        is.peekBuf(&h, sizeof(h));
        _type = h.type;
        if (h.tag == 15)
        {
            is.peekBuf(&_tag, sizeof(_tag), sizeof(h));
            n += sizeof(_tag);
        }
        else
        {
            _tag = h.tag;
        }
        return n;
    }



};

#endif

