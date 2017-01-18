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
#include "tarsAnalyzer.h"
#include <string.h>

using namespace  std;


/// 读取一个指定类型的数据（基本类型）
template<typename T>
T TarsAnalyzer::readByType()
{
    T n;
    readBuf(&n, sizeof(n));
    return n;
}

void TarsAnalyzer::analyzeWithTupBuffer(const string &file, const string& putName)
{
    string stName;
    if(_tupVersion == 2)
    {
        map<string, map<string, vector<char> > >::iterator it1 = _tup2_helper.find(_putName);
        if (it1 != _tup2_helper.end())
        {
            cout << "put    name:" << it1->first << endl;
            map<string, vector<char> >::iterator it2 = it1->second.begin();
            while(it2 != it1->second.end())
            {
                //去掉名字空间
                string::size_type pos = 0;
                pos = it2->first.find(".");
                if (pos != string::npos)
                {
                    stName = it2->first.substr(pos + 1);
                }
                else
                {
                    stName = it2->first;
                }
                _StructName = stName; //设定需要分析的结构名
                cout << "struct name:" << stName << endl;

                
                vector<char> v = it2->second;

                //分析tup包中的sBuffer
                setBuffer(&v[0], v.size()) ;
                std::vector<ContextPtr> contexts = g_parse->getContexts();
                for(size_t i = 0; i < contexts.size(); i++)
                {
                    if(file == contexts[i]->getFileName())
                    {
                        analyzeContext(contexts[i]);
                    }
                }
                it2++;
            }
        }
        else
        {
            cout << "not find tup put name:" << _putName<<endl;
        }
    }
    else if(_tupVersion == 3)//精简版没有structname,how to do ?
    {
        map<string, vector<char> >::const_iterator it = _tup3_helper.find(_putName);
        if( it != _tup3_helper.end())
        {
            cout << "put    name:" << it->first << endl;
            cout << "struct name:" << _StructName << endl;
            vector<char> v = it->second;

            //分析tup包中的sBuffer
            setBuffer(&v[0], v.size()) ;
            std::vector<ContextPtr> contexts = g_parse->getContexts();
            for(size_t i = 0; i < contexts.size(); i++)
            {
                if(file == contexts[i]->getFileName())
                {
                    analyzeContext(contexts[i]);
                }
            }

        }
        else
        {
            cout << "not find tup put name:" << _putName<<endl;
        }
    }


}




void TarsAnalyzer::analyzeFile(const string &file, string streamFile)
{
    string content = tars::TC_File::load2str(streamFile);
    cout << "analyzeFile:" << tars::TC_File::extractFileName(streamFile) << "'s buffer size:" <<  content.length() << endl;
    setBuffer(content.c_str(), content.length());

    if (_startPos > 0)
    {
        _cur = _startPos;
        _last_pos = _startPos;

    }
    if (_bTup)
    {
        checkTupBuffer();
    }
    std::vector<ContextPtr> contexts = g_parse->getContexts();
    for(size_t i = 0; i < contexts.size(); i++)
    {
        if(file == contexts[i]->getFileName())
        {
            analyzeContext(contexts[i]);
        }
    }
}

void TarsAnalyzer::checkTupBuffer(void)
{
    if(_buf_len < sizeof(int))
    {
        cout << "ERROR, tup buffer too short!" << endl;
        exit(-1);
    }
    uint32_t iHeaderLen;

    if (_buf)
    {
        iHeaderLen = ntohl(*(uint32_t*)(_buf));
    }
    else
    {
        cout << "ERROR, tup buffer not set!" << endl;
        exit(-1);
    }

    //长度大于10M
    if (iHeaderLen > 10000000)
    {
        cout << "packet length too long,len:" + tars::TC_Common::tostr(iHeaderLen) << endl;
        exit(-1);
    }

    if (iHeaderLen != (_buf_len - chars_buf_reserve))
    {
        cout << "ERROR, buffer size: " << _buf_len - chars_buf_reserve <<   " not eq header len:" << iHeaderLen << endl;
        exit(-1);
    }

    cout << "check tup buffer ok!" << endl;


}

void TarsAnalyzer::peekBuf(void * buf, size_t len, size_t offset)
{
    if (_cur + offset + len > _buf_len)
    {
        
        cout<< "ERROR, buffer overflow when peekBuf, buf_len:" << _buf_len << " cur:" << _cur << " len:"<< len <<endl;
        exit(-1);
    }
    ::memcpy(buf, _buf + _cur + offset, len);

}

/// 读取缓存
void TarsAnalyzer::readBuf(void * buf, size_t len)
{
    peekBuf(buf, len);
    _last_pos = _cur;
    _cur += len;
}


/// 跳过len个字节
void TarsAnalyzer::skip(size_t len)
{
    _last_pos = _cur;
    _cur += len;
}


/// 跳到当前结构的结束
void TarsAnalyzer::skipToStructEnd()
{
    AnalyzerDataHead h;
    do
    {
        h.readFrom(*this);
        skipField(h.getType());
    }while (h.getType() != AnalyzerDataHead::eStructEnd);
}

/// 跳过一个字段
void TarsAnalyzer::skipField()
{
    AnalyzerDataHead h;
    h.readFrom(*this);
    skipField(h.getType());
}

/// 跳过一个字段，不包含头信息
void TarsAnalyzer::skipField(uint8_t type)
{
    unsigned char tag = 0;
    int realType;
    switch (type)
    {
    case AnalyzerDataHead::eChar:
        this->skip(sizeof(char));
        break;
    case AnalyzerDataHead::eShort:
        this->skip(sizeof(short));
        break;
    case AnalyzerDataHead::eInt32:
        this->skip(sizeof(int));
        break;
    case AnalyzerDataHead::eInt64:
        this->skip(sizeof(long long));
        break;
    case AnalyzerDataHead::eFloat:
        this->skip(sizeof(float));
        break;
    case AnalyzerDataHead::eDouble:
        this->skip(sizeof(double));
        break;
    case AnalyzerDataHead::eString1:
        {
            size_t len = readByType<uint8_t>();
            this->skip(len);
        }
        break;
    case AnalyzerDataHead::eString4:
        {
            size_t len = ntohl(readByType<uint32_t>());
            this->skip(len);
        }
        break;
    case AnalyzerDataHead::eMap:
        {
            Int32 size;
            read(size, tag, realType);
            for (int i = 0; i < size * 2; ++i)
                skipField();
        }
        break;
    case AnalyzerDataHead::eList:
        {
            Int32 size;
            read(size, tag, realType);
            for (int i = 0; i < size; ++i)
                skipField();
        }
        break;
    case AnalyzerDataHead::eSimpleList:
        {
            AnalyzerDataHead h;
            h.readFrom(*this);
            if (h.getType() != AnalyzerDataHead::eChar)
            {

                cout << "ERROR, skipField with invalid type, type value: " << (int)type <<", " << h.tostrType(h.getType()) << endl;

                cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos ;
                exit(-1);
            }
            Int32 size;
            read(size, tag, realType);
            this->skip(size);
        }
        break;
    case AnalyzerDataHead::eStructBegin:
        skipToStructEnd();
        break;
    case AnalyzerDataHead::eStructEnd:
    case AnalyzerDataHead::eZeroTag:
        break;
    default:
        {
            cout << "ERROR, skipField with invalid type, type value:" << (int)type << endl;
            cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos ;
            exit(-1);
        }
    }
}


void TarsAnalyzer::backToLast()
{
    _cur = _last_pos;
}


int TarsAnalyzer::readType(const TypePtr &pPtr, int expectTag, bool isRequire)
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
    if (bPtr) return readBuiltin(bPtr, expectTag, isRequire, NULL);

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
    if (vPtr) return readVector(vPtr, expectTag,isRequire, NULL);

    MapPtr mPtr = MapPtr::dynamicCast(pPtr);
    if(mPtr) return readMap(mPtr, expectTag, isRequire);

    StructPtr sPtr = StructPtr::dynamicCast(pPtr);
    if (sPtr) return readStruct(sPtr,  NULL);

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
    if (ePtr) return 0;

    if (!pPtr) return 0;
    cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos ;
    exit(-1);
    return 0;
}

/*******************************BuiltinPtr********************************/
string TarsAnalyzer::tostrBuiltin(const BuiltinPtr &pPtr)
{
    string s;

    switch(pPtr->kind())
    {
    case Builtin::KindBool:
        s = " bool ";
        break;
    case Builtin::KindByte:
        s = " char ";
        break;
    case Builtin::KindShort:
    //为了兼容java无unsigned, 编结码时把tars问件中 unsigned char 对应到short
    //c++中需要还原回来
        s = (pPtr->isUnsigned()?"uchar":" short ");
        break;
    case Builtin::KindInt:
        s = (pPtr->isUnsigned()?"ushort":" int ");
        break;
    case Builtin::KindLong:
        s =  (pPtr->isUnsigned()?"uint":" int64 ");
        break;
    case Builtin::KindFloat:
        s = " float ";
        break;
    case Builtin::KindDouble:
        s = " double ";
        break;
    case Builtin::KindString:
        if(pPtr->isArray())
            s = "char[]"; //char a [8];
        else
            s = " string ";//string a;
        break;
    case Builtin::KindVector:
        s = " vector ";
        break;
    case Builtin::KindMap:
        s = " map ";
        break;
    default:
        cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos << endl;
        exit(-1);
        break;
    }

    return s;
}
string TarsAnalyzer::tostr(const TypePtr &pPtr)
{
    BuiltinPtr bPtr = BuiltinPtr::dynamicCast(pPtr);
    if(bPtr) return tostrBuiltin(bPtr);

    VectorPtr vPtr = VectorPtr::dynamicCast(pPtr);
    if(vPtr) return tostrVector(vPtr);

    MapPtr mPtr = MapPtr::dynamicCast(pPtr);
    if(mPtr) return tostrMap(mPtr);

    StructPtr sPtr = StructPtr::dynamicCast(pPtr);
    if(sPtr) return tostrStruct(sPtr);

    EnumPtr ePtr = EnumPtr::dynamicCast(pPtr);
    //if(ePtr) return tostrEnum(ePtr);

    if(!pPtr) return "void";
    cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos ;
    exit(-1);
    return "";
}

string TarsAnalyzer::tostrMap(const MapPtr &pPtr, bool bNew)
{
    string s = Builtin::builtinTable[Builtin::KindMap] + string("<") + tostr(pPtr->getLeftTypePtr()) + ", " + tostr(pPtr->getRightTypePtr());
    if(MapPtr::dynamicCast(pPtr->getRightTypePtr()) || VectorPtr::dynamicCast(pPtr->getRightTypePtr()))
    {
        s += " >";
    }
    else
    {
        s += ">";
    }
    return s;
}


string TarsAnalyzer::getTab()
{
    ostringstream s;
    for(int i = 0; i < _itab; i++)
    {
        s << "    ";
    }

    return s.str();
}
string TarsAnalyzer::getSep()
{
    ostringstream s;
    for(int i = 0; i < CHAR_WIDTH_ID; i++)
    {
        s << "-";
    }

    return s.str();
}

/**
 * 增加tab数
 */
void TarsAnalyzer::incTab()
{
    _itab++;
}

void TarsAnalyzer::addTab(int count)
{
    _itab += count;
}

void TarsAnalyzer::eraseTab(int count)
{
    _itab -= count;
}

/**
 * 减少tab数
 */
void TarsAnalyzer::decTab()
{
    _itab--;
}

void TarsAnalyzer::read(double& n, uint8_t expecttag, int & realType, bool isRequire)
{
    AnalyzerDataHead h;
    h.readFrom(*this);

    int ret = compareTag(h.getTag(), expecttag, isRequire);
    expecttag = h.getTag();
    realType = h.getType();
    n = 0.0f;
    if (ret != eOK)
    {
        return;
    }

    switch (h.getType())
    {
    case AnalyzerDataHead::eZeroTag:
        n = 0;
        break;
    case AnalyzerDataHead::eFloat:
        n = readByType<float>();
        n = tars_ntohf(n);
        break;
    case AnalyzerDataHead::eDouble:
        this->readBuf(&n, sizeof(n));
        n = tars_ntohd(n);
        break;
    default:
        {
            cout << "ERROR, read string type mismatch, tag: " << (int) expecttag << ", get type: " <<  (int)h.getType() << endl;
            cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos << endl;;
            exit(-1);
        }

    }
}

void TarsAnalyzer::read(string& s, uint8_t&  expecttag, int& realType, bool isRequire )
{
    AnalyzerDataHead h;
    h.readFrom(*this);

    int ret = compareTag(h.getTag(), expecttag, isRequire);

    expecttag = h.getTag();
    realType = h.getType();
    s = "";

    if (ret != eOK)
    {
        return;
    }
    switch (h.getType())
    {
    case AnalyzerDataHead::eString1:
        {
            size_t len = readByType<uint8_t>();
            char ss[256];
            this->readBuf(ss, len);
            s.assign(ss, ss + len);
        }
        break;
    case AnalyzerDataHead::eString4:
        {
            //读取string4的长度
            uint32_t len = ntohl(readByType<uint32_t>());
            if (len > TARS_MAX_STRING_LENGTH)
            {

            }
            char *ss = new char[len];
            try
            {
                this->readBuf(ss, len);
                s.assign(ss, ss + len);
            }
            catch (...)
            {
                delete[] ss;
                throw;
            }
            delete[] ss;
        }
        break;
    default:
        {
            cout << "ERROR, read string type mismatch, tag: " << (int) expecttag << ", get type: " <<  (int)h.getType() << endl;
            cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos << endl;;
            exit(-1);
        }
    }
}
void TarsAnalyzer::read(Int32& n, uint8_t&  tag, int & realType, bool isRequire)
{
    AnalyzerDataHead h;
    h.readFrom(*this);

    int ret = compareTag(h.getTag(), tag, isRequire);
    tag = h.getTag();
    realType = h.getType();
    n = 0;
    if (ret != eOK)
    {
        return;
    }
    switch (h.getType())
    {
    case AnalyzerDataHead::eZeroTag:
        n = 0;
        break;
    case AnalyzerDataHead::eChar:
        n = readByType<char>();
        break;
    case AnalyzerDataHead::eShort:
        n = (short) ntohs(readByType<short>());
        break;
    case AnalyzerDataHead::eInt32:
        this->readBuf(&n, sizeof(n) );
        n = ntohl((Int32)n);
        break;
    default:
        {
            cout<< "ERROR, read numeric type mismatch, buf tag: " << (int)tag << ", get type: " << h.tostrType(h.getType()) << endl;
            cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos << endl;;
            exit(-1);
        }
    }

}


void TarsAnalyzer::read(Int64& n, uint8_t&  tag, int & realType, bool isRequire)
{
    AnalyzerDataHead h;
    h.readFrom(*this);

    int ret = compareTag(h.getTag(), tag, isRequire);
    tag = h.getTag();
    realType = h.getType();
    n = 0;
    if (ret != eOK)
    {
        return;
    }
    switch (h.getType())
    {
    case AnalyzerDataHead::eZeroTag:
        n = 0;
        break;
    case AnalyzerDataHead::eChar:
        n = readByType<char>();
        break;
    case AnalyzerDataHead::eShort:
        n = (short) ntohs(readByType<short>());
        break;
    case AnalyzerDataHead::eInt32:
        n = (Int32) ntohl(readByType<Int32>());
        break;
    case AnalyzerDataHead::eInt64:
        this->readBuf(&n, sizeof(n) );
        n = tars_ntohll((Int64)n);
        break;
    default:
        {
            cout<< "ERROR, read numeric type mismatch, buf tag: " << (int)tag << ", get type: " << h.tostrType(h.getType()) << endl;
            cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos << endl;;
            exit(-1);
        }
    }

}


template<typename K, typename V, typename Cmp, typename Alloc>
void TarsAnalyzer::read(std::map<K, V, Cmp, Alloc>& m, uint8_t tag, bool isRequire)
{
    uint8_t utag = 0;
    int realtype;

    AnalyzerDataHead h;
    h.readFrom(*this);
    switch (h.getType())
    {
    case AnalyzerDataHead::eMap:
        {
            int size;
            read(size, utag, realtype);
            if (size < 0)
            {
                char s[128];
                snprintf(s, sizeof(s), "invalid map, tag: %d, size: %d", tag, size);
                cout << s << endl;
            }
            m.clear();

            for (int i = 0; i < size; ++i)
            {
                std::pair<K, V> pr;
                utag = 0;
                read(pr.first, utag, realtype);
                utag = 1;
                read(pr.second, utag, realtype);
                m.insert(pr);
            }
        }
        break;
    default:
        {
            char s[64];
            snprintf(s, sizeof(s), "read 'map' type mismatch, tag: %d, get type: %d.", tag, h.getType());
            cout << s << endl;
            cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos ;
        }
    }

}

template<typename Alloc>
void TarsAnalyzer::read(std::vector<char, Alloc>& v,  uint8_t&  tag, int & realType, bool isRequire )
{
    //if (skipToTag(tag))
    {
        uint8_t t=0;
        int realType;
        AnalyzerDataHead h;
        h.readFrom(*this);
        switch (h.getType())
        {
        case AnalyzerDataHead::eSimpleList:
            {
                AnalyzerDataHead hh;
                hh.readFrom(*this);
                if (hh.getType() != AnalyzerDataHead::eChar)
                {
                    char s[128];
                    snprintf(s, sizeof(s), "type mismatch, tag: %d, type: %d, %d", tag, h.getType(), hh.getType());
                    //throw TarsDecodeMismatch(s);
                }
                int size;
                read(size, t, realType);
                if (size < 0)
                {
                    char s[128];
                    snprintf(s, sizeof(s), "invalid size, tag: %d, type: %d, %d, size: %d", tag, h.getType(), hh.getType(), size);
                    //throw TarsDecodeInvalidValue(s);
                }
                v.resize(size);
                readBuf(&v[0], size);
            }
            break;

    
        default:
            {
                char s[128];
                snprintf(s, sizeof(s), "type mismatch, tag: %d, type: %d", tag, h.getType());
                //throw TarsDecodeMismatch(s);
            }
        }
    }


}

int TarsAnalyzer::readVector(const VectorPtr &pPtr, int expectTag, bool isRequire, TypeIdPtr pType)
{
    {
        int type;
        AnalyzerDataHead h;
        h.readFrom(*this);
        
        uint8_t t=0;
        uint8_t tag =h.getTag();
        int ret = compareTag(tag, expectTag,isRequire);
        if (ret != eOK)
        {
            return 0;
        }
        switch (h.getType())
        {
        case AnalyzerDataHead::eList:
            {
                int size;
                read(size, t, type);
                cout << getTab()<< "read buf tag " <<  (int)tag  <<  " vector size:" << size << endl;
                if (size < 0|| size > 10 * 1024 * 1024)
                {
                    cout<< getTab() << "ERROR, invalid size, tag: " <<  (int)tag << endl;
                    exit(-1);
                }
                //addTab(7);
                
                cout << getTab()<<"{ //tag:" << (int)tag << endl;
                for (int i = 0; i < size; ++i)
                {
                
                    cout  << getTab() << " index:" << i << " ";
                    readType(pPtr->getTypePtr(), 0, isRequire);
                }
                
                cout << getTab()<<"} //tag:" << (int)tag << endl;
                //eraseTab(7);
            }
            break;
            
        case AnalyzerDataHead::eSimpleList:
            {
                
                AnalyzerDataHead hh;
                hh.readFrom(*this);
                if (hh.getType() != AnalyzerDataHead::eChar)
                {
                    cout<< getTab() << "error, type mismatch, tag: " <<  (int)tag;
                    
                    cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos ;
                }
                int size;
                read(size, t, type);
                
                cout<< getTab() << "read buf tag " <<  (int)tag << " vector byte size:" << size << endl;
                if (size < 0 || size > 10 * 1024 * 1024)
                {
                    cout<< getTab() << "ERROR, invalid size " << size << endl;
                    cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos ;
                    exit(-1);

                }
                
                char *ss = new char[size];
                try
                {
                    memset(ss, 0, size);
                    readBuf(ss, size);
                    // debug 再打开
                    //cout << getTab()<< "tag " << (int)tag << " vector byte :\n" << tars::TC_Common::bin2str(ss, size, " ",  chars_one_line) << endl;
                }
                catch (...)
                {
                    delete[] ss;
                    throw;
                }
                delete[] ss;


            }
            break;
        default:
            {
                 cout << getTab() << "ERROR, read 'vector' type mismatch, tag: " << (int)tag << "  get type: " <<  h.tostrType(h.getType()) << endl;
                cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos << endl;;
                exit(-1);
            }
        }
    }

    return 0;
}



int TarsAnalyzer::readMap(MapPtr pPtr, int expectTag, bool isRequire)
{

    int type;
    AnalyzerDataHead h;
    h.readFrom(*this);
    
    uint8_t tag = h.getTag();
    
    int ret = compareTag(tag, expectTag,isRequire);
    if (ret != eOK)
    {
        return 0;
    }
    switch (h.getType())
    {
    case AnalyzerDataHead::eMap:
        {
            int size;
            uint8_t t = 0;
            read(size, t, type);
            
            cout << "read " << " map size:" << size << endl;
            if (size < 0|| size > 10 * 1024 * 1024)
            {
                cout<< getTab() << "ERROR, invalid size " << size << endl;
                cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos ;
                exit(-1);
            }
            //addTab(7);
            cout << getTab()<< "{ //tag:" << (int)tag << endl;
            for (int i = 0; i < size; ++i)
            {
            
            cout  << getTab() << " index:" << i << " first:";
                readType(pPtr->getLeftTypePtr(), 0,  isRequire);
                cout<< getTab()<< " index:" << i << " second:" ;
                readType(pPtr->getRightTypePtr(), 1,  isRequire);

            }
            
            cout << getTab()<< "} //tag:" << (int)tag << endl;
            //eraseTab(7);
        }
        break;
    default:
        {
            cout << "ERROR, read map type mismatch, tag:" <<  (int)tag <<"get type " << h.tostrType(h.getType() )<< endl;
            cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos << endl;;
            exit(-1);
        }
    }
    
    return 0;

}

int TarsAnalyzer::readEnum(EnumPtr pPtr, int expectTag, bool isRequire)
{

    int n = 0;
    AnalyzerDataHead h;
    h.readFrom(*this);
    uint8_t tag = h.getTag();
    int ret = compareTag(tag, expectTag,isRequire);
    if (ret != eOK)
    {
        return 0;
    }
    switch (h.getType())
    {
            case AnalyzerDataHead::eZeroTag:
                n = 0;
                break;
            case AnalyzerDataHead::eChar:
                n = readByType<char>();
                break;
            case AnalyzerDataHead::eShort:
                n = (short) ntohs(readByType<short>());
                break;
            case AnalyzerDataHead::eInt32:
                this->readBuf(&n, sizeof(n));
                n = ntohl(n);
                break;
    
            default:
                {
                    cout<< "ERROR, read enum type mismatch, buf tag: " << (int)tag << ", get type: " << h.tostrType(h.getType()) << endl;
                    cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos ;
                    exit(-1);
                }
    }
    cout << "read enum:" << n << endl;
    return 0;


}

int TarsAnalyzer::readBuiltin(const BuiltinPtr &pPtr, int expectTag, bool bIsRequire, TypeIdPtr pType)
{
    uint8_t tag = expectTag;
    int realType = 0 ;

    switch (pPtr->kind())
    {
        case Builtin::KindBool:

        case Builtin::KindByte:

        case Builtin::KindShort:

        case Builtin::KindInt:

        case Builtin::KindLong:
            {

                Int64 n;
                read(n, tag, realType, bIsRequire);
                cout <<  "read buf tag [" <<(int)tag <<"|"<< AnalyzerDataHead::tostrType(realType)<<"|"<<n<<"]"<<endl;
            }
            //compareTag(tag, expectTag, bIsRequire);
            break;

        case Builtin::KindFloat:

        case Builtin::KindDouble:
            {
                double n;
                read(n, tag, realType, bIsRequire);
                cout <<  "read buf tag [" <<(int)tag <<"|"<< AnalyzerDataHead::tostrType(realType) << n<<"]"<<endl;
            }
            //compareTag(tag, expectTag, bIsRequire);
            break;

        case Builtin::KindString:
            {
                string s;
                read(s, tag, realType, bIsRequire);
                cout <<  "read buf tag [" <<(int)tag <<"|"<< AnalyzerDataHead::tostrType(realType)  << s<<"]"<<endl;
                //compareTag(tag, expectTag, bIsRequire);
               }
            break;

        default:
            cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos ;
            exit(-1);
    }
    
    return 0;
}

int TarsAnalyzer::compareTag(int tag, int expectTag, bool bIsRequire)
{
    if (tag != expectTag && bIsRequire)
    {
         cout << getTab() << "ERROR ,required tag is " << expectTag << ", but read tag:" << tag<< " from buf !" ;
         cout << getTab() << " _cur:0x" << ::hex<<_cur << " _last_pos:0x" << ::hex<<_last_pos << endl;
         //assert(false);
         exit(-1);
    }
    // optional tag
    if (expectTag < tag) //TARS 中tag小于从buf读出来的tag
    {
        // 可能对方没有编码expectTag到buf
        //因为是可选，就跳过tars文件中的expectTag
        //并回退AnalyzerDataHead，tars文件中下一个tag会用到
        cout << getTab() << "warning ,optional tag is " << expectTag << " < tag:" << tag<< " from buf !"  ;
        cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos << " " << endl;
        backToLast();
        return eNeedBackDataHead;
    }
    else if (expectTag > tag)
    {
        // 可能是TARS文件中增加了可选字段
        //可能TARS文件把tag号写错了
        //可能是TARS文件中把原来的可选字段去掉了
        cout << getTab() << "warning ,optional tag is " << expectTag << " < tag:" << tag<< " from buf !"  ;
        cout << getTab() << " _cur:" << _cur << " _last_pos:" << _last_pos << " ";
        backToLast();
        skipField();
        return eNeedSkipTag;
    }
    return eOK;
}
string TarsAnalyzer::tostrVector(const VectorPtr &pPtr)
{
    //数组类型
    if(pPtr->isArray())
    {
        return tostr(pPtr->getTypePtr());
    }

    //指针类型
    if(pPtr->isPointer())
    {
        return tostr(pPtr->getTypePtr())+" *";
    }

    string s = Builtin::builtinTable[Builtin::KindVector] + string("<") + tostr(pPtr->getTypePtr());

    if(MapPtr::dynamicCast(pPtr->getTypePtr()) || VectorPtr::dynamicCast(pPtr->getTypePtr()))
    {
        s += " >";
    }
    else
    {
        s += ">";
    }
    return s;
}


string TarsAnalyzer::tostrStruct(const StructPtr &pPtr) const
{
    return pPtr->getSid();
}

string TarsAnalyzer::tostrEnum(const EnumPtr &pPtr) 
{
    return pPtr->getSid();
}


int TarsAnalyzer::readStruct(const StructPtr & pPtr, const NamespacePtr & nPtr)
{
    vector<TypeIdPtr>& member = pPtr->getAllMemberPtr();

    AnalyzerDataHead h;
    size_t hLen = h.peekFrom(*this);
    if (AnalyzerDataHead::eStructBegin == h.getType()) // 检查结构开始
    {
        cout << endl;
        incTab();
        cout << getTab()<<  "struct " << pPtr->getId() << " {"  << endl;
        skip(hLen);
    }

    //解析struct成员变量
    for (size_t i = 0; i < member.size(); i++)
    {
        cout<< getTab() <<"expect tag ";
        cout.setf(ios::right);
        cout.width(CHAR_WIDTH_TAG);
        cout<< member[i]->getTag();
        cout.width(CHAR_WIDTH_ID); // 成员名12字符宽
        cout<< member[i]->getId();

        //获取tup版本号
        if(isTup() && (pPtr->getId() == "RequestPacket"))
        {
            if(member[i]->getTag() == 1)
            {
                char iVer = 2;
                this->peekBuf(&iVer, sizeof(iVer),1);
                _tupVersion = iVer;
            }
            else if(member[i]->getTag() == 7)///解析sbuffer
            {
                _tup_sbuffer_pos = _cur;
                _cur +=2; //7D 00

                int n;
                uint8_t  tag = 0;
                int  realType;
                read(n,  tag,  realType, true ); //sbuffer的长度

                if(_tupVersion == 2)
                {
                    read(_tup2_helper, 7);
                    _bIsDumpTup?dumpTup(_tup2_helper):(void(0));
                }
                else///3
                {
                    read(_tup3_helper, 7);
                    _bIsDumpTup?dumpTup(_tup3_helper):(void(0));
                }

                _cur = _tup_sbuffer_pos;
            }
        }

        //读取内置基本类型
        BuiltinPtr bPtr = BuiltinPtr::dynamicCast(member[i]->getTypePtr());
        if (bPtr)
        {
            cout.width(CHAR_WIDTH_TYPE);
            cout<<tostrBuiltin(bPtr);

            cout<<getSep();
            readBuiltin(bPtr, member[i]->getTag(), member[i]->isRequire(), NULL);
        }

        VectorPtr vPtr = VectorPtr::dynamicCast(member[i]->getTypePtr());
        if (vPtr)
        {
            cout.width(CHAR_WIDTH_TYPE);
            cout<<tostrVector(vPtr);
            cout<<getSep();
            readVector(vPtr, member[i]->getTag(),member[i]->isRequire(), NULL);
        }

        MapPtr mPtr   = MapPtr::dynamicCast(member[i]->getTypePtr());
        if (mPtr)
        {
            cout.width(CHAR_WIDTH_TYPE);
            cout<<tostrMap(mPtr);
            cout<<getSep();
            readMap(mPtr, member[i]->getTag(), member[i]->isRequire());
        }

        StructPtr sPtr = StructPtr::dynamicCast(member[i]->getTypePtr());
        if (sPtr)
        {
            cout.width(CHAR_WIDTH_TYPE);
            cout<< tostrStruct(sPtr);
            cout<<getSep();
            readStruct(sPtr, NULL);
        }

        EnumPtr ePtr = EnumPtr::dynamicCast(member[i]->getTypePtr());
        if (ePtr)
        {

            cout.width(CHAR_WIDTH_TYPE);
            cout<< tostrEnum(ePtr);
            cout<<getSep();
            readEnum(ePtr , member[i]->getTag(), member[i]->isRequire());
        }

    }

     if (!_bTup)
     {
        hLen = h.peekFrom(*this); //buf中没有结构结束标志，也不会抛异常，因为setBuffer的buf_len有1个保护字节
        if (AnalyzerDataHead::eStructEnd == h.getType() || _cur >= _buf_len-chars_buf_reserve) // 检查结构结束
        {
            cout << getTab() <<  "} //struct " << pPtr->getId() <<  endl;
            decTab();
            skip(hLen);
            if (pPtr->getId() == _StructName)
            {
                cout << "analyze " << _streamFileName << " with struct " << _StructName << " ok!" << endl;
            }
        }
     }
    return 0;

}

void TarsAnalyzer::dumpTup(const map<string, vector<char> >& tupData)
{
    map<string, vector<char> >::const_iterator it = tupData.begin();
    cout << "\ntup attr begin:" << endl;
    while(it != tupData.end())
    {
        cout << "tup Attribute name: " << it->first << endl;
        vector<char> vContent = it->second;
        cout << "struct buff: ";
        cout << tars::TC_Common::bin2str(&vContent[0], vContent.size(), " ", chars_one_line ) << endl;
        it++;
    }
    cout << "tup attr end:\n" << endl;
}

void TarsAnalyzer::dumpTup(const map<string, map<string, vector<char> > >& tupData)
{
    map<string, map<string, vector<char> > >::const_iterator it1 = tupData.begin();
    cout << "\ntup attr begin:" << endl;
    while (it1 != tupData.end())
    {
        cout << "tup Attribute name: " << it1->first << endl;
        map<string, vector<char> >::const_iterator it2 = it1->second.begin();
        while(it2 != it1->second.end())
        {

            cout << "struct name: " << it2->first << endl;
            vector<char> vContent = it2->second;

            //int lines = vContent.size()/chars_one_line + 1;
            //cout << "struct buff lines:"<< lines << endl;
            cout << "struct buff: ";
            cout << tars::TC_Common::bin2str(&vContent[0], vContent.size(), " ", chars_one_line ) << endl;
            it2++;
        }

        it1++;
    }
    cout << "tup attr end:\n" << endl;
}

