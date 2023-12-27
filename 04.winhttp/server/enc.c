
#include "head.h"
#include <stdio.h>


// 32位XORShift算法生成伪随机数
unsigned int xorshift32(unsigned int* state) {
    unsigned int x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}
void* genKey16() {

    char* tmpAlloc = malloc(64);
    int tmp32;
    for (int i = 0; i < 4; i++) {
        tmp32 = rand();
        xorshift32(&tmp32);
        memcpy(tmpAlloc + i * 4, &tmp32, 4);
    }
    return tmpAlloc;
}
#define uint32_t unsigned int
//加密函数  
void encrypt(uint32_t* v, uint32_t* k) {
    uint32_t v0 = v[0], v1 = v[1], sum = 0, i;           /* set up */
    uint32_t delta = 0x9e3779b9;                     /* a key schedule constant */
    uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];   /* cache key */
    for (i = 0; i < 32; i++) {                       /* basic cycle start */
        sum += delta;
        v0 += ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
        v1 += ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
    }                                              /* end cycle */
    v[0] = v0; v[1] = v1;
}
//解密函数  
void decrypt(uint32_t* v, uint32_t* k) {
    uint32_t v0 = v[0], v1 = v[1], sum = 0xC6EF3720, i;  /* set up */
    uint32_t delta = 0x9e3779b9;                     /* a key schedule constant */
    uint32_t k0 = k[0], k1 = k[1], k2 = k[2], k3 = k[3];   /* cache key */
    for (i = 0; i < 32; i++) {                         /* basic cycle start */
        v1 -= ((v0 << 4) + k2) ^ (v0 + sum) ^ ((v0 >> 5) + k3);
        v0 -= ((v1 << 4) + k0) ^ (v1 + sum) ^ ((v1 >> 5) + k1);
        sum -= delta;
    }                                              /* end cycle */
    v[0] = v0; v[1] = v1;
}

void xxtea(char* c, char* k, unsigned int len, int mod) {
    int round = len / 8;
    int i;
    void (*lp_func)(uint32_t * v, uint32_t * k) = NULL;
    if (mod == 0) {
        lp_func = encrypt;
    }
    else {
        lp_func = decrypt;
    }
    for (i = 0; i < round; i++) {
        lp_func(c + i * 8, k);
    }
}