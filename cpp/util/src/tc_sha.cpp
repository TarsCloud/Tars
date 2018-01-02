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

#include "util/tc_sha.h"
#include "util/tc_common.h"
#include <stdio.h>
#include <string.h>
#include <endian.h>
#include <limits.h>

namespace tars
{

/*  Defines for suffixes to 32 and 64 bit unsigned numeric values   */
#define sfx_lo(x,y) x##y
#define sfx_hi(x,y) sfx_lo(x,y)
#define n_u32(p)    sfx_hi(0x##p, s_u32)
#define n_u64(p)    sfx_hi(0x##p, s_u64)

#if UINT_MAX == 0xffffffff
  #define s_u32    u
#elif ULONG_MAX == 0xffffffff
  #define s_u32   ul
#else
#error Please define sha2_32t as an unsigned 32 bit type in sha2.h
#endif

#if ULONG_MAX == 0xffffffffffffffff
  #define s_u64   ul
#elif ULONG_MAX == 0xffffffff
  #define s_u64  ull
#else
#error Please define sha2_64t as an unsigned 64 bit type in sha2.h
#endif

#define ch(x,y,z)       (((x) & (y)) ^ (~(x) & (z)))
#define parity(x,y,z)   ((x) ^ (y) ^ (z))
#define maj(x,y,z)      (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

namespace detail_sha1
{

#define SHA1_BLOCK_SIZE  64
#define SHA1_DIGEST_SIZE 20

typedef struct
{
    uint32_t count[2];
    uint32_t hash[5];
    uint32_t wbuf[16];
} sha1_ctx;

#define rotr32_sha1(x,n) (((x) << n) | ((x) >> (32 - n)))

#define SHA1_MASK   (SHA1_BLOCK_SIZE - 1)

#if(__BYTE_ORDER == __BIG_ENDIAN)
#define swap_b32_sha1(x) (x)
#elif defined(bswap_32_sha1)
#define swap_b32_sha1(x) bswap_32_sha1(x)
#else
#define swap_b32_sha1(x) ((rotr32_sha1((x), 8) & 0x00ff00ff) | (rotr32_sha1((x), 24) & 0xff00ff00))
#endif

/* A normal version as set out in the FIPS. This version uses   */
/* partial loop unrolling and is optimised for the Pentium 4    */

#define rnd(f,k)    \
    t = a; a = rotr32_sha1(a,5) + f(b,c,d) + e + k + w[i]; \
    e = d; d = c; c = rotr32_sha1(b, 30); b = t

void sha1_compile(sha1_ctx ctx[1])
{   uint32_t    w[80], i, a, b, c, d, e, t;

    /* note that words are compiled from the buffer into 32-bit */
    /* words in big-endian order so an order reversal is needed */
    /* here on little endian machines                           */
    for (i = 0; i < SHA1_BLOCK_SIZE / 4; ++i) w[i] = swap_b32_sha1(ctx->wbuf[i]);

    for (i = SHA1_BLOCK_SIZE / 4; i < 80; ++i) w[i] = rotr32_sha1(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);

    a = ctx->hash[0];
    b = ctx->hash[1];
    c = ctx->hash[2];
    d = ctx->hash[3];
    e = ctx->hash[4];

    for (i = 0; i < 20; ++i)
    {
        rnd(ch, 0x5a827999);
    }

    for (i = 20; i < 40; ++i)
    {
        rnd(parity, 0x6ed9eba1);
    }

    for (i = 40; i < 60; ++i)
    {
        rnd(maj, 0x8f1bbcdc);
    }

    for (i = 60; i < 80; ++i)
    {
        rnd(parity, 0xca62c1d6);
    }

    ctx->hash[0] += a;
    ctx->hash[1] += b;
    ctx->hash[2] += c;
    ctx->hash[3] += d;
    ctx->hash[4] += e;
}

void sha1_begin(sha1_ctx ctx[1])
{
    ctx->count[0] = ctx->count[1] = 0;
    ctx->hash[0] = 0x67452301;
    ctx->hash[1] = 0xefcdab89;
    ctx->hash[2] = 0x98badcfe;
    ctx->hash[3] = 0x10325476;
    ctx->hash[4] = 0xc3d2e1f0;
}

/* SHA1 hash data in an array of bytes into hash buffer and call the        */
/* hash_compile function as required.                                       */

void sha1_hash(const unsigned char data[], unsigned int len, sha1_ctx ctx[1])
{   
    uint32_t pos = (uint32_t)(ctx->count[0] & SHA1_MASK),
        space = SHA1_BLOCK_SIZE - pos;
    const unsigned char *sp = data;

    if ((ctx->count[0] += len) < len) ++(ctx->count[1]);

    while (len >= space)     /* tranfer whole blocks while possible  */
    {
        memcpy(((unsigned char *)ctx->wbuf) + pos, sp, space);
        sp += space; len -= space; space = SHA1_BLOCK_SIZE; pos = 0;
        sha1_compile(ctx);
    }

    memcpy(((unsigned char *)ctx->wbuf) + pos, sp, len);
}

/* SHA1 final padding and digest calculation  */
#if(__BYTE_ORDER == __LITTLE_ENDIAN)
static uint32_t  mask[4] = {   0x00000000, 0x000000ff, 0x0000ffff, 0x00ffffff };
static uint32_t  bits[4] = {   0x00000080, 0x00008000, 0x00800000, 0x80000000 };
#else
static uint32_t  mask[4] = {   0x00000000, 0xff000000, 0xffff0000, 0xffffff00 };
static uint32_t  bits[4] = {   0x80000000, 0x00800000, 0x00008000, 0x00000080 };
#endif

void sha1_end(unsigned char hval[], sha1_ctx ctx[1])
{
    uint32_t i = (uint32_t)(ctx->count[0] & SHA1_MASK);

    /* mask out the rest of any partial 32-bit word and then set    */
    /* the next byte to 0x80. On big-endian machines any bytes in   */
    /* the buffer will be at the top end of 32 bit words, on little */
    /* endian machines they will be at the bottom. Hence the AND    */
    /* and OR masks above are reversed for little endian systems    */
    /* Note that we can always add the first padding byte at this   */
    /* because the buffer always contains at least one empty slot   */
    ctx->wbuf[i >> 2] = (ctx->wbuf[i >> 2] & mask[i & 3]) | bits[i & 3];

    /* we need 9 or more empty positions, one for the padding byte  */
    /* (above) and eight for the length count.  If there is not     */
    /* enough space pad and empty the buffer                        */
    if (i > SHA1_BLOCK_SIZE - 9)
    {
        if (i < 60) ctx->wbuf[15] = 0;
        sha1_compile(ctx);
        i = 0;
    }
    else    /* compute a word index for the empty buffer positions  */
        i = (i >> 2) + 1;

    while (i < 14) /* and zero pad all but last two positions      */
        ctx->wbuf[i++] = 0;

    /* assemble the eight byte counter in in big-endian format      */
    ctx->wbuf[14] = swap_b32_sha1((ctx->count[1] << 3) | (ctx->count[0] >> 29));
    ctx->wbuf[15] = swap_b32_sha1(ctx->count[0] << 3);

    sha1_compile(ctx);

    /* extract the hash value as bytes in case the hash buffer is   */
    /* misaligned for 32-bit words                                  */
    for (i = 0; i < SHA1_DIGEST_SIZE; ++i) hval[i] = (unsigned char)(ctx->hash[i >> 2] >> 8 * (~i & 3));
}

}

//////////////////////////////////////////////////////////////////////////////////////////////////////

namespace detail_sha2
{

#define SHA256_DIGEST_SIZE  32
#define SHA384_DIGEST_SIZE  48
#define SHA512_DIGEST_SIZE  64

#define SHA256_BLOCK_SIZE   64
#define SHA384_BLOCK_SIZE  128
#define SHA512_BLOCK_SIZE  128

/* type to hold the SHA256 context              */

typedef struct
{uint32_t count[2];
    uint32_t hash[8];
    uint32_t wbuf[16];
} sha256_ctx;

/* type to hold the SHA384/512 context          */

typedef struct
{uint64_t count[2];
    uint64_t hash[8];
    uint64_t wbuf[16];
} sha512_ctx;

typedef sha512_ctx  sha384_ctx;

#define rotr32_sha2(x,n) (((x) >> n) | ((x) << (32 - n)))
#define rotr64_sha2(x,n) (((x) >> n) | ((x) << (64 - n)))

#define SHA256_MASK (SHA256_BLOCK_SIZE - 1)
#define SHA512_MASK (SHA512_BLOCK_SIZE - 1)

#if !defined(bswap_32_sha2)
#define bswap_32_sha2(x) ((rotr32_sha2((x), 24) & 0x00ff00ff) | (rotr32_sha2((x), 8) & 0xff00ff00))
#endif

#if(__BYTE_ORDER == __LITTLE_ENDIAN)
#define bsw_32_sha2(p,n) { int _i = (n); while(_i--) p[_i] = bswap_32_sha2(p[_i]); }
#else
#define bsw_32_sha2(p,n)
#endif

#if(__BYTE_ORDER == __LITTLE_ENDIAN)
  #if !defined(bswap_64_sha2)
  #define bswap_64_sha2(x) (((uint64_t)(bswap_32_sha2((uint32_t)(x)))) << 32 | bswap_32_sha2((uint32_t)((x) >> 32)))
  #endif
#define bsw_64_sha2(p,n) { int _i = (n); while(_i--) p[_i] = bswap_64_sha2(p[_i]); }
#else
#define bsw_64_sha2(p,n)
#endif

/* SHA256 mixing function definitions   */

#define s256_0(x) (rotr32_sha2((x),  2) ^ rotr32_sha2((x), 13) ^ rotr32_sha2((x), 22))
#define s256_1(x) (rotr32_sha2((x),  6) ^ rotr32_sha2((x), 11) ^ rotr32_sha2((x), 25))
#define g256_0(x) (rotr32_sha2((x),  7) ^ rotr32_sha2((x), 18) ^ ((x) >>  3))
#define g256_1(x) (rotr32_sha2((x), 17) ^ rotr32_sha2((x), 19) ^ ((x) >> 10))

/* rotated SHA256 round definition. Rather than swapping variables as in    */
/* FIPS-180, different variables are 'rotated' on each round, returning     */
/* to their starting positions every eight rounds                           */

#define h2(i) ctx->wbuf[i & 15] += \
    g256_1(ctx->wbuf[(i + 14) & 15]) + ctx->wbuf[(i + 9) & 15] + g256_0(ctx->wbuf[(i + 1) & 15])

#define h2_cycle(i,j)  \
    v[(7 - i) & 7] += (j ? h2(i) : ctx->wbuf[i & 15]) + k256[i + j] \
        + s256_1(v[(4 - i) & 7]) + ch(v[(4 - i) & 7], v[(5 - i) & 7], v[(6 - i) & 7]); \
    v[(3 - i) & 7] += v[(7 - i) & 7]; \
    v[(7 - i) & 7] += s256_0(v[(0 - i) & 7]) + maj(v[(0 - i) & 7], v[(1 - i) & 7], v[(2 - i) & 7])

/* SHA256 mixing data   */

const uint32_t k256[64] =
{   n_u32(428a2f98), n_u32(71374491), n_u32(b5c0fbcf), n_u32(e9b5dba5),
    n_u32(3956c25b), n_u32(59f111f1), n_u32(923f82a4), n_u32(ab1c5ed5),
    n_u32(d807aa98), n_u32(12835b01), n_u32(243185be), n_u32(550c7dc3),
    n_u32(72be5d74), n_u32(80deb1fe), n_u32(9bdc06a7), n_u32(c19bf174),
    n_u32(e49b69c1), n_u32(efbe4786), n_u32(0fc19dc6), n_u32(240ca1cc),
    n_u32(2de92c6f), n_u32(4a7484aa), n_u32(5cb0a9dc), n_u32(76f988da),
    n_u32(983e5152), n_u32(a831c66d), n_u32(b00327c8), n_u32(bf597fc7),
    n_u32(c6e00bf3), n_u32(d5a79147), n_u32(06ca6351), n_u32(14292967),
    n_u32(27b70a85), n_u32(2e1b2138), n_u32(4d2c6dfc), n_u32(53380d13),
    n_u32(650a7354), n_u32(766a0abb), n_u32(81c2c92e), n_u32(92722c85),
    n_u32(a2bfe8a1), n_u32(a81a664b), n_u32(c24b8b70), n_u32(c76c51a3),
    n_u32(d192e819), n_u32(d6990624), n_u32(f40e3585), n_u32(106aa070),
    n_u32(19a4c116), n_u32(1e376c08), n_u32(2748774c), n_u32(34b0bcb5),
    n_u32(391c0cb3), n_u32(4ed8aa4a), n_u32(5b9cca4f), n_u32(682e6ff3),
    n_u32(748f82ee), n_u32(78a5636f), n_u32(84c87814), n_u32(8cc70208),
    n_u32(90befffa), n_u32(a4506ceb), n_u32(bef9a3f7), n_u32(c67178f2),
};

/* SHA256 initialisation data */

const uint32_t i256[8] =
{
    n_u32(6a09e667), n_u32(bb67ae85), n_u32(3c6ef372), n_u32(a54ff53a),
    n_u32(510e527f), n_u32(9b05688c), n_u32(1f83d9ab), n_u32(5be0cd19)
};

void sha256_begin(sha256_ctx ctx[1])
{
    ctx->count[0] = ctx->count[1] = 0;
    memcpy(ctx->hash, i256, 8 * sizeof(uint32_t));
}

/* Compile 64 bytes of hash data into SHA256 digest value   */
/* NOTE: this routine assumes that the byte order in the    */
/* ctx->wbuf[] at this point is in such an order that low   */
/* address bytes in the ORIGINAL byte stream placed in this */
/* buffer will now go to the high end of words on BOTH big  */
/* and little endian systems                                */

void sha256_compile(sha256_ctx ctx[1])
{   uint32_t    v[8], j;

    memcpy(v, ctx->hash, 8 * sizeof(uint32_t));

    for (j = 0; j < 64; j += 16)
    {
        h2_cycle(0, j); h2_cycle(1, j); h2_cycle(2, j); h2_cycle(3, j);
        h2_cycle(4, j); h2_cycle(5, j); h2_cycle(6, j); h2_cycle(7, j);
        h2_cycle(8, j); h2_cycle(9, j); h2_cycle(10, j); h2_cycle(11, j);
        h2_cycle(12, j); h2_cycle(13, j); h2_cycle(14, j); h2_cycle(15, j);
    }

    ctx->hash[0] += v[0]; ctx->hash[1] += v[1]; ctx->hash[2] += v[2]; ctx->hash[3] += v[3];
    ctx->hash[4] += v[4]; ctx->hash[5] += v[5]; ctx->hash[6] += v[6]; ctx->hash[7] += v[7];
}

/* SHA256 hash data in an array of bytes into hash buffer   */
/* and call the hash_compile function as required.          */

void sha256_hash(const unsigned char data[], unsigned long len, sha256_ctx ctx[1])
{   uint32_t pos = (uint32_t)(ctx->count[0] & SHA256_MASK),
    space = SHA256_BLOCK_SIZE - pos;
    const unsigned char *sp = data;

    if ((ctx->count[0] += len) < len) ++(ctx->count[1]);

    while (len >= space)     /* tranfer whole blocks while possible  */
    {
        memcpy(((unsigned char *)ctx->wbuf) + pos, sp, space);
        sp += space; len -= space; space = SHA256_BLOCK_SIZE; pos = 0;
        bsw_32_sha2(ctx->wbuf, SHA256_BLOCK_SIZE >> 2);
        sha256_compile(ctx);
    }

    memcpy(((unsigned char *)ctx->wbuf) + pos, sp, len);
}

/* SHA256 Final padding and digest calculation  */

static uint32_t  m1[4] =
{
    n_u32(00000000), n_u32(ff000000), n_u32(ffff0000), n_u32(ffffff00)
};

static uint32_t  b1[4] =
{
    n_u32(80000000), n_u32(00800000), n_u32(00008000), n_u32(00000080)
};

void sha256_end(unsigned char hval[], sha256_ctx ctx[1])
{   
    uint32_t    i = (uint32_t)(ctx->count[0] & SHA256_MASK);

    bsw_32_sha2(ctx->wbuf, (i + 3) >> 2);
    /* bytes in the buffer are now in an order in which references  */
    /* to 32-bit words will put bytes with lower addresses into the */
    /* top of 32 bit words on BOTH big and little endian machines   */

    /* we now need to mask valid bytes and add the padding which is */
    /* a single 1 bit and as many zero bits as necessary.           */
    ctx->wbuf[i >> 2] = (ctx->wbuf[i >> 2] & m1[i & 3]) | b1[i & 3];

    /* we need 9 or more empty positions, one for the padding byte  */
    /* (above) and eight for the length count.  If there is not     */
    /* enough space pad and empty the buffer                        */
    if (i > SHA256_BLOCK_SIZE - 9)
    {
        if (i < 60) ctx->wbuf[15] = 0;
        sha256_compile(ctx);
        i = 0;
    }
    else    /* compute a word index for the empty buffer positions  */
        i = (i >> 2) + 1;

    while (i < 14) /* and zero pad all but last two positions      */
        ctx->wbuf[i++] = 0;

    /* the following 32-bit length fields are assembled in the      */
    /* wrong byte order on little endian machines but this is       */
    /* corrected later since they are only ever used as 32-bit      */
    /* word values.                                                 */

    ctx->wbuf[14] = (ctx->count[1] << 3) | (ctx->count[0] >> 29);
    ctx->wbuf[15] = ctx->count[0] << 3;

    sha256_compile(ctx);

    /* extract the hash value as bytes in case the hash buffer is   */
    /* mislaigned for 32-bit words                                  */
    for (i = 0; i < SHA256_DIGEST_SIZE; ++i) hval[i] = (unsigned char)(ctx->hash[i >> 2] >> 8 * (~i & 3));
}

/* SHA512 mixing function definitions   */

#define s512_0(x) (rotr64_sha2((x), 28) ^ rotr64_sha2((x), 34) ^ rotr64_sha2((x), 39))
#define s512_1(x) (rotr64_sha2((x), 14) ^ rotr64_sha2((x), 18) ^ rotr64_sha2((x), 41))
#define g512_0(x) (rotr64_sha2((x),  1) ^ rotr64_sha2((x),  8) ^ ((x) >>  7))
#define g512_1(x) (rotr64_sha2((x), 19) ^ rotr64_sha2((x), 61) ^ ((x) >>  6))

/* rotated SHA512 round definition. Rather than swapping variables as in    */
/* FIPS-180, different variables are 'rotated' on each round, returning     */
/* to their starting positions every eight rounds                           */

#define h5(i) ctx->wbuf[i & 15] += \
    g512_1(ctx->wbuf[(i + 14) & 15]) + ctx->wbuf[(i + 9) & 15] + g512_0(ctx->wbuf[(i + 1) & 15])

#define h5_cycle(i,j)  \
    v[(7 - i) & 7] += (j ? h5(i) : ctx->wbuf[i & 15]) + k512[i + j] \
        + s512_1(v[(4 - i) & 7]) + ch(v[(4 - i) & 7], v[(5 - i) & 7], v[(6 - i) & 7]); \
    v[(3 - i) & 7] += v[(7 - i) & 7]; \
    v[(7 - i) & 7] += s512_0(v[(0 - i) & 7]) + maj(v[(0 - i) & 7], v[(1 - i) & 7], v[(2 - i) & 7])

/* SHA384/SHA512 mixing data    */

const uint64_t  k512[80] =
{
    n_u64(428a2f98d728ae22), n_u64(7137449123ef65cd),
    n_u64(b5c0fbcfec4d3b2f), n_u64(e9b5dba58189dbbc),
    n_u64(3956c25bf348b538), n_u64(59f111f1b605d019),
    n_u64(923f82a4af194f9b), n_u64(ab1c5ed5da6d8118),
    n_u64(d807aa98a3030242), n_u64(12835b0145706fbe),
    n_u64(243185be4ee4b28c), n_u64(550c7dc3d5ffb4e2),
    n_u64(72be5d74f27b896f), n_u64(80deb1fe3b1696b1),
    n_u64(9bdc06a725c71235), n_u64(c19bf174cf692694),
    n_u64(e49b69c19ef14ad2), n_u64(efbe4786384f25e3),
    n_u64(0fc19dc68b8cd5b5), n_u64(240ca1cc77ac9c65),
    n_u64(2de92c6f592b0275), n_u64(4a7484aa6ea6e483),
    n_u64(5cb0a9dcbd41fbd4), n_u64(76f988da831153b5),
    n_u64(983e5152ee66dfab), n_u64(a831c66d2db43210),
    n_u64(b00327c898fb213f), n_u64(bf597fc7beef0ee4),
    n_u64(c6e00bf33da88fc2), n_u64(d5a79147930aa725),
    n_u64(06ca6351e003826f), n_u64(142929670a0e6e70),
    n_u64(27b70a8546d22ffc), n_u64(2e1b21385c26c926),
    n_u64(4d2c6dfc5ac42aed), n_u64(53380d139d95b3df),
    n_u64(650a73548baf63de), n_u64(766a0abb3c77b2a8),
    n_u64(81c2c92e47edaee6), n_u64(92722c851482353b),
    n_u64(a2bfe8a14cf10364), n_u64(a81a664bbc423001),
    n_u64(c24b8b70d0f89791), n_u64(c76c51a30654be30),
    n_u64(d192e819d6ef5218), n_u64(d69906245565a910),
    n_u64(f40e35855771202a), n_u64(106aa07032bbd1b8),
    n_u64(19a4c116b8d2d0c8), n_u64(1e376c085141ab53),
    n_u64(2748774cdf8eeb99), n_u64(34b0bcb5e19b48a8),
    n_u64(391c0cb3c5c95a63), n_u64(4ed8aa4ae3418acb),
    n_u64(5b9cca4f7763e373), n_u64(682e6ff3d6b2b8a3),
    n_u64(748f82ee5defb2fc), n_u64(78a5636f43172f60),
    n_u64(84c87814a1f0ab72), n_u64(8cc702081a6439ec),
    n_u64(90befffa23631e28), n_u64(a4506cebde82bde9),
    n_u64(bef9a3f7b2c67915), n_u64(c67178f2e372532b),
    n_u64(ca273eceea26619c), n_u64(d186b8c721c0c207),
    n_u64(eada7dd6cde0eb1e), n_u64(f57d4f7fee6ed178),
    n_u64(06f067aa72176fba), n_u64(0a637dc5a2c898a6),
    n_u64(113f9804bef90dae), n_u64(1b710b35131c471b),
    n_u64(28db77f523047d84), n_u64(32caab7b40c72493),
    n_u64(3c9ebe0a15c9bebc), n_u64(431d67c49c100d4c),
    n_u64(4cc5d4becb3e42b6), n_u64(597f299cfc657e2a),
    n_u64(5fcb6fab3ad6faec), n_u64(6c44198c4a475817)
};

/* Compile 64 bytes of hash data into SHA384/SHA512 digest value  */

void sha512_compile(sha512_ctx ctx[1])
{   uint64_t    v[8];
    uint32_t    j;

    memcpy(v, ctx->hash, 8 * sizeof(uint64_t));

    for (j = 0; j < 80; j += 16)
    {
        h5_cycle(0, j); h5_cycle(1, j); h5_cycle(2, j); h5_cycle(3, j);
        h5_cycle(4, j); h5_cycle(5, j); h5_cycle(6, j); h5_cycle(7, j);
        h5_cycle(8, j); h5_cycle(9, j); h5_cycle(10, j); h5_cycle(11, j);
        h5_cycle(12, j); h5_cycle(13, j); h5_cycle(14, j); h5_cycle(15, j);
    }

    ctx->hash[0] += v[0]; ctx->hash[1] += v[1]; ctx->hash[2] += v[2]; ctx->hash[3] += v[3];
    ctx->hash[4] += v[4]; ctx->hash[5] += v[5]; ctx->hash[6] += v[6]; ctx->hash[7] += v[7];
}

/* Compile 128 bytes of hash data into SHA256 digest value  */
/* NOTE: this routine assumes that the byte order in the    */
/* ctx->wbuf[] at this point is in such an order that low   */
/* address bytes in the ORIGINAL byte stream placed in this */
/* buffer will now go to the high end of words on BOTH big  */
/* and little endian systems                                */

void sha512_hash(const unsigned char data[], unsigned long len, sha512_ctx ctx[1])
{   
    uint32_t pos = (uint32_t)(ctx->count[0] & SHA512_MASK),
    space = SHA512_BLOCK_SIZE - pos;
    const unsigned char *sp = data;

    if ((ctx->count[0] += len) < len) ++(ctx->count[1]);

    while (len >= space)     /* tranfer whole blocks while possible  */
    {
        memcpy(((unsigned char *)ctx->wbuf) + pos, sp, space);
        sp += space; len -= space; space = SHA512_BLOCK_SIZE; pos = 0;
        bsw_64_sha2(ctx->wbuf, SHA512_BLOCK_SIZE >> 3);
        sha512_compile(ctx);
    }

    memcpy(((unsigned char *)ctx->wbuf) + pos, sp, len);
}

/* SHA384/512 Final padding and digest calculation  */

static uint64_t  m2[8] =
{
    n_u64(0000000000000000), n_u64(ff00000000000000),
    n_u64(ffff000000000000), n_u64(ffffff0000000000),
    n_u64(ffffffff00000000), n_u64(ffffffffff000000),
    n_u64(ffffffffffff0000), n_u64(ffffffffffffff00)
};

static uint64_t  b2[8] =
{
    n_u64(8000000000000000), n_u64(0080000000000000),
    n_u64(0000800000000000), n_u64(0000008000000000),
    n_u64(0000000080000000), n_u64(0000000000800000),
    n_u64(0000000000008000), n_u64(0000000000000080)
};

static void sha_end(unsigned char hval[], sha512_ctx ctx[1], const unsigned int hlen)
{   
    uint32_t    i = (uint32_t)(ctx->count[0] & SHA512_MASK);

    bsw_64_sha2(ctx->wbuf, (i + 7) >> 3);

    /* bytes in the buffer are now in an order in which references  */
    /* to 64-bit words will put bytes with lower addresses into the */
    /* top of 64 bit words on BOTH big and little endian machines   */

    /* we now need to mask valid bytes and add the padding which is */
    /* a single 1 bit and as many zero bits as necessary.           */
    ctx->wbuf[i >> 3] = (ctx->wbuf[i >> 3] & m2[i & 7]) | b2[i & 7];

    /* we need 17 or more empty byte positions, one for the padding */
    /* byte (above) and sixteen for the length count.  If there is  */
    /* not enough space pad and empty the buffer                    */
    if (i > SHA512_BLOCK_SIZE - 17)
    {
        if (i < 120) ctx->wbuf[15] = 0;
        sha512_compile(ctx);
        i = 0;
    }
    else i = (i >> 3) + 1;

    while (i < 14) ctx->wbuf[i++] = 0;

    /* the following 64-bit length fields are assembled in the      */
    /* wrong byte order on little endian machines but this is       */
    /* corrected later since they are only ever used as 64-bit      */
    /* word values.                                                 */

    ctx->wbuf[14] = (ctx->count[1] << 3) | (ctx->count[0] >> 61);
    ctx->wbuf[15] = ctx->count[0] << 3;

    sha512_compile(ctx);

    /* extract the hash value as bytes in case the hash buffer is   */
    /* misaligned for 32-bit words                                  */
    for (i = 0; i < hlen; ++i) hval[i] = (unsigned char)(ctx->hash[i >> 3] >> 8 * (~i & 7));
}

///////////////////////////////////////////////////////////////////////////////////////////////
/* SHA384 initialisation data   */

const uint64_t  i384[80] =
{
    n_u64(cbbb9d5dc1059ed8), n_u64(629a292a367cd507),
    n_u64(9159015a3070dd17), n_u64(152fecd8f70e5939),
    n_u64(67332667ffc00b31), n_u64(8eb44a8768581511),
    n_u64(db0c2e0d64f98fa7), n_u64(47b5481dbefa4fa4)
};

void sha384_begin(sha384_ctx ctx[1])
{
    ctx->count[0] = ctx->count[1] = 0;
    memcpy(ctx->hash, i384, 8 * sizeof(uint64_t));
}

void sha384_end(unsigned char hval[], sha384_ctx ctx[1])
{
    sha_end(hval, ctx, SHA384_DIGEST_SIZE);
}

///////////////////////////////////////////////////////////////////////////////////////////////

/* SHA512 initialisation data   */

const uint64_t  i512[80] =
{
    n_u64(6a09e667f3bcc908), n_u64(bb67ae8584caa73b),
    n_u64(3c6ef372fe94f82b), n_u64(a54ff53a5f1d36f1),
    n_u64(510e527fade682d1), n_u64(9b05688c2b3e6c1f),
    n_u64(1f83d9abfb41bd6b), n_u64(5be0cd19137e2179)
};

void sha512_begin(sha512_ctx ctx[1])
{
    ctx->count[0] = ctx->count[1] = 0;
    memcpy(ctx->hash, i512, 8 * sizeof(uint64_t));
}

void sha512_end(unsigned char hval[], sha512_ctx ctx[1])
{
    sha_end(hval, ctx, SHA512_DIGEST_SIZE);
}

}//detail_sha2

//////////////////////////////////////////////////////////////////////////////////////////////////////

vector<char> TC_SHA::sha1bin(const char *data, size_t len)
{
    vector<char> digest;
    digest.resize(SHA1_DIGEST_SIZE);

    detail_sha1::sha1_ctx cx[1];

    detail_sha1::sha1_begin(cx);
    detail_sha1::sha1_hash((unsigned char *)data, len, cx);
    detail_sha1::sha1_end((unsigned char *)&digest[0], cx);

    return digest;
}

string TC_SHA::sha1str(const char *buffer, size_t length)
{
    vector<char> digest =  sha1bin(buffer, length);

    return TC_Common::bin2str(&digest[0], digest.size());
}

string TC_SHA::sha1file(const string &fileName)
{
    unsigned char sOutBuffer[SHA1_DIGEST_SIZE];
    unsigned char buf[16*1024];
    FILE *f;
    size_t n;
    detail_sha1::sha1_ctx cx[1];
    if(( f = fopen( fileName.c_str(), "rb" )) == NULL )
        throw TC_SHA_Exception("[TC_SHA::sha1file] fopen '" + fileName + "', error.", errno);

    detail_sha1::sha1_begin(cx);
    while((n = fread( buf, 1, sizeof( buf ),f)) > 0 )
        detail_sha1::sha1_hash(buf, n, cx);

    detail_sha1::sha1_end (sOutBuffer, cx);

    fclose(f);

    return TC_Common::bin2str(sOutBuffer, sizeof(sOutBuffer));
}

vector<char> TC_SHA::sha256bin(const char *data, size_t len)
{
    vector<char> digest;
    digest.resize(SHA256_DIGEST_SIZE);

    detail_sha2::sha256_ctx cx[1];

    detail_sha2::sha256_begin(cx);
    detail_sha2::sha256_hash((unsigned char *)data, len, cx);
    detail_sha2::sha256_end((unsigned char *)&digest[0], cx);

    return digest;
}

string TC_SHA::sha256str(const char *buffer, size_t length)
{
    vector<char> digest =  sha256bin(buffer, length);

    return TC_Common::bin2str(&digest[0], digest.size());
}

string TC_SHA::sha256file(const string &fileName)
{
    unsigned char sOutBuffer[SHA256_DIGEST_SIZE];
    unsigned char buf[16*1024];
    FILE *f;
    size_t n;
    detail_sha2::sha256_ctx cx[1];
    if(( f = fopen( fileName.c_str(), "rb" )) == NULL )
        throw TC_SHA_Exception("[TC_SHA::sha256file] fopen '" + fileName + "', error.", errno);

    detail_sha2::sha256_begin(cx);
    while((n = fread( buf, 1, sizeof( buf ),f)) > 0 )
        detail_sha2::sha256_hash(buf, n, cx);

    detail_sha2::sha256_end(sOutBuffer, cx);

    fclose(f);

    return TC_Common::bin2str(sOutBuffer, sizeof(sOutBuffer));
}

vector<char> TC_SHA::sha384bin(const char *data, size_t len)
{
    vector<char> digest;
    digest.resize(SHA384_DIGEST_SIZE);

    detail_sha2::sha384_ctx cx[1];

    detail_sha2::sha384_begin(cx);
    detail_sha2::sha512_hash((unsigned char *)data, len, cx);
    detail_sha2::sha384_end((unsigned char *)&digest[0], cx);

    return digest;
}

string TC_SHA::sha384str(const char *buffer, size_t length)
{
    vector<char> digest =  sha384bin(buffer, length);

    return TC_Common::bin2str(&digest[0], digest.size());
}

string TC_SHA::sha384file(const string &fileName)
{
    unsigned char sOutBuffer[SHA384_DIGEST_SIZE];
    unsigned char buf[16*1024];
    FILE *f;
    size_t n;
    detail_sha2::sha384_ctx cx[1];
    if(( f = fopen( fileName.c_str(), "rb" )) == NULL )
        throw TC_SHA_Exception("[TC_SHA::sha384file] fopen '" + fileName + "', error.", errno);

    detail_sha2::sha384_begin(cx);
    while((n = fread( buf, 1, sizeof( buf ),f)) > 0 )
        detail_sha2::sha512_hash(buf, n, cx);

    detail_sha2::sha384_end(sOutBuffer, cx);

    fclose(f);

    return TC_Common::bin2str(sOutBuffer, sizeof(sOutBuffer));
}

vector<char> TC_SHA::sha512bin(const char *data, size_t len)
{
    vector<char> digest;
    digest.resize(SHA512_DIGEST_SIZE);

    detail_sha2::sha512_ctx cx[1];

    detail_sha2::sha512_begin(cx);
    detail_sha2::sha512_hash((unsigned char *)data, len, cx);
    detail_sha2::sha512_end((unsigned char *)&digest[0], cx);

    return digest;
}

string TC_SHA::sha512str(const char *buffer, size_t length)
{
    vector<char> digest =  sha512bin(buffer, length);

    return TC_Common::bin2str(&digest[0], digest.size());
}

string TC_SHA::sha512file(const string &fileName)
{
    unsigned char sOutBuffer[SHA512_DIGEST_SIZE];
    unsigned char buf[16*1024];
    FILE *f;
    size_t n;
    detail_sha2::sha512_ctx cx[1];
    if(( f = fopen( fileName.c_str(), "rb" )) == NULL )
        throw TC_SHA_Exception("[TC_SHA::sha512file] fopen '" + fileName + "', error.", errno);

    detail_sha2::sha512_begin(cx);
    while((n = fread( buf, 1, sizeof( buf ),f)) > 0 )
        detail_sha2::sha512_hash(buf, n, cx);

    detail_sha2::sha512_end(sOutBuffer, cx);

    fclose(f);

    return TC_Common::bin2str(sOutBuffer, sizeof(sOutBuffer));
}

}

