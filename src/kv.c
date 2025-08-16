#include "kv.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct Entry {
  uint64_t h;
  char* k;
  char* v;
  size_t klen;
  size_t vlen;
  struct Entry* next;
} Entry;

struct MR_KV {
  Entry** buckets;
  size_t cap;
  size_t size;
};

// -------------- Hash functions --------------
// Fowler-Noll-Vo hash (FNV-1a):
// https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
static uint64_t fnv1a(const char* s, size_t n) {
  uint64_t h = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    h ^= (unsigned char)s[i];
    h *= 1099511628211ULL;
  }

  return h;
}

// -------------- Internal helpers --------------
static MR_KV* kv_alloc(size_t cap) {
  MR_KV* kv = calloc(1, sizeof *kv);
  if (!kv) return NULL;
  kv->cap = cap;
  kv->buckets = calloc(cap, sizeof(Entry*));
  if (!kv->buckets) {
    free(kv);
    return NULL;
  }
  return kv;
}

static void free_entry(Entry* e) {
  if (!e) return;
  free(e->k);
  free(e->v);
  free(e);
}

static Entry* find_entry(Entry* head, const char* k, size_t klen, uint64_t h) {
  for (Entry* e = head; e; e = e->next) {
    if (e->h == h && e->klen == klen && memcmp(e->k, k, klen) == 0) return e;
  }
  return NULL;
}

static bool kv_resize(MR_KV* kv, size_t new_cap) {
  Entry** nb = calloc(new_cap, sizeof(Entry*));
  if (!nb) return false;

  for (size_t i = 0; i < kv->cap; i++) {
    for (Entry* e = kv->buckets[i]; e;) {
      Entry* next = e->next;
      size_t ni = (size_t)(e->h % new_cap);
      e->next = nb[ni];
      nb[ni] = e;
      e = next;
    }
  }
  free(kv->buckets);
  kv->buckets = nb;
  kv->cap = new_cap;
  return true;
}

// -------------- Public API --------------
MR_KV* kv_create(void) { return kv_alloc(1024); }

void kv_destroy(MR_KV* kv) {
  if (!kv) return;
  for (size_t i = 0; i < kv->cap; ++i) {
    for (Entry* e = kv->buckets[i]; e;) {
      Entry* n = e->next;
      free_entry(e);
      e = n;
    }
  }
  free(kv->buckets);
  free(kv);
}

const char* kv_get(MR_KV* kv, const char* key, size_t klen, size_t* out_vlen) {
  if(!kv) return NULL;

  uint64_t h = fnv1a(key, klen);
  size_t i = (size_t)(h % kv->cap);
}
