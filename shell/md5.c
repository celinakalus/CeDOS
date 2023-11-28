#include "cedos.h"
#include "stdio.h"
#include "string.h"

const uint32_t K[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee,
    0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa,
    0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed,
    0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05,
    0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039,
    0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391 };

const uint32_t s[64] = {
    7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,  7, 12, 17, 22,
    5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,  5,  9, 14, 20,
    4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,  4, 11, 16, 23,
    6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21,  6, 10, 15, 21 };

#define MD5_CHUNK_SIZE 512

#define lrotate32(x, i) ((x << i) | (x >> (32 - i)))

void main(char *args) {
    FILE* file = fopen(args, "r");

    if (file == NULL) {
        printf("Could not find file: %s\n", args);
        return;
    }

    char buffer[MD5_CHUNK_SIZE];
    uint32_t hash[4] = { 0x01234567, 0x89abcdef, 0xfedcba98, 0x76543210 };

    int run = 1;
    while (run) {
        memset(buffer, 0, MD5_CHUNK_SIZE);
        int size = fread(buffer, 1, MD5_CHUNK_SIZE, file);
        if (size < MD5_CHUNK_SIZE) { 
            buffer[size] = 0x80;
            run = 0;
        }

        uint32_t *M = buffer;
        uint32_t chunk_hash[4] = { hash[0], hash[1], hash[2], hash[3] };
        uint32_t F, g;

        for (int i = 0; i < 64; i++) {
            if (i < 16) {
                F = (chunk_hash[1] & chunk_hash[2]) 
                    | ((~chunk_hash[1]) & chunk_hash[3]);
                g = i;
            } else if (i < 32) {
                F = (chunk_hash[3] & chunk_hash[1])
                    | ((~chunk_hash[3]) & chunk_hash[2]);
                g = (5 * i + 1) % 16;
            } else if (i < 48) {
                F = chunk_hash[1] ^ chunk_hash[2] ^ chunk_hash[3];
                g = (3 * i + 5) % 16;
            } else {
                F = chunk_hash[2] ^ (chunk_hash[2] | (~chunk_hash[3]));
                g = (7 * i) % 16;
            }
            
            F = F + chunk_hash[0] + K[i] + M[g];
            chunk_hash[0] = chunk_hash[3];
            chunk_hash[3] = chunk_hash[2];
            chunk_hash[2] = chunk_hash[1];
            chunk_hash[1] = chunk_hash[1] + lrotate32(F, s[i]);
        }

        hash[0] += chunk_hash[0];
        hash[1] += chunk_hash[1];
        hash[2] += chunk_hash[2];
        hash[3] += chunk_hash[3];
    }

    uint8_t *digest = (uint8_t *)(hash);

    for (int i = 0; i < 16; i++) {
        printf("%x", digest[i]);
    }
    printf("\n");
}