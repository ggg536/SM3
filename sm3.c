#include "sm3.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 禁用安全警告
#define _CRT_SECURE_NO_WARNINGS

// 循环左移
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

// 初始IV值
static const uint32_t IV[8] = {
    0x7380166f, 0x4914b2b9, 0x172442d7, 0xda8a0600,
    0xa96f30bc, 0x163138aa, 0xe38dee4d, 0xb0fb0e4e
};

// 压缩函数
static void compress(uint32_t state[8], const unsigned char block[64]) {
    uint32_t W[68], W1[64];
    uint32_t A, B, C, D, E, F, G, H;
    uint32_t SS1, SS2, TT1, TT2, T;
    int j;

    // 消息扩展
    // 第一步：生成W[0..15]
    for (j = 0; j < 16; j++) {
        W[j] = (block[j * 4] << 24) | 
               (block[j * 4 + 1] << 16) | 
               (block[j * 4 + 2] << 8) | 
               (block[j * 4 + 3]);
    }
    
    // 第二步：生成W[16..67]
    for (j = 16; j < 68; j++) {
        uint32_t temp = W[j-16] ^ W[j-9] ^ ROTATE_LEFT(W[j-3], 15);
        W[j] = (temp ^ ROTATE_LEFT(temp, 15) ^ ROTATE_LEFT(temp, 23))
               ^ ROTATE_LEFT(W[j-13], 7) 
               ^ W[j-6];
    }
    
    // 第三步：生成W'[0..63]
    for (j = 0; j < 64; j++) {
        W1[j] = W[j] ^ W[j+4];
    }

    // 初始化工作变量
    A = state[0]; B = state[1]; C = state[2]; D = state[3];
    E = state[4]; F = state[5]; G = state[6]; H = state[7];

    // 64轮迭代
    for (j = 0; j < 64; j++) {
        // 选择T常量
        if (j < 16) {
            T = 0x79CC4519;
        } else {
            T = 0x7A879D8A;
        }
        
        SS1 = ROTATE_LEFT(ROTATE_LEFT(A, 12) + E + ROTATE_LEFT(T, j), 7);
        SS2 = SS1 ^ ROTATE_LEFT(A, 12);
        
        if (j < 16) {
            // FF1
            TT1 = (A ^ B ^ C) + D + SS2 + W1[j];
            // GG1
            TT2 = (E ^ F ^ G) + H + SS1 + W[j];
        } else {
            // FF2
            TT1 = ((A & B) | (A & C) | (B & C)) + D + SS2 + W1[j];
            // GG2
            TT2 = ((E & F) | ((~E) & G)) + H + SS1 + W[j];
        }
        
        D = C;
        C = ROTATE_LEFT(B, 9);
        B = A;
        A = TT1;
        H = G;
        G = ROTATE_LEFT(F, 19);
        F = E;
        // P0置换
        E = TT2 ^ ROTATE_LEFT(TT2, 9) ^ ROTATE_LEFT(TT2, 17);
    }

    // 更新状态
    state[0] ^= A; state[1] ^= B; state[2] ^= C; state[3] ^= D;
    state[4] ^= E; state[5] ^= F; state[6] ^= G; state[7] ^= H;
}

// SM3哈希主函数
void sm3_hash(const unsigned char *input, int input_len, unsigned char output[SM3_DIGEST_SIZE]) {
    uint32_t state[8];
    unsigned char buffer[64];
    uint64_t bit_len;
    int i, pad_len;
    
    // 初始化状态
    memcpy(state, IV, sizeof(IV));
    bit_len = (uint64_t)input_len * 8;
    
    // 处理完整分组
    while (input_len >= 64) {
        compress(state, input);
        input += 64;
        input_len -= 64;
    }
    
    // 处理最后一个分组（填充）
    memcpy(buffer, input, input_len);
    buffer[input_len] = 0x80;
    
    if (input_len <= 55) {
        pad_len = 55 - input_len;
        memset(buffer + input_len + 1, 0, pad_len);
    } else {
        pad_len = 119 - input_len;
        memset(buffer + input_len + 1, 0, pad_len);
        compress(state, buffer);
        memset(buffer, 0, 56);
    }
    
    // 添加长度（大端序）
    for (i = 0; i < 8; i++) {
        buffer[56 + i] = (unsigned char)((bit_len >> (56 - 8 * i)) & 0xFF);
    }
    compress(state, buffer);
    
    // 输出结果（大端序）
    for (i = 0; i < 8; i++) {
        output[i * 4] = (unsigned char)((state[i] >> 24) & 0xFF);
        output[i * 4 + 1] = (unsigned char)((state[i] >> 16) & 0xFF);
        output[i * 4 + 2] = (unsigned char)((state[i] >> 8) & 0xFF);
        output[i * 4 + 3] = (unsigned char)(state[i] & 0xFF);
    }
}

// 打印哈希值到控制台
void sm3_print_hash(const unsigned char hash[SM3_DIGEST_SIZE]) {
    int i;
    for (i = 0; i < SM3_DIGEST_SIZE; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}

// 将哈希值转换为字符串
void sm3_hash_to_string(const unsigned char hash[SM3_DIGEST_SIZE], char* output_string) {
    int i;
    for (i = 0; i < SM3_DIGEST_SIZE; i++) {
      sprintf(output_string + i * 2, "%02x", hash[i]);
    }
    output_string[SM3_DIGEST_SIZE * 2] = '\0';
}
