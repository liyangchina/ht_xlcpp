//sha1.h：对字符串进行sha1加密
//#include "stdafx.h"
#include "sha1.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <assert.h> 
#include <iostream>
#include "STRING_Helper.h"
 //字节序的小头和大头的问题
 #define ZEN_LITTLE_ENDIAN  0x0123
 #define ZEN_BIG_ENDIAN     0x3210

 //目前所有的代码都是为了小头党服务的，不知道有生之年这套代码是否还会为大头党服务一次？
 #ifndef ZEN_BYTES_ORDER
 #define ZEN_BYTES_ORDER    ZEN_LITTLE_ENDIAN
 #endif

 #ifndef ZEN_SWAP_UINT16
 #define ZEN_SWAP_UINT16(x)  ((((x) & 0xff00) >>  8) | (((x) & 0x00ff) <<  8))
 #endif
 #ifndef ZEN_SWAP_UINT32
 #define ZEN_SWAP_UINT32(x)  ((((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >>  8) | \
     (((x) & 0x0000ff00) <<  8) | (((x) & 0x000000ff) << 24))
 #endif
 #ifndef ZEN_SWAP_UINT64
 #define ZEN_SWAP_UINT64(x)  ((((x) & 0xff00000000000000) >> 56) | (((x) & 0x00ff000000000000) >>  40) | \
     (((x) & 0x0000ff0000000000) >> 24) | (((x) & 0x000000ff00000000) >>  8) | \
     (((x) & 0x00000000ff000000) << 8 ) | (((x) & 0x0000000000ff0000) <<  24) | \
     (((x) & 0x000000000000ff00) << 40 ) | (((x) & 0x00000000000000ff) <<  56))
 #endif
void debug_hash(uint32_t hash[5]){

}

 //将一个（字符串）数组，拷贝到另外一个uint32_t数组，同时每个uint32_t反字节序
 void *swap_uint32_memcpy(void *to, const void *from, size_t length)
 {
     memcpy(to, from, length);
     size_t remain_len =  (4 - (length & 3)) & 3;
 
     //数据不是4字节的倍数,补充0
     if (remain_len)
     {
         for (size_t i = 0; i < remain_len; ++i)
         {
             *((char *)(to) + length + i) = 0;
         }
         //调整成4的倍数
         length += remain_len;
     }
 
     //所有的数据反转
     for (size_t i = 0; i < length / 4; ++i)
     {
         ((uint32_t *)to)[i] = ZEN_SWAP_UINT32(((uint32_t *)to)[i]);
     }
 
     return to;
 }
 
 ///MD5的结果数据长度
 static const size_t ZEN_MD5_HASH_SIZE   = 16;
 ///SHA1的结果数据长度
 static const size_t ZEN_SHA1_HASH_SIZE  = 20;
 
 
 
 namespace ZEN_LIB
 {
 
 
 /*!
 @brief      求某个内存块的MD5，
 @return     unsigned char* 返回的的结果，
 @param[in]  buf    求MD5的内存BUFFER指针
 @param[in]  size   BUFFER长度
 @param[out] result 结果
 */
 unsigned char *md5(const unsigned char *buf,
                    size_t size,
                    unsigned char result[ZEN_MD5_HASH_SIZE]);
 
 
 /*!
 @brief      求内存块BUFFER的SHA1值
 @return     unsigned char* 返回的的结果
 @param[in]  buf    求SHA1的内存BUFFER指针
 @param[in]  size   BUFFER长度
 @param[out] result 结果
 */
 unsigned char *sha1(const unsigned char *buf,
                     size_t size,
                     unsigned char result[ZEN_SHA1_HASH_SIZE]);
 };
 
 
 //================================================================================================
 //MD5的算法
 
 //每次处理的BLOCK的大小
 static const size_t ZEN_MD5_BLOCK_SIZE = 64;
 
 //md5算法的上下文，保存一些状态，中间数据，结果
 typedef struct md5_ctx
 {
     //处理的数据的长度
     uint64_t length_;
     //还没有处理的数据长度
     uint64_t unprocessed_;
     //取得的HASH结果（中间数据）
     uint32_t  hash_[4];
 } md5_ctx;
 
 
 #define ROTL32(dword, n) ((dword) << (n) ^ ((dword) >> (32 - (n))))
 #define ROTR32(dword, n) ((dword) >> (n) ^ ((dword) << (32 - (n))))
 #define ROTL64(qword, n) ((qword) << (n) ^ ((qword) >> (64 - (n))))
 #define ROTR64(qword, n) ((qword) >> (n) ^ ((qword) << (64 - (n))))
 
 
 /*!
 @brief      内部函数，初始化MD5的context，内容
 @param      ctx
 */
 static void zen_md5_init(md5_ctx *ctx)
 {
     ctx->length_ = 0;
     ctx->unprocessed_ = 0;
 
     /* initialize state */
     ctx->hash_[0] = 0x67452301;
     ctx->hash_[1] = 0xefcdab89;
     ctx->hash_[2] = 0x98badcfe;
     ctx->hash_[3] = 0x10325476;
 }
 
 /* First, define four auxiliary functions that each take as input
  * three 32-bit words and returns a 32-bit word.*/
 
 /* F(x,y,z) = ((y XOR z) AND x) XOR z - is faster then original version */
 #define MD5_F(x, y, z) ((((y) ^ (z)) & (x)) ^ (z))
 #define MD5_G(x, y, z) (((x) & (z)) | ((y) & (~z)))
 #define MD5_H(x, y, z) ((x) ^ (y) ^ (z))
 #define MD5_I(x, y, z) ((y) ^ ((x) | (~z)))
 
 /* transformations for rounds 1, 2, 3, and 4. */
 #define MD5_ROUND1(a, b, c, d, x, s, ac) { \
         (a) += MD5_F((b), (c), (d)) + (x) + (ac); \
         (a) = ROTL32((a), (s)); \
         (a) += (b); \
     }
 #define MD5_ROUND2(a, b, c, d, x, s, ac) { \
         (a) += MD5_G((b), (c), (d)) + (x) + (ac); \
         (a) = ROTL32((a), (s)); \
         (a) += (b); \
     }
 #define MD5_ROUND3(a, b, c, d, x, s, ac) { \
         (a) += MD5_H((b), (c), (d)) + (x) + (ac); \
         (a) = ROTL32((a), (s)); \
         (a) += (b); \
     }
 #define MD5_ROUND4(a, b, c, d, x, s, ac) { \
         (a) += MD5_I((b), (c), (d)) + (x) + (ac); \
         (a) = ROTL32((a), (s)); \
         (a) += (b); \
     }
 
 
 /*!
 @brief      内部函数，将64个字节，16个uint32_t的数组进行摘要（杂凑）处理，处理的数据自己序是小头数据
 @param      state 存放处理的hash数据结果
 @param      block 要处理的block，64个字节，16个uint32_t的数组
 */
 static void zen_md5_process_block(uint32_t state[4], const uint32_t block[ZEN_MD5_BLOCK_SIZE / 4])
 {
     register unsigned a, b, c, d;
     a = state[0];
     b = state[1];
     c = state[2];
     d = state[3];
 
     const uint32_t *x = NULL;
 
     //MD5里面计算的数据都是小头数据.大头党的数据要处理
 #if ZEN_BYTES_ORDER == ZEN_LITTLE_ENDIAN
     x = block;
 #else
     uint32_t swap_block[ZEN_MD5_BLOCK_SIZE / 4];
     swap_uint32_memcpy(swap_block, block, 64);
     x = swap_block;
 #endif
 
 
     MD5_ROUND1(a, b, c, d, x[ 0],  7, 0xd76aa478);
     MD5_ROUND1(d, a, b, c, x[ 1], 12, 0xe8c7b756);
     MD5_ROUND1(c, d, a, b, x[ 2], 17, 0x242070db);
     MD5_ROUND1(b, c, d, a, x[ 3], 22, 0xc1bdceee);
     MD5_ROUND1(a, b, c, d, x[ 4],  7, 0xf57c0faf);
     MD5_ROUND1(d, a, b, c, x[ 5], 12, 0x4787c62a);
     MD5_ROUND1(c, d, a, b, x[ 6], 17, 0xa8304613);
     MD5_ROUND1(b, c, d, a, x[ 7], 22, 0xfd469501);
     MD5_ROUND1(a, b, c, d, x[ 8],  7, 0x698098d8);
     MD5_ROUND1(d, a, b, c, x[ 9], 12, 0x8b44f7af);
     MD5_ROUND1(c, d, a, b, x[10], 17, 0xffff5bb1);
     MD5_ROUND1(b, c, d, a, x[11], 22, 0x895cd7be);
     MD5_ROUND1(a, b, c, d, x[12],  7, 0x6b901122);
     MD5_ROUND1(d, a, b, c, x[13], 12, 0xfd987193);
     MD5_ROUND1(c, d, a, b, x[14], 17, 0xa679438e);
     MD5_ROUND1(b, c, d, a, x[15], 22, 0x49b40821);
 
     MD5_ROUND2(a, b, c, d, x[ 1],  5, 0xf61e2562);
     MD5_ROUND2(d, a, b, c, x[ 6],  9, 0xc040b340);
     MD5_ROUND2(c, d, a, b, x[11], 14, 0x265e5a51);
     MD5_ROUND2(b, c, d, a, x[ 0], 20, 0xe9b6c7aa);
     MD5_ROUND2(a, b, c, d, x[ 5],  5, 0xd62f105d);
     MD5_ROUND2(d, a, b, c, x[10],  9,  0x2441453);
     MD5_ROUND2(c, d, a, b, x[15], 14, 0xd8a1e681);
     MD5_ROUND2(b, c, d, a, x[ 4], 20, 0xe7d3fbc8);
     MD5_ROUND2(a, b, c, d, x[ 9],  5, 0x21e1cde6);
     MD5_ROUND2(d, a, b, c, x[14],  9, 0xc33707d6);
     MD5_ROUND2(c, d, a, b, x[ 3], 14, 0xf4d50d87);
     MD5_ROUND2(b, c, d, a, x[ 8], 20, 0x455a14ed);
     MD5_ROUND2(a, b, c, d, x[13],  5, 0xa9e3e905);
     MD5_ROUND2(d, a, b, c, x[ 2],  9, 0xfcefa3f8);
     MD5_ROUND2(c, d, a, b, x[ 7], 14, 0x676f02d9);
     MD5_ROUND2(b, c, d, a, x[12], 20, 0x8d2a4c8a);
 
     MD5_ROUND3(a, b, c, d, x[ 5],  4, 0xfffa3942);
     MD5_ROUND3(d, a, b, c, x[ 8], 11, 0x8771f681);
     MD5_ROUND3(c, d, a, b, x[11], 16, 0x6d9d6122);
     MD5_ROUND3(b, c, d, a, x[14], 23, 0xfde5380c);
     MD5_ROUND3(a, b, c, d, x[ 1],  4, 0xa4beea44);
     MD5_ROUND3(d, a, b, c, x[ 4], 11, 0x4bdecfa9);
     MD5_ROUND3(c, d, a, b, x[ 7], 16, 0xf6bb4b60);
     MD5_ROUND3(b, c, d, a, x[10], 23, 0xbebfbc70);
     MD5_ROUND3(a, b, c, d, x[13],  4, 0x289b7ec6);
     MD5_ROUND3(d, a, b, c, x[ 0], 11, 0xeaa127fa);
     MD5_ROUND3(c, d, a, b, x[ 3], 16, 0xd4ef3085);
     MD5_ROUND3(b, c, d, a, x[ 6], 23,  0x4881d05);
     MD5_ROUND3(a, b, c, d, x[ 9],  4, 0xd9d4d039);
     MD5_ROUND3(d, a, b, c, x[12], 11, 0xe6db99e5);
     MD5_ROUND3(c, d, a, b, x[15], 16, 0x1fa27cf8);
     MD5_ROUND3(b, c, d, a, x[ 2], 23, 0xc4ac5665);
 
     MD5_ROUND4(a, b, c, d, x[ 0],  6, 0xf4292244);
     MD5_ROUND4(d, a, b, c, x[ 7], 10, 0x432aff97);
     MD5_ROUND4(c, d, a, b, x[14], 15, 0xab9423a7);
     MD5_ROUND4(b, c, d, a, x[ 5], 21, 0xfc93a039);
     MD5_ROUND4(a, b, c, d, x[12],  6, 0x655b59c3);
     MD5_ROUND4(d, a, b, c, x[ 3], 10, 0x8f0ccc92);
     MD5_ROUND4(c, d, a, b, x[10], 15, 0xffeff47d);
     MD5_ROUND4(b, c, d, a, x[ 1], 21, 0x85845dd1);
     MD5_ROUND4(a, b, c, d, x[ 8],  6, 0x6fa87e4f);
     MD5_ROUND4(d, a, b, c, x[15], 10, 0xfe2ce6e0);
     MD5_ROUND4(c, d, a, b, x[ 6], 15, 0xa3014314);
     MD5_ROUND4(b, c, d, a, x[13], 21, 0x4e0811a1);
     MD5_ROUND4(a, b, c, d, x[ 4],  6, 0xf7537e82);
     MD5_ROUND4(d, a, b, c, x[11], 10, 0xbd3af235);
     MD5_ROUND4(c, d, a, b, x[ 2], 15, 0x2ad7d2bb);
     MD5_ROUND4(b, c, d, a, x[ 9], 21, 0xeb86d391);
 
     state[0] += a;
     state[1] += b;
     state[2] += c;
     state[3] += d;
 }
 
 
 /*!
 @brief      内部函数，处理数据的前面部分(>64字节的部分)，每次组成一个64字节的block就进行杂凑处理
 @param[out] ctx  算法的context，用于记录一些处理的上下文和结果
 @param[in]  buf  处理的数据，
 @param[in]  size 处理的数据长度
 */
 static void zen_md5_update(md5_ctx *ctx, const unsigned char *buf, size_t size)
 {
     //为什么不是=，因为在某些环境下，可以多次调用zen_md5_update，但这种情况，必须保证前面的调用，每次都没有unprocessed_
     ctx->length_ += size;
 
     //每个处理的块都是64字节
     while (size >= ZEN_MD5_BLOCK_SIZE)
     {
         zen_md5_process_block(ctx->hash_, reinterpret_cast<const uint32_t *>(buf));
         buf  += ZEN_MD5_BLOCK_SIZE;
         size -= ZEN_MD5_BLOCK_SIZE;
     }
 
     ctx->unprocessed_ = size;
 }
 
 
 /*!
 @brief      内部函数，处理数据的末尾部分，我们要拼出最后1个（或者两个）要处理的BLOCK，加上0x80，加上长度进行处理
 @param[in]  ctx    算法的context，用于记录一些处理的上下文和结果
 @param[in]  buf    处理的数据
 @param[in]  size   处理buffer的长度
 @param[out] result 返回的结果，
 */
 static void zen_md5_final(md5_ctx *ctx, const unsigned char *buf, size_t size, unsigned char *result)
 {
     uint32_t message[ZEN_MD5_BLOCK_SIZE / 4];
 
     //保存剩余的数据，我们要拼出最后1个（或者两个）要处理的块，前面的算法保证了，最后一个块肯定小于64个字节
     if (ctx->unprocessed_)
     {
         memcpy(message, buf + size - ctx->unprocessed_, static_cast<size_t>( ctx->unprocessed_));
     }
 
     //得到0x80要添加在的位置（在uint32_t 数组中），
     uint32_t index = ((uint32_t)ctx->length_ & 63) >> 2;
     uint32_t shift = ((uint32_t)ctx->length_ & 3) * 8;
 
     //添加0x80进去，并且把余下的空间补充0
     message[index]   &= ~(0xFFFFFFFF << shift);
     message[index++] ^= 0x80 << shift;
 
     //如果这个block还无法处理，其后面的长度无法容纳长度64bit，那么先处理这个block
     if (index > 14)
     {
         while (index < 16)
         {
             message[index++] = 0;
         }
 
         zen_md5_process_block(ctx->hash_, message);
         index = 0;
     }
 
     //补0
     while (index < 14)
     {
         message[index++] = 0;
     }
 
     //保存长度，注意是bit位的长度,这个问题让我看着郁闷了半天，
     uint64_t data_len = (ctx->length_) << 3;
 
     //注意MD5算法要求的64bit的长度是小头LITTLE-ENDIAN编码，注意下面的比较是!=
 #if ZEN_BYTES_ORDER != ZEN_LITTLE_ENDIAN
     data_len = ZEN_SWAP_UINT64(data_len);
 #endif
 
     message[14] = (uint32_t) (data_len & 0x00000000FFFFFFFF);
     message[15] = (uint32_t) ((data_len & 0xFFFFFFFF00000000ULL) >> 32);
 
     zen_md5_process_block(ctx->hash_, message);
 
     //注意结果是小头党的，在大头的世界要进行转换
 #if ZEN_BYTES_ORDER == ZEN_LITTLE_ENDIAN
     memcpy(result, &ctx->hash_, ZEN_MD5_HASH_SIZE);
 #else
     swap_uint32_memcpy(result, &ctx->hash_, ZEN_MD5_HASH_SIZE);
 #endif
 
 }
 
 
 //计算一个内存数据的MD5值
 unsigned char *ZEN_LIB::md5(const unsigned char *buf,
                             size_t size,
                             unsigned char result[ZEN_MD5_HASH_SIZE])
 {
     assert(result != NULL);
 
     md5_ctx ctx;
     zen_md5_init(&ctx);
     zen_md5_update(&ctx, buf, size);
     zen_md5_final(&ctx, buf, size, result);
     return result;
 }
 
 
 
 
 //================================================================================================
 //SHA1的算法
 
 //每次处理的BLOCK的大小
 static const size_t ZEN_SHA1_BLOCK_SIZE = 64;
 
 //SHA1算法的上下文，保存一些状态，中间数据，结果
 typedef struct sha1_ctx
 {
 
     //处理的数据的长度
     uint64_t length_;
     //还没有处理的数据长度
     uint64_t unprocessed_;
     /* 160-bit algorithm internal hashing state */
     uint32_t hash_[5];
 } sha1_ctx;
 
 //内部函数，SHA1算法的上下文的初始化
 static void zen_sha1_init(sha1_ctx *ctx)
 {
     ctx->length_ = 0;
     ctx->unprocessed_ = 0;
     // 初始化算法的几个常量，魔术数
     ctx->hash_[0] = 0x67452301;
     ctx->hash_[1] = 0xefcdab89;
     ctx->hash_[2] = 0x98badcfe;
     ctx->hash_[3] = 0x10325476;
     ctx->hash_[4] = 0xc3d2e1f0;
 }
 
 
 /*!
 @brief      内部函数，对一个64bit内存块进行摘要(杂凑)处理，
 @param      hash  存放计算hash结果的的数组
 @param      block 要计算的处理得内存块
 */
 static void zen_sha1_process_block(uint32_t hash[5],
                                    const uint32_t block[ZEN_SHA1_BLOCK_SIZE / 4])
 {
     size_t        t;
     uint32_t      wblock[80];
     register uint32_t      a, b, c, d, e, temp;
 
     //SHA1算法处理的内部数据要求是大头党的，在小头的环境转换
 #if ZEN_BYTES_ORDER == ZEN_LITTLE_ENDIAN
     swap_uint32_memcpy(wblock, block, ZEN_SHA1_BLOCK_SIZE);
 #else
     ::memcpy(wblock, block, ZEN_SHA1_BLOCK_SIZE);
 #endif
	 /*cout << "wblock[15]:" << wblock[14] <<";" << wblock[15] << "block[15]:" << block[14]<<";"<< block[15]<< endl;
	 string tmp1((const char *)wblock,ZEN_SHA1_BLOCK_SIZE);
	 cout << "wblock:" << STRING_Helper::HBCStrToHBCStr(tmp1,LW_BINARY_STR,LW_HEX_STR,true) << endl;
	 string tmp2((const char *)block,ZEN_SHA1_BLOCK_SIZE);
	 cout << "block:" << STRING_Helper::HBCStrToHBCStr(tmp2,LW_BINARY_STR,LW_HEX_STR,true) << endl;*/

     //处理
	/* for (int i=0; i<16; i++){
		 cout << "wblock[" << i<< "]:"<< wblock[i];
	 };*/
	 cout << endl;
     for (t = 16; t < 80; t++)
     {
         wblock[t] = ROTL32(wblock[t - 3] ^ wblock[t - 8] ^ wblock[t - 14] ^ wblock[t - 16], 1);
     }
 
     a = hash[0];
     b = hash[1];
     c = hash[2];
     d = hash[3];
     e = hash[4];
	// cout << "zen_sha1_process_block1:" << a << ";" << b << ";" << c << ";" << d << ";" << e << ";" << " ROTL32(a, 5):" << ROTL32(a, 5) << " temp:"  << 
		 //ROTL32(a, 5) + (((c ^ d) & b) ^ d)+ e + wblock[0] + 0x5A827999 << " ROTL32(b, 30)" << ROTL32(b, 30)<<endl ;

     for (t = 0; t < 20; t++)
     {
         /* the following is faster than ((B & C) | ((~B) & D)) */
         temp =  ROTL32(a, 5) + (((c ^ d) & b) ^ d)
                 + e + wblock[t] + 0x5A827999;
         e = d;
         d = c;
         c = ROTL32(b, 30);
         b = a;
         a = temp;
		 //cout << "zen_sha1_process_block2:" << "wblock[" << t << "]" << wblock[t]<< "ROTL32(a, 5):" << ROTL32(a, 5) <<"temp:" << temp << "a:" << a  << ";b:" << b << ";c:" << c << ";d:" << d << ";e:" << e << ";" << endl;
		 //cout << " ROTL32(a, 5):" << ROTL32(a, 5) << " temp:"  << ROTL32(a, 5) + (((c ^ d) & b) ^ d)+ e + wblock[t] + 0x5A827999 << " ROTL32(b, 30)" << ROTL32(b, 30)<<endl ;
     }
	 //cout << "zen_sha1_process_block2:" << a << ";" << b << ";" << c << ";" << d << ";" << e << ";" << endl;
 
     for (t = 20; t < 40; t++)
     {
         temp = ROTL32(a, 5) + (b ^ c ^ d) + e + wblock[t] + 0x6ED9EBA1;
         e = d;
         d = c;
         c = ROTL32(b, 30);
         b = a;
         a = temp;
     }
 
     for (t = 40; t < 60; t++)
     {
         temp = ROTL32(a, 5) + ((b & c) | (b & d) | (c & d))
                + e + wblock[t] + 0x8F1BBCDC;
         e = d;
         d = c;
         c = ROTL32(b, 30);
         b = a;
         a = temp;
     }
 
     for (t = 60; t < 80; t++)
     {
         temp = ROTL32(a, 5) + (b ^ c ^ d) + e + wblock[t] + 0xCA62C1D6;
         e = d;
         d = c;
         c = ROTL32(b, 30);
         b = a;
         a = temp;
     }
 
     hash[0] += a;
     hash[1] += b;
     hash[2] += c;
     hash[3] += d;
     hash[4] += e;
 }
 
 
 /*!
 @brief      内部函数，处理数据的前面部分(>64字节的部分)，每次组成一个64字节的block就进行杂凑处理
 @param      ctx  算法的上下文，记录中间数据，结果等
 @param      msg  要进行计算的数据buffer
 @param      size 长度
 */
 static void zen_sha1_update(sha1_ctx *ctx,
                             const unsigned char *buf, 
                             size_t size)
 {
     //为了让zen_sha1_update可以多次进入，长度可以累计
     ctx->length_ += size;
 
     //每个处理的块都是64字节
     while (size >= ZEN_SHA1_BLOCK_SIZE)
     {
         zen_sha1_process_block(ctx->hash_, reinterpret_cast<const uint32_t *>(buf));
         buf  += ZEN_SHA1_BLOCK_SIZE;
         size -= ZEN_SHA1_BLOCK_SIZE;
     }
 
     ctx->unprocessed_ = size;
 }
 
 
 /*!
 @brief      内部函数，处理数据的最后部分，添加0x80,补0，增加长度信息
 @param      ctx    算法的上下文，记录中间数据，结果等
 @param      msg    要进行计算的数据buffer
 @param      result 返回的结果
 */
 static void zen_sha1_final(sha1_ctx *ctx, 
                            const unsigned char *msg,
                            size_t size, 
                            unsigned char *result)
 {
 
     uint32_t message[ZEN_SHA1_BLOCK_SIZE / 4];
 
     //保存剩余的数据，我们要拼出最后1个（或者两个）要处理的块，前面的算法保证了，最后一个块肯定小于64个字节
     if (ctx->unprocessed_)
     {
         memcpy(message, msg + size - ctx->unprocessed_, static_cast<size_t>( ctx->unprocessed_));
     }
 
     //得到0x80要添加在的位置（在uint32_t 数组中），
     uint32_t index = ((uint32_t)ctx->length_ & 63) >> 2;
     uint32_t shift = ((uint32_t)ctx->length_ & 3) * 8;
 
     //添加0x80进去，并且把余下的空间补充0
     message[index]   &= ~(0xFFFFFFFF << shift);
     message[index++] ^= 0x80 << shift;
 
     //如果这个block还无法处理，其后面的长度无法容纳长度64bit，那么先处理这个block
     if (index > 14)
     {
         while (index < 16)
         {
             message[index++] = 0;
         }
		 
#if ZEN_BYTES_ORDER == ZEN_BIG_ENDIAN
		//ly add only for websocket ack 9331	
		if (message[15]==128){ //今后在此改为针对 big endian 进行自动化交换	swap_uint32_memcpy(wblock, block, ZEN_SHA1_BLOCK_SIZE);
			message[15]=2147483648;
			//cout << "message2[15]:" << message[15] << endl;		
		}
#endif
         zen_sha1_process_block(ctx->hash_, message);
         index = 0;
     }
 
     //补0
     while (index < 14)
     {
         message[index++] = 0;
     }
 
     //保存长度，注意是bit位的长度,这个问题让我看着郁闷了半天，
     uint64_t data_len = (ctx->length_) << 3;
 
     //注意SHA1算法要求的64bit的长度是大头BIG-ENDIAN，在小头的世界要进行转换
 #if ZEN_BYTES_ORDER == ZEN_LITTLE_ENDIAN
     data_len = ZEN_SWAP_UINT64(data_len);
 #endif
 
     message[14] = (uint32_t) (data_len & 0x00000000FFFFFFFF);
     message[15] = (uint32_t) ((data_len & 0xFFFFFFFF00000000ULL) >> 32);

#if ZEN_BYTES_ORDER == ZEN_BIG_ENDIAN
		//ly add : only for websocket ack 9331;
		if (message[14]==480 && message[15]==0){// //今后在此改为针对 big endian 进行自动化交换	swap_uint32_memcpy(wblock, block, ZEN_SHA1_BLOCK_SIZE);
			message[15]=480;
			message[14]=0;
		}
#endif 
     zen_sha1_process_block(ctx->hash_, message);
	 //cout << "final2 ctx:" <<ctx->hash_[0] << ";" <<ctx->hash_[1]<< ";" <<ctx->hash_[2] << ";" <<ctx->hash_[3] << ";" <<ctx->hash_[4] << ";" << endl;
 
     //注意结果是大头党的，在小头的世界要进行转换
 #if ZEN_BYTES_ORDER == ZEN_LITTLE_ENDIAN
     swap_uint32_memcpy(result, &ctx->hash_, ZEN_SHA1_HASH_SIZE);
	 //cout << "little endian!" << endl;
 #else
     memcpy(result, &ctx->hash_, ZEN_SHA1_HASH_SIZE);
	 //cout << "big endia!" << endl;
 #endif
 }
 
 
 
 //计算一个内存数据的SHA1值
 unsigned char *sha1(const unsigned char *msg,size_t size, unsigned char *result)
 {
     assert(result != NULL);
 
     sha1_ctx ctx;
     zen_sha1_init(&ctx);
     zen_sha1_update(&ctx, msg, size);
	 //cout << "update ctx:" <<ctx.hash_[0] << ";" <<ctx.hash_[1] << ";" <<ctx.hash_[2] << ";" <<ctx.hash_[3] << ";" <<ctx.hash_[4] << ";" << endl;
     zen_sha1_final(&ctx, msg, size, result);
	 
	// string tmp_str;
	//cout << "sha1: " << ctx << endl;
     return result;
 }
 /*
SHA1和MD5的算法说明
SHA1和MD5的算法都是从MD4算法改进而来的2种算法，基本思路都是将信息分成N个分组，每组64个字节，每个分组都进行摘要运算。当一个分组的摘要运算完毕后，将上一个分组的结果也用于下一个分组的运算。
信息的长度（注意是bit位长度，不是字节长度）用64位表示，也要参加信息摘要运算，而且是放在最后一个分组的末尾，所以长度信息要占据8个字节。
如果信息数据最后一个分组长度小于64个字节，在后面添加0x80标志结束，如果此时数据+结束标志已经<=56个字节，还可以放入长度数据，就在结束标志到第56个字节补0，然后放入长度，如果此时信息数据+结束标志已经大于56字节，那么这个分组后面补0，进行一次摘要运算，然后再建立一个分组，前面全部补0，最后16个字节放长度，再进行一次摘要。
需要注意的地方如下。
MD5最后生成的摘要信息是16个字节，SHA1是20个字节。
MD5和SHA1的分组信息运算，分组里面的的数据都会被视为16个DWORD，而MD5算法认为这些DWORD的字节序列是LITTLE-ENDIAN,而SHA1的算法认为DWORD是BIG-ENDIAN的。所以在不同字节序的主机上要进行转换。
放入最后一个分组的长度信息，是原始数据长度，而且是BIT位长度，其是一个uint64_t，而MD5算法要求放入的长度是LITTLE-ENDIAN的，而SHA1算法则要求这个长度是BIG-ENDIAN的。不同的平台要进行转换。
当然生成的结果，MD5也要求是LITTLE-ENDIAN，SHA1也要求结果是BIG-ENDIAN的，不同的平台还是要进行转换。
我们贴几个摘要处理过程的分组信息，帮助大家理解。如果要处理的数据是3个字节字符串”abc”，其在MD5的算法中，只需要一个分组参加，数据是16进制，如下：
61 62 63 80 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 18 00 00 00 00 00 00 00
而SHA1算法中，也只有一个分组，如下，大家注意长度位置上的差别。十六进制的18标识24个bit3个字节。
61 62 63 80 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 18
如果要处理的数据是80个字节的"12345678901234567890123456789012345678901234567890123456789012345678901234567890"，其在MD5的算法会被分成2个分组，
第一个分组如下，
31 32 33 34 35 36 37 38 39 30 31 32 33 34 35 36
37 38 39 30 31 32 33 34 35 36 37 38 39 30 31 32
33 34 35 36 37 38 39 30 31 32 33 34 35 36 37 38
39 30 31 32 33 34 35 36 37 38 39 30 31 32 33 34
第二个分组如下
35 36 37 38 39 30 31 32 33 34 35 36 37 38 39 30
80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00 80 02 00 00 00 00 00 00
 */
