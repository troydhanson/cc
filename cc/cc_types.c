#include "cc-internal.h"

const UT_mm ptr_mm = { .sz = sizeof(void*) };
const UT_mm ccmap_mm={ .sz = sizeof(struct cc_map) };

static void cc_init(void *_cc) {
  struct cc *cc = (struct cc*)_cc;
  utvector_init(&cc->names,        utstring_mm);
  utvector_init(&cc->output_types, utmm_int);
  utvector_init(&cc->defaults,     utstring_mm);
  utvector_init(&cc->caller_addrs, &ptr_mm);
  utvector_init(&cc->caller_types, utmm_int);
  utvector_init(&cc->dissect_map,  &ccmap_mm);
  utstring_init(&cc->flat);
  utstring_init(&cc->tmp);
  cc->json = json_object();
}
static void cc_fini(void *_cc) {
  struct cc *cc = (struct cc*)_cc;
  utvector_fini(&cc->names);
  utvector_fini(&cc->output_types);
  utvector_fini(&cc->defaults);
  utvector_fini(&cc->caller_addrs);
  utvector_fini(&cc->caller_types);
  utvector_fini(&cc->dissect_map);
  utstring_done(&cc->flat);
  utstring_done(&cc->tmp);
  json_decref(cc->json);
}
static void cc_copy(void *_dst, void *_src) {
  struct cc *dst = (struct cc*)_dst;
  struct cc *src = (struct cc*)_src;
  //utmm_copy(utvector_mm, &dst->names, &src->names, 1);
  //utmm_copy(utvector_mm, &dst->output_types, &src->output_types, 1);
  //utmm_copy(utvector_mm, &dst->defaults, &src->defaults, 1);
  //utmm_copy(utvector_mm, &dst->caller_addrs, &src->caller_addrs, 1);
  //utmm_copy(utvector_mm, &dst->caller_types, &src->caller_types, 1);
  //utmm_copy(utvector_mm, &dst->dissect_map, &src->dissect_map, 1);
  //utmm_copy(utstring_mm, &dst->flat, &src->flat, 1);
  //utmm_copy(utstring_mm, &dst->tmp, &src->tmp, 1);
  utvector_copy(&dst->names,       &src->names);
  utvector_copy(&dst->output_types,&src->output_types);
  utvector_copy(&dst->defaults,    &src->defaults);
  utvector_copy(&dst->caller_addrs,&src->caller_addrs);
  utvector_copy(&dst->caller_types,&src->caller_types);
  utvector_copy(&dst->dissect_map, &src->dissect_map);
  utstring_bincpy(&dst->flat,utstring_body(&src->flat),utstring_len(&src->flat));
  utstring_bincpy(&dst->tmp,utstring_body(&src->tmp),utstring_len(&src->tmp));
  dst->json = json_incref(src->json);
}
static void cc_clear(void *_cc) {
  struct cc *cc = (struct cc*)_cc;
  utvector_clear(&cc->names);
  utvector_clear(&cc->output_types);
  utvector_clear(&cc->defaults);
  utvector_clear(&cc->caller_addrs);
  utvector_clear(&cc->caller_types);
  utvector_clear(&cc->dissect_map);
  utstring_clear(&cc->flat);
  utstring_clear(&cc->tmp);
  json_object_clear(cc->json);
}

UT_mm const cc_mm = {
  .sz = sizeof(struct cc),
  .init = cc_init,
  .fini = cc_fini,
  .copy = cc_copy,
  .clear = cc_clear,
};

static int xcpf_i16_u16(UT_string *d, void *p) {
  utstring_bincpy(d, p, sizeof(int16_t));
  return 0;
}

static int xcpf_u16_u16(UT_string *d, void *p) {
  utstring_bincpy(d, p, sizeof(uint16_t));
  return 0;
}

static int xcpf_u16_i16(UT_string *d, void *p) {
  utstring_bincpy(d, p, sizeof(int16_t));
  return 0;
}

static int xcpf_i16_i16(UT_string *d, void *p) {
  utstring_bincpy(d, p, sizeof(int16_t));
  return 0;
}

static int xcpf_u16_i32(UT_string *d, void *p) {
  uint16_t u16 = *(uint16_t*)p;
  int32_t i32 = u16;
  utstring_bincpy(d, &i32, sizeof(i32));
  return 0;
}

static int xcpf_i16_i32(UT_string *d, void *p) {
  int16_t i16 = *(int16_t*)p;
  int32_t i32 = i16;
  utstring_bincpy(d, &i32, sizeof(i32));
  return 0;
}

static int xcpf_u16_str8(UT_string *d, void *p) {
  uint16_t u16 = *(uint16_t*)p;
  unsigned u = u16;
  char s[10];
  snprintf(s, sizeof(s), "%u", u);
  uint32_t l = strlen(s);
  assert(l < 256);
  uint8_t u8 = (uint8_t)l;
  utstring_bincpy(d, &u8, sizeof(u8));
  utstring_bincpy(d, s, u8);
  return 0;
}

static int xcpf_u16_str(UT_string *d, void *p) {
  uint16_t u16 = *(uint16_t*)p;
  unsigned u = u16;
  char s[10];
  snprintf(s, sizeof(s), "%u", u);
  uint32_t l = strlen(s);
  utstring_bincpy(d, &l, sizeof(l));
  utstring_bincpy(d, s, l);
  return 0;
}

static int xcpf_i16_str8(UT_string *d, void *p) {
  int16_t i16 = *(int16_t*)p;
  int i = i16;
  char s[10];
  snprintf(s, sizeof(s), "%d", i);
  uint32_t l = strlen(s);
  assert(l < 256);
  uint8_t u8 = (uint8_t)l;
  utstring_bincpy(d, &u8, sizeof(u8));
  utstring_bincpy(d, s, u8);
  return 0;
}

static int xcpf_i16_str(UT_string *d, void *p) {
  int16_t i16 = *(int16_t*)p;
  int i = i16;
  char s[10];
  snprintf(s, sizeof(s), "%d", i);
  uint32_t l = strlen(s);
  utstring_bincpy(d, &l, sizeof(l));
  utstring_bincpy(d, s, l);
  return 0;
}

static int xcpf_u16_d64(UT_string *d, void *p) {
  uint16_t u16 = *(uint16_t*)p;
  double f = u16;
  utstring_bincpy(d, &f, sizeof(f));
  return 0;
}

static int xcpf_i16_d64(UT_string *d, void *p) {
  int16_t i16 = *(int16_t*)p;
  double f = i16;
  utstring_bincpy(d, &f, sizeof(f));
  return 0;
}

static int xcpf_i32_i32(UT_string *d, void *p) {
  utstring_bincpy(d, p, sizeof(int32_t));
  return 0;
}

/* assumes source i32 is already in network order */
static int xcpf_i32_ipv4(UT_string *d, void *p) {
  utstring_bincpy(d, p, sizeof(int32_t));
  return 0;
}

/* assumes source i32 is already in network order */
static int xcpf_i32_ipv46(UT_string *d, void *p) {
  uint8_t u8 = 4;
  utstring_bincpy(d, &u8, sizeof(uint8_t));
  utstring_bincpy(d, p, sizeof(int32_t));
  return 0;
}

static int xcpf_i32_str8(UT_string *d, void *p) {
  int32_t i32 = *(int32_t*)p;
  int i = i32;
  char s[20];
  snprintf(s, sizeof(s), "%d", i);
  uint32_t l = strlen(s);
  assert(l < 256);
  uint8_t u8 = (uint8_t)l;
  utstring_bincpy(d, &u8, sizeof(u8));
  utstring_bincpy(d, s, u8);
  return 0;
}

static int xcpf_i32_str(UT_string *d, void *p) {
  int32_t i32 = *(int32_t*)p;
  int i = i32;
  char s[20];
  snprintf(s, sizeof(s), "%d", i);
  uint32_t l = strlen(s);
  utstring_bincpy(d, &l, sizeof(l));
  utstring_bincpy(d, s, l);
  return 0;
}

static int xcpf_i32_d64(UT_string *d, void *p) {
  int32_t i32 = *(int32_t*)p;
  double f = i32;
  utstring_bincpy(d, &f, sizeof(f));
  return 0;
}

static int xcpf_ipv4_i32(UT_string *d, void *p) {
  utstring_bincpy(d, p, sizeof(int32_t));
  return 0;
}

static int xcpf_ipv4_ipv4(UT_string *d, void *p) {
  utstring_bincpy(d, p, sizeof(int32_t));
  return 0;
}

static int xcpf_ipv4_ipv46(UT_string *d, void *p) {
  uint8_t u8 = 4;
  utstring_bincpy(d, &u8, sizeof(uint8_t));
  utstring_bincpy(d, p, sizeof(int32_t));
  return 0;
}

/* consider u32 as in network order; see also slot_to_json case CC_ipv4*/
static int xcpf_ipv4_str8(UT_string *d, void *p) {
  uint32_t u32 = *(uint32_t*)p;
  char out[INET_ADDRSTRLEN];
  if (inet_ntop(AF_INET, &u32, out, sizeof(out)) == NULL)
    return -1;
  uint32_t l = strlen(out);
  assert(l < 256);
  uint8_t u8 = (uint8_t)l;
  utstring_bincpy(d, &u8, sizeof(u8));
  utstring_bincpy(d, out, u8);
  return 0;
}

/* consider u32 as in network order; see also slot_to_json case CC_ipv4*/
static int xcpf_ipv4_str(UT_string *d, void *p) {
  uint32_t u32 = *(uint32_t*)p;
  char out[INET_ADDRSTRLEN];
  if (inet_ntop(AF_INET, &u32, out, sizeof(out)) == NULL)
    return -1;
  uint32_t l = strlen(out);
  utstring_bincpy(d, &l, sizeof(l));
  utstring_bincpy(d, out, l);
  return 0;
}

static int xcpf_ipv46_ipv46(UT_string *d, void *p) {
  uint8_t u8;
  memcpy(&u8, p, sizeof(u8));
  assert((u8 == 4) || (u8 == 16));
  utstring_bincpy(d, p, sizeof(uint8_t) + u8);
  return 0;
}

static int xcpf_ipv46_str8(UT_string *d, void *p) {
  uint8_t u8;

  memcpy(&u8, p, sizeof(u8));
  assert((u8 == 4) || (u8 == 16));

  p = (char*)p + sizeof(uint8_t);

  if (u8 == 4)
    return xcpf_ipv4_str8(d, p);

  char out[INET6_ADDRSTRLEN];
  struct in6_addr ia6;
  memcpy(&ia6, p, sizeof(ia6));
  if (inet_ntop(AF_INET6, &ia6, out, sizeof(out)) == NULL)
    return -1;

  uint32_t l = strlen(out);
  assert(l < 256);
  u8 = (uint8_t)l;
  utstring_bincpy(d, &u8, sizeof(u8));
  utstring_bincpy(d, out, u8);
  return 0;
}

static int xcpf_ipv46_str(UT_string *d, void *p) {
  uint8_t u8;

  memcpy(&u8, p, sizeof(u8));
  assert((u8 == 4) || (u8 == 16));

  p = (char*)p + sizeof(uint8_t);

  if (u8 == 4)
    return xcpf_ipv4_str(d, p);

  char out[INET6_ADDRSTRLEN];
  struct in6_addr ia6;
  memcpy(&ia6, p, sizeof(ia6));
  if (inet_ntop(AF_INET6, &ia6, out, sizeof(out)) == NULL)
    return -1;

  uint32_t l = strlen(out);
  utstring_bincpy(d, &l, sizeof(l));
  utstring_bincpy(d, out, l);
  return 0;
}

static int xcpf_str_u16(UT_string *d, void *p) {
  char **c = (char **)p;
  unsigned u;
  if (sscanf(*c, "%u", &u) != 1) return -1;
  uint16_t u16 = u; // may truncate
  utstring_bincpy(d, &u16, sizeof(u16));
  return 0;
}

static int xcpf_str_i16(UT_string *d, void *p) {
  char **c = (char **)p;
  int i;
  if (sscanf(*c, "%d", &i) != 1) return -1;
  int16_t i16 = i; // may truncate
  utstring_bincpy(d, &i16, sizeof(i16));
  return 0;
}

static int xcpf_str_i32(UT_string *d, void *p) {
  char **c = (char **)p;
  int i;
  if (sscanf(*c, "%d", &i) != 1) return -1;
  int32_t i32 = i;
  utstring_bincpy(d, &i32, sizeof(i32));
  return 0;
}

static int xcpf_str_ipv4(UT_string *d, void *p) {
  char **s = (char **)p;
  struct in_addr ia;
  assert(sizeof(ia) == 4);
  if (inet_pton(AF_INET, *s, &ia) <= 0)
    return -1;
  utstring_bincpy(d, &ia, sizeof(ia));
  return 0;
}

static int xcpf_str_ipv46(UT_string *d, void *p) {
  char **s = (char **)p;
  struct in_addr ia4;
  struct in6_addr ia6;
  uint8_t u8;
  int sc;

  sc = inet_pton(AF_INET, *s, &ia4);
  if (sc == 1) {
    u8 = 4;
    assert(sizeof(ia4) == 4);
    utstring_bincpy(d, &u8, sizeof(uint8_t));
    utstring_bincpy(d, &ia4, sizeof(ia4));
    return 0;
  }

  sc = inet_pton(AF_INET6, *s, &ia6);
  if (sc == 1) {
    u8 = 16;
    assert(sizeof(ia6) == 16);
    utstring_bincpy(d, &u8, sizeof(uint8_t));
    utstring_bincpy(d, &ia6, sizeof(ia6));
    return 0;
  }

  return -1;
}

static int xcpf_str_str8(UT_string *d, void *p) {
  char **c = (char **)p;
  uint32_t l = strlen(*c);
  if (l > 255) return -1;
  uint8_t u8 = (uint8_t)l;
  utstring_bincpy(d, &u8, sizeof(u8));
  if (l) utstring_printf(d, "%s", *c);
  return 0;
}

static int xcpf_str_str(UT_string *d, void *p) {
  char **c = (char **)p;
  uint32_t l = strlen(*c);
  utstring_bincpy(d, &l, sizeof(l));
  if (l) utstring_printf(d, "%s", *c);
  return 0;
}

static int xcpf_str_blob(UT_string *d, void *p) {
  char **c = (char **)p;
  uint32_t l = strlen(*c);
  utstring_bincpy(d, &l, sizeof(l));
  if (l) utstring_printf(d, "%s", *c);
  return 0;
}

static int xcpf_str_i8(UT_string *d, void *p) {
  char **c = (char **)p;
  int i;
  if (sscanf(*c, "%d", &i) != 1) return -1;
  int8_t i8 = i; // may truncate
  utstring_bincpy(d, &i8, sizeof(i8));
  return 0;
}

static int xcpf_str_d64(UT_string *d, void *p) {
  char **c = (char **)p;
  double f;
  if (sscanf(*c, "%lf", &f) != 1) return -1;
  utstring_bincpy(d, &f, sizeof(f));
  return 0;
}

static int xcpf_str_mac(UT_string *d, void *p) {
  char **c = (char **)p;
  unsigned int ma, mb, mc, md, me, mf;
  if (sscanf(*c, "%x:%x:%x:%x:%x:%x", &ma,&mb,&mc,&md,&me,&mf) != 6) return -1;
  if ((ma > 255) || (mb > 255) || (mc > 255) || 
      (md > 255) || (me > 255) || (mf > 255)) return -1;
  utstring_printf(d, "%c%c%c%c%c%c", ma, mb, mc, md, me, mf);
  return 0;
}

static int xcpf_i8_u16(UT_string *d, void *p) {
  int8_t i8 = *(int8_t*)p;
  uint16_t u16 = i8;
  utstring_bincpy(d, &u16, sizeof(u16));
  return 0;
}

static int xcpf_i8_i16(UT_string *d, void *p) {
  int8_t i8 = *(int8_t*)p;
  int16_t i16 = i8;
  utstring_bincpy(d, &i16, sizeof(i16));
  return 0;
}

static int xcpf_i8_i32(UT_string *d, void *p) {
  int8_t i8 = *(int8_t*)p;
  int32_t i32 = i8;
  utstring_bincpy(d, &i32, sizeof(i32));
  return 0;
}

static int xcpf_i8_str8(UT_string *d, void *p) {
  int8_t i8 = *(int8_t*)p;
  int i = i8;
  char s[5];
  snprintf(s, sizeof(s), "%d", i);
  uint32_t l = strlen(s);
  assert(l < 256);
  uint8_t u8 = (uint8_t)l;
  utstring_bincpy(d, &u8, sizeof(u8));
  utstring_bincpy(d, s, u8);
  return 0;
}

static int xcpf_i8_str(UT_string *d, void *p) {
  int8_t i8 = *(int8_t*)p;
  int i = i8;
  char s[5];
  snprintf(s, sizeof(s), "%d", i);
  uint32_t l = strlen(s);
  utstring_bincpy(d, &l, sizeof(l));
  utstring_bincpy(d, s, l);
  return 0;
}

static int xcpf_i8_i8(UT_string *d, void *p) {
  utstring_bincpy(d, p, sizeof(int8_t));
  return 0;
}

static int xcpf_i8_d64(UT_string *d, void *p) {
  int8_t i8 = *(int8_t*)p;
  double f = i8;
  utstring_bincpy(d, &f, sizeof(f));
  return 0;
}

static int xcpf_d64_u16(UT_string *d, void *p) {
  double f = *(double*)p;
  uint16_t u16 = f;
  utstring_bincpy(d, &u16, sizeof(u16));
  return 0;
}

static int xcpf_d64_i16(UT_string *d, void *p) {
  double f = *(double*)p;
  int16_t i16 = f;
  utstring_bincpy(d, &i16, sizeof(i16));
  return 0;
}

static int xcpf_d64_i32(UT_string *d, void *p) {
  double f = *(double*)p;
  int32_t i32 = f;
  utstring_bincpy(d, &i32, sizeof(i32));
  return 0;
}

static int xcpf_d64_str8(UT_string *d, void *p) {
  double f = *(double*)p;
  char s[40];
  snprintf(s, sizeof(s), "%f", f);
  uint32_t l = strlen(s);
  assert(l < 256);
  uint8_t u8 = (uint8_t)l;
  utstring_bincpy(d, &u8, sizeof(u8));
  utstring_bincpy(d, s, u8);
  return 0;
}

static int xcpf_d64_str(UT_string *d, void *p) {
  double f = *(double*)p;
  char s[40];
  snprintf(s, sizeof(s), "%f", f);
  uint32_t l = strlen(s);
  utstring_bincpy(d, &l, sizeof(l));
  utstring_bincpy(d, s, l);
  return 0;
}

static int xcpf_d64_i8(UT_string *d, void *p) {
  double f = *(double*)p;
  int8_t i8 = f;
  utstring_bincpy(d, &i8, sizeof(i8));
  return 0;
}

static int xcpf_d64_d64(UT_string *d, void *p) {
  utstring_bincpy(d, p, sizeof(double));
  return 0;
}

static int xcpf_mac_str8(UT_string *d, void *p) {
  unsigned char *m = p;
  char s[20];
  snprintf(s, sizeof(s), "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
     (unsigned)m[0], (unsigned)m[1], (unsigned)m[2],  
     (unsigned)m[3], (unsigned)m[4], (unsigned)m[5]);
  uint32_t l = strlen(s);
  assert(l < 256);
  uint8_t u8 = (uint8_t)l;
  utstring_bincpy(d, &u8, sizeof(u8));
  utstring_bincpy(d, s, u8);
  return 0;
}

static int xcpf_mac_str(UT_string *d, void *p) {
  unsigned char *m = p;
  char s[20];
  snprintf(s, sizeof(s), "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x",
     (unsigned)m[0], (unsigned)m[1], (unsigned)m[2],  
     (unsigned)m[3], (unsigned)m[4], (unsigned)m[5]);
  uint32_t l = strlen(s);
  utstring_bincpy(d, &l, sizeof(l));
  utstring_bincpy(d, s, l);
  return 0;
}

static int xcpf_mac_mac(UT_string *d, void *p) {
  utstring_bincpy(d, p, 6*sizeof(char));
  return 0;
}

static int xcpf_blob_str(UT_string *d, void *p) {
  struct cc_blob *bp = (struct cc_blob*)p;
  uint32_t l = (uint32_t)bp->len;
  utstring_bincpy(d, &l, sizeof(l));
  if (l) utstring_bincpy(d, bp->buf, l);
  return 0;
}

static int xcpf_blob_blob(UT_string *d, void *p) {
  struct cc_blob *bp = (struct cc_blob*)p;
  uint32_t l = (uint32_t)bp->len;
  utstring_bincpy(d, &l, sizeof(l));
  if (l) utstring_bincpy(d, bp->buf, l);
  return 0;
}


xcpf cc_conversions[/*from*/CC_MAX][/*to*/CC_MAX] = {
  [CC_i16][CC_u16] = xcpf_i16_u16,
  [CC_i16][CC_i16] = xcpf_i16_i16,
  [CC_i16][CC_i32] = xcpf_i16_i32,
  [CC_i16][CC_ipv4] = NULL,
  [CC_i16][CC_ipv46] = NULL,
  [CC_i16][CC_str] = xcpf_i16_str,
  [CC_i16][CC_str8] = xcpf_i16_str8,
  [CC_i16][CC_i8] = NULL,
  [CC_i16][CC_d64] = xcpf_i16_d64,
  [CC_i16][CC_mac] = NULL,
  [CC_i16][CC_blob] = NULL,

  [CC_u16][CC_u16] = xcpf_u16_u16,
  [CC_u16][CC_i16] = xcpf_u16_i16,
  [CC_u16][CC_i32] = xcpf_u16_i32,
  [CC_u16][CC_ipv4] = NULL,
  [CC_u16][CC_ipv46] = NULL,
  [CC_u16][CC_str] = xcpf_u16_str,
  [CC_u16][CC_str8] = xcpf_u16_str8,
  [CC_u16][CC_i8] = NULL,
  [CC_u16][CC_d64] = xcpf_u16_d64,
  [CC_u16][CC_mac] = NULL,
  [CC_u16][CC_blob] = NULL,

  [CC_i32][CC_u16] = NULL,
  [CC_i32][CC_i16] = NULL,
  [CC_i32][CC_i32] = xcpf_i32_i32,
  [CC_i32][CC_ipv4] = xcpf_i32_ipv4,
  [CC_i32][CC_ipv46] = xcpf_i32_ipv46,
  [CC_i32][CC_str] = xcpf_i32_str,
  [CC_i32][CC_str8] = xcpf_i32_str8,
  [CC_i32][CC_i8] = NULL,
  [CC_i32][CC_d64] = xcpf_i32_d64,
  [CC_i32][CC_mac] = NULL,
  [CC_i32][CC_blob] = NULL,

  [CC_ipv4][CC_u16] = NULL,
  [CC_ipv4][CC_i16] = NULL,
  [CC_ipv4][CC_i32] = xcpf_ipv4_i32,
  [CC_ipv4][CC_ipv4] = xcpf_ipv4_ipv4,
  [CC_ipv4][CC_ipv46] = xcpf_ipv4_ipv46,
  [CC_ipv4][CC_str] = xcpf_ipv4_str,
  [CC_ipv4][CC_str8] = xcpf_ipv4_str8,
  [CC_ipv4][CC_i8] = NULL,
  [CC_ipv4][CC_d64] = NULL,
  [CC_ipv4][CC_mac] = NULL,
  [CC_ipv4][CC_blob] = NULL,

  [CC_ipv46][CC_u16] = NULL,
  [CC_ipv46][CC_i16] = NULL,
  [CC_ipv46][CC_i32] = NULL,
  [CC_ipv46][CC_ipv4] = NULL,
  [CC_ipv46][CC_ipv46] = xcpf_ipv46_ipv46,
  [CC_ipv46][CC_str] = xcpf_ipv46_str,
  [CC_ipv46][CC_str8] = xcpf_ipv46_str8,
  [CC_ipv46][CC_i8] = NULL,
  [CC_ipv46][CC_d64] = NULL,
  [CC_ipv46][CC_mac] = NULL,
  [CC_ipv46][CC_blob] = NULL,

  /* FIXME
   *
   * conversions from CC_str 
   * only apply to cc_capture as
   * there is no conversion from 
   * length-prefixed strings to 
   * other-types right now.
   *
   * there are a couple implications:
   * 1. caller cc_mapv must use CC_str
   *    to receive CC_str fields (i.e.
   *    no conversion via cc_restore from
   *    a CC_str to say, CC_i32),
   *    since we can't programmatically
   *    convert from length-prefixed 
   *    strings to other types-- at 
   *    least, not without a DWIMMER
   *    that switches off whether the
   *    input is caller memory (capture)
   *    or a flat buffer (restore).
   * 
   * 2. CC_str8 is not supported as a
   *    cc_mapv type; just use CC_str.
   *    CC_str8 is allowed in the cast
   *    format, and will be stored
   *    that way after capturing from
   *    CC_str
   *
   */
#if 0
  [CC_str8][CC_u16] = xcpf_str8_u16,
  [CC_str8][CC_i16] = xcpf_str8_i16,
  [CC_str8][CC_i32] = xcpf_str8_i32,
  [CC_str8][CC_ipv4] = xcpf_str8_ipv4,
  [CC_str8][CC_ipv46] = xcpf_str8_ipv46,
  [CC_str8][CC_str] = xcpf_str8_str,
  [CC_str8][CC_str8] = xcpf_str8_str,
  [CC_str8][CC_i8] = xcpf_str8_i8,
  [CC_str8][CC_d64] = xcpf_str8_d64,
  [CC_str8][CC_mac] = xcpf_str8_mac,
  [CC_str8][CC_blob] = xcpf_str8_blob,
#endif
  [CC_str][CC_u16] = xcpf_str_u16,
  [CC_str][CC_i16] = xcpf_str_i16,
  [CC_str][CC_i32] = xcpf_str_i32,
  [CC_str][CC_ipv4] = xcpf_str_ipv4,
  [CC_str][CC_ipv46] = xcpf_str_ipv46,
  [CC_str][CC_str] = xcpf_str_str,
  [CC_str][CC_str8] = xcpf_str_str8,
  [CC_str][CC_i8] = xcpf_str_i8,
  [CC_str][CC_d64] = xcpf_str_d64,
  [CC_str][CC_mac] = xcpf_str_mac,
  [CC_str][CC_blob] = xcpf_str_blob,

  [CC_i8][CC_u16] = xcpf_i8_u16,
  [CC_i8][CC_i16] = xcpf_i8_i16,
  [CC_i8][CC_i32] = xcpf_i8_i32,
  [CC_i8][CC_ipv4] = NULL,
  [CC_i8][CC_ipv46] = NULL,
  [CC_i8][CC_str] = xcpf_i8_str,
  [CC_i8][CC_str8] = xcpf_i8_str8,
  [CC_i8][CC_i8] = xcpf_i8_i8,
  [CC_i8][CC_d64] = xcpf_i8_d64,
  [CC_i8][CC_mac] = NULL,
  [CC_i8][CC_blob] = NULL,

  [CC_d64][CC_u16] = xcpf_d64_u16,
  [CC_d64][CC_i16] = xcpf_d64_i16,
  [CC_d64][CC_i32] = xcpf_d64_i32,
  [CC_d64][CC_ipv4] = NULL,
  [CC_d64][CC_ipv46] = NULL,
  [CC_d64][CC_str] = xcpf_d64_str,
  [CC_d64][CC_str8] = xcpf_d64_str8,
  [CC_d64][CC_i8] = xcpf_d64_i8,
  [CC_d64][CC_d64] = xcpf_d64_d64,
  [CC_d64][CC_mac] = NULL,
  [CC_d64][CC_blob] = NULL,

  [CC_mac][CC_u16] = NULL,
  [CC_mac][CC_i16] = NULL,
  [CC_mac][CC_i32] = NULL,
  [CC_mac][CC_ipv4] = NULL,
  [CC_mac][CC_str] = xcpf_mac_str,
  [CC_mac][CC_str8] = xcpf_mac_str8,
  [CC_mac][CC_i8] = NULL,
  [CC_mac][CC_d64] = NULL,
  [CC_mac][CC_mac] = xcpf_mac_mac,
  [CC_mac][CC_blob] = NULL,

  [CC_blob][CC_u16] = NULL,
  [CC_blob][CC_i16] = NULL,
  [CC_blob][CC_i32] = NULL,
  [CC_blob][CC_ipv4] = NULL,
  [CC_blob][CC_ipv46] = NULL,
  [CC_blob][CC_str] = xcpf_blob_str,
  [CC_blob][CC_str8] = NULL,
  [CC_blob][CC_i8] = NULL,
  [CC_blob][CC_d64] = NULL,
  [CC_blob][CC_mac] = NULL,
  [CC_blob][CC_blob] = xcpf_blob_blob,
};

/*
 * blob_encode
 *
 * simple encoder of binary to safe string. caller must free!
 */
static char hex[16] = {'0','1','2','3','4','5','6','7','8','9',
                       'a','b','c','d','e','f'};
static char *blob_encode(char *from, size_t len, size_t *enc_len) {
  char *enc, *e;
  unsigned char f;

  enc = malloc(len * 2);
  if (enc == NULL) return NULL;

  *enc_len = len * 2;
  e = enc;
  while(len) {
    f = (unsigned char)*from;
    *e = hex[(f & 0xf0) >> 4];
    e++;
    *e = hex[(f & 0x0f)];
    e++;
    from++;
    len--;
  }
  return enc;
}

/*
 * slot_to_json
 *
 * convert a single item from flat cc buffer 
 * to a refcounted json value. 
 *
 * the caller is expected to steal the reference 
 * (e.g. json_set_object_new).
 *
 * return 
 *    bytes consumed (into from) on success
 *    or -1 on error 
 *
 */
int slot_to_json(cc_type ot, char *from, size_t len, json_t **j) {
  int rc = -1;
  unsigned l=0;
  int8_t i8;
  uint8_t u8;
  int16_t i16;
  uint16_t u16;
  int32_t i32;
  uint32_t u32;
  char s[20];
  char ip4str[INET_ADDRSTRLEN];
  char ip6str[INET6_ADDRSTRLEN];
  struct in6_addr ia6;
  unsigned char *m;
  double f;
  char *enc;
  size_t enc_len;

  switch(ot) {
    case CC_i8:
      l = sizeof(int8_t);
      if (len < l) goto done;
      memcpy(&i8, from, l);
      *j = json_integer(i8);
      if (*j == NULL) goto done;
      break;
    case CC_u16:
      l = sizeof(uint16_t);
      if (len < l) goto done;
      memcpy(&u16, from, l);
      *j = json_integer(u16);
      if (*j == NULL) goto done;
      break;
    case CC_i16:
      l = sizeof(int16_t);
      if (len < l) goto done;
      memcpy(&i16, from, l);
      *j = json_integer(i16);
      if (*j == NULL) goto done;
      break;
    case CC_i32:
      l = sizeof(int32_t);
      if (len < l) goto done;
      memcpy(&i32, from, l);
      *j = json_integer(i32);
      if (*j == NULL) goto done;
      break;
    case CC_blob:
      l = sizeof(uint32_t);
      if (len < l) goto done;
      memcpy(&u32, from, l);
      from += l;
      len -= l;
      if ((u32 > 0) && (len < u32)) goto done;
      l += u32;
      enc = blob_encode(from, u32, &enc_len);
      if (enc == NULL) goto done;
      *j = json_stringn(enc, enc_len);
      free(enc);
      if (*j == NULL) goto done;
      break;
    case CC_str8:
      l = sizeof(uint8_t);
      if (len < l) goto done;
      memcpy(&u8, from, l);
      from += l;
      len -= l;
      if ((u8 > 0) && (len < u8)) goto done;
      l += u8;
      *j = json_stringn(from, u8);
      if (*j == NULL) goto done;
      break;
    case CC_str:
      l = sizeof(uint32_t);
      if (len < l) goto done;
      memcpy(&u32, from, l);
      from += l;
      len -= l;
      if ((u32 > 0) && (len < u32)) goto done;
      l += u32;
      *j = json_stringn(from, u32);
      if (*j == NULL) goto done;
      break;
    case CC_d64:
      l = sizeof(double);
      if (len < l) goto done;
      memcpy(&f, from, l);
      *j = json_real(f);
      if (*j == NULL) goto done;
      break;
    case CC_ipv46:
      l = sizeof(uint8_t);
      if (len < l) goto done;
      memcpy(&u8, from, l);
      assert((u8 == 4) || (u8 == 16));
      l += u8;
      from += sizeof(uint8_t);
      len -=  sizeof(uint8_t);
      if (len < u8) goto done;
      if (u8 == 16) {
        memcpy(&ia6, from, sizeof(struct in6_addr));
        if (inet_ntop(AF_INET6, &ia6, ip6str, sizeof(ip6str)) == NULL)
          goto done;
        *j = json_string(ip6str);
        if (*j == NULL) goto done;
      } else {
        assert(u8 == 4);
        memcpy(&u32, from, sizeof(u32));
        if (inet_ntop(AF_INET, &u32, ip4str, sizeof(ip4str)) == NULL) goto done;
        *j = json_string(ip4str);
        if (*j == NULL) goto done;
      }
      break;
    case CC_ipv4:
      l = sizeof(uint32_t);
      if (len < l) goto done;
      memcpy(&u32, from, l);
      if (inet_ntop(AF_INET, &u32, ip4str, sizeof(ip4str)) == NULL) goto done;
      *j = json_string(ip4str);
      if (*j == NULL) goto done;
      break;
    case CC_mac:
      l = sizeof(char) * 6;
      if (len < l) goto done;
      m = (unsigned char*)from;
      snprintf(s, sizeof(s), "%x:%x:%x:%x:%x:%x", (int)m[0], (int)m[1], (int)m[2],  
                                          (int)m[3], (int)m[4], (int)m[5]);
      *j = json_string(s);
      if (*j == NULL) goto done;
      break;
    default:
      assert(0);
      goto done;
      break;
  }

  rc = 0;

 done:
  return (rc < 0) ? rc : (int)l;
}

