#ifndef MR_KV_H_INCLUDED
#define MR_KV_H_INCLUDED

#include <stdbool.h>
#include <stddef.h>

typedef struct MR_KV MR_KV;

MR_KV* kv_create(void);
void kv_destroy(MR_KV* kv);

bool kv_set(MR_KV* kv, const char* key, size_t klen, const char* val,
            size_t vlen);

const char* kv_get(MR_KV* kv, const char* key, size_t klen, size_t* out_vlen);

bool kv_del(MR_KV* kv, const char* key, size_t klen);

#endif