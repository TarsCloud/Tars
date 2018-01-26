#ifndef __TC_MD5_H
#define __TC_MD5_H

#include <string>
#include "util/tc_common.h"

using namespace std;

namespace tars
{
/////////////////////////////////////////////////
/**
 * @file tc_md5.h
 * @brief md5类(修改并完善至md5的c版本)
 * 
 */            
/////////////////////////////////////////////////

/**
* @brief 该类提供MD5的散列算法，通过静态函数提供 . 
*  
* 提供两种输出方式:字符串(32个字符)或二进制(16个字节)
*/
#ifndef GET_ULONG_LE
#define GET_ULONG_LE(n,b,i)                             \
{                                                       \
    (n) = ( (unsigned long) (b)[(i)    ]       )        \
        | ( (unsigned long) (b)[(i) + 1] <<  8 )        \
        | ( (unsigned long) (b)[(i) + 2] << 16 )        \
        | ( (unsigned long) (b)[(i) + 3] << 24 );       \
}
#endif

#ifndef PUT_ULONG_LE
#define PUT_ULONG_LE(n,b,i)                             \
{                                                       \
    (b)[(i)    ] = (unsigned char) ( (n)       );       \
    (b)[(i) + 1] = (unsigned char) ( (n) >>  8 );       \
    (b)[(i) + 2] = (unsigned char) ( (n) >> 16 );       \
    (b)[(i) + 3] = (unsigned char) ( (n) >> 24 );       \
}
#endif

class TC_MD5
{
    typedef unsigned char *POINTER;
    typedef unsigned short int UINT2;
    //typedef unsigned long int UINT4;
    typedef uint32_t UINT4;

    typedef struct 
    {
        /**
        * state (ABCD)
        */
        //unsigned long state[4];        
        UINT4 state[4];        

        /**
        * number of bits, modulo 2^64 (lsb first)
        */
        //unsigned long count[2];        
        UINT4 count[2];        

        /**
        * input buffer
        */
        unsigned char buffer[64];      
    }MD5_CTX;

public:
    /**
    * @brief 对字符串进行md5处理,返回16字节二进制数据. 
    *  
    * @param sString  输入字符串
    * @return string 输出,16个字节的二进制数据
    */
    static string md5bin(const string &sString);

    /**
    * @brief 对字符串进行md5处理 ，
    *        将md5的二进制数据转换成HEX的32个字节的字符串
    * @param sString  输入字符串
    * @return string 输出,32个字符
    */
    static string md5str (const string &sString);

    /**
     * @brief 对文件进行md5处理. 
     *  
     * @param fileName 要处理的文件名 
     * @return string  处理后的字符串
     */
    static string md5file(const string& fileName);

protected:

    static string bin2str(const void *buf, size_t len, const string &sSep);

    /**
    * @brief MD5 init.
    *  
    * @param context 上下文信息
    * @return
    */ 
    static void md5init(MD5_CTX *context);
    
    /** 
    * @brief MD5 block update operation，Continues an MD5 
    * message-digest operation, processing another message block, 
    * and updating the context 
    * @param context  上下文信息
    * @param input    输入
    * @param inputLen 输入长度
    * @return 
    */    
    static void md5update (MD5_CTX *context, unsigned char *input,unsigned int inputLen);    
    
    /** 
    * @brief  MD5 finalization，Ends an MD5 message-digest 
    * operation, writing the message digest and zeroizing the 
    * context 
    * @param digest   摘要
    * @param context 上下文信息
    */
    static void md5final (unsigned char digest[16], MD5_CTX *context);
    
    /** 
    * @brief  MD5 basic transformation，Transforms state based on 
    *         block
    * @param state 状态
    * @param block : ...
    */
    static void md5_process( MD5_CTX *ctx, const unsigned char data[64]);
    
    /** 
    * @brief  Encodes input (UINT4) into output (unsigned 
    *         char)，Assumes len is a multiple of 4
    * @param output 输出
    * @param input  输入
    * @param len    输入长度
    */    
    static void encode (unsigned char *output,UINT4 *input,unsigned int len);
    
    /** 
    * @brief Decodes input (unsigned char) into output (UINT4)， 
    * Assumes len is a multiple of 4
    * @param output 输出
    * @param input  输入
    * @param len    输入长度
    */    
    static void decode (UINT4 *output,unsigned char *input,unsigned int len);
    
    /** 
    * @brief replace "for loop" with standard memcpy if possible. 
    *  
    * @param output  输出
    * @param input   输入
    * @param len     输入长度
    */    
    static void md5_memcpy (POINTER output,POINTER input,unsigned int len);
    
    /** 
    * @brief replace "for loop" with standard memset if possible. 
    *  
    * @param output 输出
    * @param value  值
    * @param len    输入长度
    */ 
    static void md5_memset (POINTER output,int value,unsigned int len);
    
    /**
    * 填充值
    */
    static unsigned char PADDING[64];
};

}
#endif
