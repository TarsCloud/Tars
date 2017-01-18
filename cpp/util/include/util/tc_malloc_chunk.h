#ifndef __TC_MALLOC_CHUNK_H
#define __TC_MALLOC_CHUNK_H

#include <iostream>
#include <cassert>
#include <stdlib.h>
#include <string.h>
#include <vector>

using namespace std;

namespace tars
{
    /////////////////////////////////////////////////
    /**  
    * @file tc_mem_chunk.h
    * @brief 内存分配器 
    *          
    */ 
    static const size_t kPageShift  = 15;
    static const size_t kNumClasses = 78;
    static const size_t kPageSize   = 1 << kPageShift;
    static const size_t kMaxSize    = 256 * 1024;
    static const size_t kAlignment  = 8;
    static const size_t kMaxPages    = 1 << (20 - kPageShift);
    static const size_t kPageAlignment = 64;

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /**
    * 请求的内存大小与请求实际得到的内存大小的映射
    * 实际得到的内存大小与内存大小所属的大小类别的映射
    * 内存大小所属的大小类别与该内存大小类别需要的内存页的映射
    */
    class SizeMap
    {
    public:
        SizeMap() { Init(); }    

        /*
        *size所属的尺寸类别
        */
        inline int SizeClass(int size)
        {
            return class_array_[ClassIndex(size)];
        }

        /*
        *尺寸类别cl对应的尺寸大小
        */
        inline size_t ByteSizeForClass(size_t cl)
        {
            return class_to_size_[cl];
        }

        /*
        *尺寸类别cl对应的页数
        */
        inline size_t class_to_pages(size_t cl)
        {
            return class_to_pages_[cl];
        }

    private:
        static inline size_t ClassIndex(int s)
        {
            const bool big = (s > kMaxSmallSize);
            const size_t  add_amount= big ? (127 + (120<<7)) : 7;
            const size_t  shift_amount = big ? 7 : 3;
            return (s + add_amount) >> shift_amount;
        }

        static inline int LgFloor(size_t n)
        {
            int log = 0;
            for (int i = 4; i >= 0; --i)
            {
                int shift = (1 << i);
                size_t x = n >> shift;
                if (x != 0)
                {
                    n = x;
                    log += shift;
                }
            }

            return log;
        }

        /*
        *初始化
        */
        void Init();

        size_t AlignmentForSize(size_t size);

        int NumMoveSize(size_t size);
    private:
        static const int kMaxSmallSize        = 1024;
        static const size_t kClassArraySize = ((kMaxSize + 127 + (120 << 7)) >> 7) + 1;
        size_t            class_to_size_[kNumClasses];
        size_t            class_to_pages_[kNumClasses];
        unsigned char    class_array_[kClassArraySize];

    };
    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class Static
    {
    public:
        static SizeMap* sizemap()
        {
            return &_sizemap;
        }
    private:
        static SizeMap _sizemap;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /**
    * TC_Span内存分配器
    *
    * 用于分配sizeof(TC_Span)大小的内存块
    */
    class TC_SpanAllocator
    {
    public:
        TC_SpanAllocator() : _pHead(NULL), _pData(NULL) {}

        void* getHead() const { return _pHead; }

        /**
         * 初始化
         * @param pAddr, 地址, 换到应用程序的绝对地址
         * @param mem_size, 内存大小
         */
        void create(void* pAddr, size_t iSpanSize, size_t iSpanCount);

        /**
         * 连接上
         * @param pAddr, 地址, 换到应用程序的绝对地址
         */
        void connect(void* pAddr);

        /**
         * 重建
         */
        void rebuild();

        /**
         * 是否还有可用block
         *
         * @return bool
         */
        bool isSpanAvailable() const { return _pHead->_spanAvailable > 0; }

        /**
         * 分配一个区块
         *
         * @return void*
         */
        void* allocate();

        /**
         * 释放区块
         * @param pAddr
         */
        void deallocate(void *pAddr);

        struct tagSpanAlloc
        {
            size_t  _iSpanSize;            /**span区块大小*/
            size_t  _iSpanCount;           /**span个数*/
            size_t  _firstAvailableSpan;   /**第一个可用的span索引*/
            size_t  _spanAvailable;        /**可用span个数*/
        }__attribute__((packed));

        static size_t getHeadSize() { return sizeof(tagSpanAlloc); }

    protected:
        /**
         * 初始化
         */
        void init(void *pAddr);

        //禁止copy构造
        TC_SpanAllocator(const TC_SpanAllocator &mcm);

        //禁止复制
        TC_SpanAllocator &operator=(const TC_SpanAllocator &mcm);

    private:
        /**
         * 区块头指针
         */
        tagSpanAlloc    *_pHead;

        /**
         * 数据区指针
         */
        unsigned char   *_pData;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    /**
    * 内存分配器
    *
    * 将连续的内存按32K大小分页
    * 并通过TC_Span和PageMap进行管理
    */
    class TC_Page
    {
    public:
        struct tagShmFlag
        {
            bool         _bShmProtectedArea;    /*是否设置保护区*/
            int          _iShmFlag;                /*内存空间标识，内存未使用前为0，使用后为1*/
            size_t       _iShmMemSize;            /*内存空间大小*/
            size_t       _iShmSpanAddr;            /*TC_Span内存区的起始地址，相对地址*/
            size_t       _iShmPageMapAddr;        /*PageMap内存区的起始地址，相对地址*/
            size_t       _iShmPageAddr;            /*Data区的内存区的其实地址，相对地址*/
            size_t       _iShmSpanNum;            /*TC_Span内存区中span的个数*/
            size_t       _iShmPageMapNum;        /*PageMap内存区中map的个数*/
            size_t       _iShmPageNum;            /*Data内存区中页的个数*/
        }__attribute__((packed));

        struct tagModifyData
        {
            size_t       _iModifyAddr;            /*修改的地址*/
            char         _cBytes;                /*字节数*/
            size_t       _iModifyValue;            /*值*/
        }__attribute__((packed));

        struct tagModifyHead
        {
            char           _cModifyStatus;        /*修改状态: 0:目前没有人修改, 1: 开始准备修改, 2:修改完毕, 没有copy到内存中*/
            size_t         _iNowIndex;            /*更新到目前的索引*/
            tagModifyData  _stModifyData[32];    /*一次最多32次修改*/
        }__attribute__((packed));

        struct TC_Span
        {
            size_t       start;                    /*span所管理的页内存的起始页号*/
            size_t       length;                /*页的个数*/
            size_t       next;
            size_t       prev;
            size_t       objects;                /*用于分配内存*/
            unsigned int refcount;                /*按照sizeclass类大小划分span拥有的内存后的个数*/
            unsigned int sizeclass;                /*内存大小的类别*/
            unsigned int location;                /*用于识别该span是空闲，还是处于使用中*/

            enum { IN_USE, ON_FREELIST };
        }__attribute__((packed));    

        struct TC_CenterList
        {
            size_t       size_class;            /*内存大小的类别*/
            TC_Span      empty;                    /*空闲链表*/
            TC_Span      nonempty;                /*非空闲链表*/
        }__attribute__((packed));

    public:
        TC_Page() : _pShmFlagHead(NULL),_pModifyHead(NULL),_pCenterCache(NULL),_pLarge(NULL),_pFree(NULL),_pSpanMemHead(NULL),_pPageMap(NULL),_pData(NULL) {}

        /**
         * 初始化
         * @param pAddr, 地址, 换到应用程序的绝对地址
         * @param iShmMemSize, 内存大小
         * @param bProtectedArea, 是否使用保护区
         */
        void create(void *pAddr, size_t iShmMemSize, bool bProtectedArea);

        /**
         * 连接上
         * @param pAddr, 地址, 换到应用程序的绝对地址
         */
        void connect(void *pAddr);

        /**
         * 重建
         */
        void rebuild();

        /**
         * 分配一个区块
         * @param iClassSize,需要分配的内存大小类别
         * @param iAllocSize, 分配的数据块大小
         * @param iPageId, 该内存所属的TC_Span的起始页号
         * @param iIndex, 该内存所属的TC_Span的按iAllocSize大小划分后的第iIndex个
         * @return void *
         */
        void* fetchFromSpansSafe(size_t iClassSize, size_t &iAllocSize, size_t &iPageId, size_t &iIndex);

        /**
         * 释放内存, 根据该内存所属TC_Span的起始页和该TC_Span按大小类别划分后的第iIndex个
         * @param iPageId, 该内存所属的TC_Span的起始页号
         * @param iIndex, 该内存所属的TC_Span的按iAllocSize大小划分后的第iIndex个
         */
        void  releaseToSpans(size_t iPageId, size_t iIndex);

        /**
         * 释放内存, 绝对地址
         * @param pAddr
         */
        void  releaseToSpans(void* pObject);

        /**
         * 根据该内存所属TC_Span的起始页和该TC_Span按大小类别划分后的第iIndex个得到该内存块的起始绝对地址
         * @param iPageId, 该内存所属的TC_Span的起始页号
         * @param iIndex, 该内存所属的TC_Span的按iAllocSize大小划分后的第iIndex个
         */
        void* getAbsolute(size_t iPageId, size_t iIndex);

        /**
         * 修改更新到内存中
         */
        void doUpdate(bool bUpdate = false);

        /**
         * 获得用于存放数据的页内存的数量
         */
        inline size_t getPageNumber();

        /**
         * 获得用于存放数据的页内存的大小
         */
        inline size_t getPageMemSize();

        /**
         * 获得TC_Page所管理内存的结束位置
         */
        inline size_t getPageMemEnd();

        /**
         * 传给TC_Page的内存的最小大小
         */
        static size_t getMinMemSize() 
        { 
            return sizeof(tagShmFlag) + sizeof(tagModifyHead) + sizeof(TC_CenterList) * kNumClasses + sizeof(TC_Span) + sizeof(TC_Span) * kMaxPages + TC_SpanAllocator::getHeadSize() + sizeof(TC_Span) + sizeof(size_t) + kPageSize; 
        }

    protected:

        //禁止copy构造
        TC_Page(const TC_Page &mcm);

        //禁止复制
        TC_Page &operator=(const TC_Page &mcm);

        /**
         * 初始化
         */
        void init(void *pAddr);

        /**
         * 按页初始化用于存放数据的内存
         */
        void initPage(void *pAddr);


        /////////////////////////////////////////////////////////////////////////////////////////////////
        /**
         * 初始化list双向链表
         */
        inline void DLL_Init(TC_Span* list, size_t iIndex);

        /**
         * 从双向链表中删除span指向的节点
         */
        inline void DLL_Remove(TC_Span* span);

        /**
         * 将span指向的节点加入到双向链表list中
         */
        inline void DLL_Prepend(TC_Span* list, TC_Span* span);

        /**
         * 双向链表list是否为空
         */
        inline bool DLL_IsEmpty(TC_Span* list, size_t iIndex) 
        {
          return list->next == iIndex;
        }

        //////////////////////////////////////////////////////////////////////////////////////////////////
        /**
         * 得到PageMap中第k个元素存放的指针值，其实际意义就是看Data内存区中第k块内存页属于哪个TC_Span管理
         */
        size_t Get(size_t k) const
        {
            return _pPageMap[k];
        }

        /**
         * 设置PageMap中第k个元素存放的指针值，其实际意义就是看Data内存区中第k块内存页属于哪个TC_Span管理
         */
        void Set(size_t k, size_t v)
        {
            _pPageMap[k] = v;
        }

        /**
         * 确保n未超过页内存的数目与k之差
         */
        bool Ensure(size_t k, size_t n)
        {
            return n <= _pShmFlagHead->_iShmPageMapNum -k;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////////
        /**
         * create或rebuild的时候会调用，用于初始化时将所有的内存页映射到_pLarge上
         */
        bool UseShmMem();

        /**
         * 得到内存页pPageId属于哪个TC_Span管理
         */
        TC_Span* GetDescriptor(size_t pPageId);

        /**
         * 在_pFree或_pLarge链表中查找n块连续的内存页，通过TC_Span返回
         */
        TC_Span* SearchFreeAndLargeLists(size_t n);

        /**
         * 分配n块连续的内存页，通过TC_Span返回，里面会调用SearchFreeAndLargeLists函数
         */
        TC_Span* New(size_t n);

        /**
         * 在_pLarge链表中分配n块连续的内存页，通过TC_Span返回
         */
        TC_Span* AllocLarge(size_t n);

        /**
         * 在span所管理的内存页中，分配出n块连续内存
         */
        TC_Span* Carve(TC_Span* span, size_t n);

        /**
         * 根据span所管理的内存页的数目，将该span插入到_pLarge或_pFree链表中
         */
        void PrependToFreeList(TC_Span* span);

        /**
         * 将span从它所属的链表中删除，里面调用MergeIntoFreeList
         */
        void Delete(TC_Span* span);

        /**
         * 将span从它所属的链表中删除，并且查看该span所管理的内存页前后连续的内存页是否空闲，若是，则进行合并操作
         */
        void MergeIntoFreeList(TC_Span* span);

        /**
         * 设置span所管理的内存页将要进行分割的内存大小类别
         */
        void RegisterSizeClass(TC_Span* span, size_t sc);

        /**
         * 将span所属的内存页，映射到_pPageMap中
         */
        void RecordSpan(TC_Span* span);

        //////////////////////////////////////////////////////////////////////////////////////////////////

        /**
         * 分配一个区块
         * @param iClassSize,需要分配的内存大小类别
         * @param iAllocSize, 分配的数据块大小
         * @param iPageId, 该内存所属的TC_Span的起始页号
         * @param iIndex, 该内存所属的TC_Span的按iAllocSize大小划分后的第iIndex个
         * @return void *
         */
        void* FetchFromSpans(size_t iClassSize, size_t &iAllocSize, size_t &iPageId, size_t &iIndex);

        /**
         * 按iClassSize类别的内存大小分割内存页
         */
        int Populate(size_t iClassSize);

        /**
         * 修改具体的值
         * @param iModifyAddr
         * @param iModifyValue
         */
        //inline void update(void* iModifyAddr, size_t iModifyValue);
        template<typename T>
        void update(void* iModifyAddr, T iModifyValue)
        {
            _pModifyHead->_cModifyStatus = 1;
            _pModifyHead->_stModifyData[_pModifyHead->_iNowIndex]._iModifyAddr  = reinterpret_cast<size_t>(iModifyAddr) - reinterpret_cast<size_t>(_pShmFlagHead);
            _pModifyHead->_stModifyData[_pModifyHead->_iNowIndex]._iModifyValue = iModifyValue;
            _pModifyHead->_stModifyData[_pModifyHead->_iNowIndex]._cBytes       = sizeof(iModifyValue);
            _pModifyHead->_iNowIndex++;

            assert(_pModifyHead->_iNowIndex < sizeof(_pModifyHead->_stModifyData) / sizeof(tagModifyData));
        }
    
    private:
        /**
         * 头部内存块头指针
         */
        tagShmFlag                *_pShmFlagHead;
        /**
         * 保护区内存块头指针
         */
        tagModifyHead             *_pModifyHead;
        /**
         * 中央自由链表头指针
         */
        TC_CenterList             *_pCenterCache;
        /**
         * 大块内存页的链表指针（大于等于1MB内存块的链表）
         */
        TC_Span                   *_pLarge;
        /**
         * 小块内存页的链表头指针（小于1MB的内存块）
         */
        TC_Span                   *_pFree;
        /**
         * 用于分配TC_Span的内存区域的头指针
         */
        TC_SpanAllocator          *_pSpanMemHead;
        /**
         * 与数据区内存相映射的内存区头指针
         */
        //void                      **_pPageMap;
        size_t                      *_pPageMap;
        /**
         * 数据区的头指针
         */
        void                      *_pData;

        /**
         * 用于分配TC_Span的内存分配器
         */
        TC_SpanAllocator          _spanAlloc;
    };

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    class TC_MallocChunkAllocator
    {
    public:
        TC_MallocChunkAllocator():_pHead(NULL),_pChunk(NULL),_nallocator(NULL) {}

        ~TC_MallocChunkAllocator()
        {
            clear();
        }

        void clear()
        {
            if(_nallocator)
            {
                delete _nallocator;
                _nallocator = NULL;
            }

            _pHead = NULL;
            _pChunk = NULL;
        }

        /**
         * 初始化
         * @param pAddr, 地址, 换到应用程序的绝对地址
         * @param iSize, 内存大小
         * @param bProtectedArea, 是否使用保护区,默认使用
         */
        void create(void *pAddr, size_t iSize, bool bProtectedArea = true);

        /**
         * 连接上
         * @param pAddr, 地址, 换到应用程序的绝对地址
         */
        void connect(void *pAddr);

        /**
         * 扩展空间
         * 
         * @param pAddr, 已经是空间被扩展之后的地址
         * @param iSize
         */
        void append(void *pAddr, size_t iSize);

        void* getHead() const { return _pHead; }

        /**
         * 总计内存大小, 包括后续增加的内存块的大小
         *
         * @return size_t
         */
        size_t getMemSize() const { return _pHead->_iTotalSize; }

        /**
         * 用于存放数据的总计内存大小, 包括后续增加的内存块的大小
         * 
         */
        size_t getAllCapacity();

        /**
         * 每个chunk的大小, 包括后续增加的内存块的大小
         *
         * vector<size_t>
         */
        void   getSingleCapacity(vector<size_t> &vec_capacity);

        /**
         * 根据该内存所属TC_Span的起始页和该TC_Span按大小类别划分后的第iIndex个，换算成绝对地址
         * @param iPageId
         * @param iIndex
         * @return void*
         */
        void* getAbsolute(size_t iPageId, size_t iIndex);

        /**
         * 分配一个区块,绝对地址
         *
         * @param iNeedSize,需要分配的大小
         * @param iAllocSize, 分配的数据块大小
         * @return void*
         */
        void* allocate(size_t iNeedSize, size_t &iAllocSize);

        /**
         * 释放区块, 绝对地址
         * @param pAddr
         */
        void  deallocate(void* pAddr);

        /**
         * 分配一个区块
         * @param iNeedSize,需要分配的大小
         * @param iAllocSize, 分配的数据块大小
         * @param iPageId, 该内存所属的TC_Span的起始页号
         * @param iIndex, 该内存所属的TC_Span的按iAllocSize大小划分后的第iIndex个
         * @return void *
         */
        void* allocate(size_t iNeedSize, size_t &iAllocSize, size_t &iPageId, size_t &iIndex);

        /**
         * 释放内存, 根据该内存所属TC_Span的起始页和该TC_Span按大小类别划分后的第iIndex个
         * @param iPageId, 该内存所属的TC_Span的起始页号
         * @param iIndex, 该内存所属的TC_Span的按iAllocSize大小划分后的第iIndex个
         */
        void  deallocate(size_t iPageId, size_t iIndex);

        /**
         * 重建
         */
        void rebuild();

        /**
         * 修改更新到内存中
         */
        void doUpdate(bool bUpdate = false);

        /**
         * 头部内存块
         */
        struct tagChunkAllocatorHead
        {
            bool   _bProtectedArea;
            size_t _iSize;
            size_t _iTotalSize;
            size_t _iNext;
        }__attribute__((packed));

        /**
         * 头部内存块大小
         */
        static size_t getHeadSize() { return sizeof(tagChunkAllocatorHead); }

        /**
         * 传递给此内存分配器的内存块大小要不小于函数的返回值
         */
        static size_t getNeedMinSize() { return sizeof(tagChunkAllocatorHead) + TC_Page::getMinMemSize(); }
    protected:
        void init(void *pAddr);

        void _connect(void *pAddr);

        TC_MallocChunkAllocator *lastAlloc();

        //禁止copy构造
        TC_MallocChunkAllocator(const TC_MallocChunkAllocator &);

        //禁止复制
        TC_MallocChunkAllocator& operator=(const TC_MallocChunkAllocator &);

    private:
        /**
         * 头指针
         */
        tagChunkAllocatorHead   *_pHead;
        /**
         *  chunk开始的指针
         */
        void                    *_pChunk;
        /**
         *  chunk分配类
         */
        TC_Page                 _page;
        /**
         * 后续的多块分配器
         */
        TC_MallocChunkAllocator *_nallocator;
    };
    
}
#endif
