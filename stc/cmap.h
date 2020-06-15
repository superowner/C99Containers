/* MIT License
 *
 * Copyright (c) 2020 Tyge Løvset, NORCE, www.norceresearch.no
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/*  // Example:
#include <stdio.h>
#include "stc/cmap.h"
declare_CMap(ex, int, char);

int main(void) {
    CMap_ex h = cmap_init;
    cmap_ex_put(&h, 5, 'a');
    cmap_ex_put(&h, 8, 'b');
    CMapEntry_ex* b = cmap_ex_get(h, 10); // = NULL
    char val = cmap_ex_get(h, 5)->value;
    cmap_ex_put(&h, 5, 'd');
    cmap_ex_erase(&h, 8);
    c_foreach (i, cmap_ex, h)
        printf("%d: %c\n", i.item->key, i.item->value);
    cmap_ex_destroy(&h);
}
*/
#ifndef CMAP__H__
#define CMAP__H__

#include <stdlib.h>
#include "cdefs.h"

#define cmap_init          {NULL, NULL, 0, 0, 90, 0}
#define cmap_size(map)     ((size_t) (map)._size)
#define cmap_bucketCount(map)  ((size_t) (map)._cap)
/* https://lemire.me/blog/2016/06/27/a-fast-alternative-to-the-modulo-reduction */
#define cmap_reduce(x, N)  ((uint32_t) (((uint64_t) (x) * (N)) >> 32))

enum   {cmapentry_HASH=0x7f, cmapentry_USED=0x80};

/* CMap: */
#define declare_CMap(...) \
    c_MACRO_OVERLOAD(declare_CMap, __VA_ARGS__)

#define declare_CMap_3(tag, Key, Value) \
    declare_CMap_4(tag, Key, Value, c_noDestroy)

#define declare_CMap_4(tag, Key, Value, valueDestroy) \
    declare_CMap_5(tag, Key, Value, valueDestroy, c_defaultHash)

#define declare_CMap_5(tag, Key, Value, valueDestroy, keyHash) \
    declare_CMap_6(tag, Key, Value, valueDestroy, keyHash, c_defaultEquals)
    
#define declare_CMap_6(tag, Key, Value, valueDestroy, keyHash, keyEquals) \
    declare_CMap_10(tag, Key, Value, valueDestroy, c_noDestroy, Key, \
                         keyHash, keyEquals, c_defaultGetRaw, c_defaultInitRaw)


/* CMap<CString, Value>: */
#define declare_CMap_stringkey(...) \
    c_MACRO_OVERLOAD(declare_CMap_stringkey, __VA_ARGS__)

#define declare_CMap_stringkey_2(tag, Value) \
    declare_CMap_stringkey_3(tag, Value, c_noDestroy)

#define declare_CMap_stringkey_3(tag, Value, valueDestroy) \
    declare_CMap_10(tag, CString, Value, valueDestroy, cstring_destroy, const char*, \
                         cstring_hashRaw, cstring_equalsRaw, cstring_getRaw, cstring_make)


/* CMap full: */
#define declare_CMap_10(tag, Key, Value, valueDestroy, keyDestroy, RawKey, \
                             keyHashRaw, keyEqualsRaw, keyGetRaw, keyInitRaw) \
typedef struct CMapEntry_##tag { \
    Key key; \
    Value value; \
} CMapEntry_##tag; \
 \
static inline CMapEntry_##tag cmapentry_##tag##_make(Key key, Value value) { \
    CMapEntry_##tag e = {key, value}; return e; \
} \
static inline void \
cmapentry_##tag##_destroy(CMapEntry_##tag* e) { \
    keyDestroy(&e->key); \
    valueDestroy(&e->value); \
} \
 \
typedef RawKey CMapRawKey_##tag; \
 \
typedef struct CMap_##tag { \
    CMapEntry_##tag* _table; \
    uint8_t* _hashx; \
    uint32_t _size, _cap; \
    uint8_t maxLoadPercent; \
    uint8_t shrinkLimitPercent; \
} CMap_##tag; \
 \
typedef struct { \
    CMapEntry_##tag *item, *_end; \
    uint8_t* _hx; \
} CMapIter_##tag, cmap_##tag##_iter_t; \
 \
typedef struct { \
    CMapRawKey_##tag rawKey; \
    size_t index; \
    uint32_t hashx; \
} CMapBucket_##tag; \
 \
STC_API void \
cmap_##tag##_destroy(CMap_##tag* self); \
 \
STC_API void \
cmap_##tag##_clear(CMap_##tag* self); \
 \
STC_API void \
cmap_##tag##_setMaxLoadFactor(CMap_##tag* self, double fac); \
 \
STC_API void \
cmap_##tag##_setShrinkLimitFactor(CMap_##tag* self, double limit); \
 \
STC_API CMapEntry_##tag* \
cmap_##tag##_get(const CMap_##tag* self, CMapRawKey_##tag rawKey); \
 \
STC_API CMapEntry_##tag* \
cmap_##tag##_put(CMap_##tag* self, CMapRawKey_##tag rawKey, Value value); \
 \
STC_API CMapEntry_##tag* \
cmap_##tag##_find(CMap_##tag* self, CMapRawKey_##tag rawKey, CMapBucket_##tag* b); \
 \
STC_API void \
cmap_##tag##_insert(CMap_##tag* self, CMapBucket_##tag b, Value value); \
 \
STC_API size_t \
cmap_##tag##_reserve(CMap_##tag* self, size_t size); \
 \
STC_API bool \
cmap_##tag##_erase(CMap_##tag* self, CMapRawKey_##tag rawKey); \
 \
STC_API cmap_##tag##_iter_t \
cmap_##tag##_begin(CMap_##tag* map); \
 \
STC_API cmap_##tag##_iter_t \
cmap_##tag##_next(cmap_##tag##_iter_t it); \
 \
implement_CMap_10(tag, Key, Value, valueDestroy, keyDestroy, RawKey, \
                       keyHashRaw, keyEqualsRaw, keyGetRaw, keyInitRaw) \
 \
typedef Key CMapKey_##tag; \
typedef Value CMapValue_##tag

/* -------------------------- IMPLEMENTATION ------------------------- */

#if !defined(STC_HEADER) || defined(STC_IMPLEMENTATION)
#define implement_CMap_10(tag, Key, Value, valueDestroy, keyDestroy, RawKey, \
                               keyHashRaw, keyEqualsRaw, keyGetRaw, keyInitRaw) \
 \
STC_API void \
cmap_##tag##_destroy(CMap_##tag* self) { \
    if (cmap_size(*self)) { \
        size_t cap = cmap_bucketCount(*self); \
        CMapEntry_##tag* e = self->_table, *end = e + cap; \
        uint8_t *hashx = self->_hashx; \
        for (; e != end; ++e) if (*hashx++) cmapentry_##tag##_destroy(e); \
    } \
    free(self->_hashx); \
    free(self->_table); \
} \
 \
STC_API void cmap_##tag##_clear(CMap_##tag* self) { \
    cmap_##tag##_destroy(self); \
    self->_cap = self->_size = 0; \
} \
 \
STC_API void \
cmap_##tag##_setMaxLoadFactor(CMap_##tag* self, double fac) { \
    self->maxLoadPercent = (uint8_t) (fac * 100); \
    if (cmap_size(*self) >= cmap_bucketCount(*self) * fac) \
        cmap_##tag##_reserve(self, (size_t) (cmap_size(*self) / fac)); \
} \
 \
STC_API void \
cmap_##tag##_setShrinkLimitFactor(CMap_##tag* self, double limit) { \
    self->shrinkLimitPercent = (uint8_t) (limit * 100); \
    if (cmap_size(*self) < cmap_bucketCount(*self) * limit) \
        cmap_##tag##_reserve(self, (size_t) (cmap_size(*self) * 1.2 / limit)); \
} \
 \
static inline size_t \
cmap_##tag##_bucket(const CMap_##tag* self, const CMapRawKey_##tag* rawKeyPtr, uint32_t* hxPtr) { \
    uint32_t hash = keyHashRaw(rawKeyPtr, sizeof(CMapRawKey_##tag)); \
    uint32_t sx, hx = (hash & cmapentry_HASH) | cmapentry_USED; \
    size_t cap = cmap_bucketCount(*self); \
    size_t idx = cmap_reduce(hash, cap); \
    uint8_t* hashx = self->_hashx; \
    while ((sx = hashx[idx])) { \
        if (sx == hx) { \
            CMapRawKey_##tag r = keyGetRaw(&self->_table[idx].key); \
            if (keyEqualsRaw(&r, rawKeyPtr)) break; \
        } \
        if (++idx == cap) idx = 0; \
    } \
    *hxPtr = hx; \
    return idx; \
} \
 \
STC_API CMapEntry_##tag* \
cmap_##tag##_get(const CMap_##tag* self, CMapRawKey_##tag rawKey) { \
    if (cmap_bucketCount(*self) == 0) return NULL; \
    uint32_t hx; \
    size_t idx = cmap_##tag##_bucket(self, &rawKey, &hx); \
    return self->_hashx[idx] ? &self->_table[idx] : NULL; \
} \
 \
static inline void _cmap_##tag##_reserveExpand(CMap_##tag* self) { \
    if (cmap_size(*self) + 1 >= cmap_bucketCount(*self) * self->maxLoadPercent * 0.01) \
        cmap_##tag##_reserve(self, (size_t) 7 + (1.6 * cmap_bucketCount(*self))); \
} \
 \
STC_API CMapEntry_##tag* \
cmap_##tag##_put(CMap_##tag* self, CMapRawKey_##tag rawKey, Value value) { \
    _cmap_##tag##_reserveExpand(self); \
    uint32_t hx; \
    size_t idx = cmap_##tag##_bucket(self, &rawKey, &hx); \
    CMapEntry_##tag* e = &self->_table[idx]; \
    if (self->_hashx[idx]) \
        valueDestroy(&e->value); \
    else { \
        e->key = keyInitRaw(rawKey); \
        self->_hashx[idx] = (uint8_t) hx; \
        ++self->_size; \
    } \
    e->value = value; \
    return e; \
} \
 \
STC_API CMapEntry_##tag* \
cmap_##tag##_find(CMap_##tag* self, CMapRawKey_##tag rawKey, CMapBucket_##tag* b) { \
    _cmap_##tag##_reserveExpand(self); \
    b->rawKey = rawKey; \
    b->index = cmap_##tag##_bucket(self, &rawKey, &b->hashx); \
    return self->_hashx[b->index] ? &self->_table[b->index] : NULL; \
} \
 \
STC_API void \
cmap_##tag##_insert(CMap_##tag* self, CMapBucket_##tag b, Value value) { \
    CMapEntry_##tag* e = &self->_table[b.index]; \
    if (self->_hashx[b.index]) \
        valueDestroy(&e->value); \
    else { \
        e->key = keyInitRaw(b.rawKey); \
        self->_hashx[b.index] = (uint8_t) b.hashx; \
        ++self->_size; \
    } \
    e->value = value; \
} \
 \
static inline void \
cmap_##tag##_swap(CMap_##tag* a, CMap_##tag* b) { \
    c_swap(CMap_##tag, *a, *b); \
} \
 \
STC_API size_t \
cmap_##tag##_reserve(CMap_##tag* self, size_t size) { \
    size_t oldcap = cmap_bucketCount(*self), newcap = 1 + (size / 2) * 2; \
    if (cmap_size(*self) >= newcap * self->maxLoadPercent * 0.01) return oldcap; \
    CMap_##tag tmp = { \
        c_new_2(CMapEntry_##tag, newcap), \
        (uint8_t *) calloc(newcap, sizeof(uint8_t)), \
        self->_size, (uint32_t) newcap, \
        self->maxLoadPercent, self->shrinkLimitPercent \
    }; \
    cmap_##tag##_swap(self, &tmp); \
 \
    CMapEntry_##tag* e = tmp._table, *slot = self->_table; \
    uint8_t* hashx = self->_hashx; \
    uint32_t hx; \
    for (size_t i = 0; i < oldcap; ++i, ++e) \
        if (tmp._hashx[i]) { \
            CMapRawKey_##tag r = keyGetRaw(&e->key); \
            size_t idx = cmap_##tag##_bucket(self, &r, &hx); \
            slot[idx] = *e, \
            hashx[idx] = (uint8_t) hx; \
        } \
    free(tmp._hashx); \
    free(tmp._table); \
    return newcap; \
} \
 \
STC_API bool \
cmap_##tag##_erase(CMap_##tag* self, CMapRawKey_##tag rawKey) { \
    if (cmap_size(*self) == 0) \
        return false; \
    size_t cap = cmap_bucketCount(*self); \
    if (cmap_size(*self) < cap * self->shrinkLimitPercent * 0.01) \
        cap = cmap_##tag##_reserve(self, cmap_size(*self) * 120 / self->maxLoadPercent); \
    uint32_t hx; \
    size_t i = cmap_##tag##_bucket(self, &rawKey, &hx), j = i, k; \
    CMapEntry_##tag* slot = self->_table; \
    uint8_t* hashx = self->_hashx; \
    CMapRawKey_##tag r; \
    if (! hashx[i]) \
        return false; \
    do { /* deletion from hash table without tombstone */ \
        if (++j == cap) j = 0; /* ++j %= cap; is slow */ \
        if (! hashx[j]) \
            break; \
        r = keyGetRaw(&slot[j].key); \
        k = cmap_reduce(keyHashRaw(&r, sizeof(CMapRawKey_##tag)), cap); \
        if ((j < i) ^ (k <= i) ^ (k > j)) /* is k outside (i, j]? */ \
            slot[i] = slot[j], hashx[i] = hashx[j], i = j; \
    } while (true); \
    hashx[i] = 0; \
    cmapentry_##tag##_destroy(&slot[i]); \
    --self->_size; \
    return true; \
} \
 \
STC_API cmap_##tag##_iter_t \
cmap_##tag##_begin(CMap_##tag* map) { \
    uint8_t* hx = map->_hashx; \
    CMapEntry_##tag* e = map->_table, *end = e + cmap_bucketCount(*map); \
    while (e != end && !*hx) ++e, ++hx; \
    cmap_##tag##_iter_t it = {e == end ? NULL : e, end, hx}; return it; \
} \
 \
STC_API cmap_##tag##_iter_t \
cmap_##tag##_next(cmap_##tag##_iter_t it) { \
    do { ++it.item, ++it._hx; } while (it.item != it._end && !*it._hx); \
    if (it.item == it._end) it.item = NULL; \
    return it; \
}

#else
#define implement_CMap_10(tag, Key, Value, valueDestroy, keyDestroy, RawKey, \
                               keyHashRaw, keyEqualsRaw, keyGetRaw, keyInitRaw)
#endif

#endif
