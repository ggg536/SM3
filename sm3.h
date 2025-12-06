#ifndef SM3_H
#define SM3_H

#include <stdint.h>

#define SM3_DIGEST_SIZE 32

void sm3_hash(const unsigned char *input, int input_len, unsigned char output[SM3_DIGEST_SIZE]);
void sm3_print_hash(const unsigned char hash[SM3_DIGEST_SIZE]);
void sm3_hash_to_string(const unsigned char hash[SM3_DIGEST_SIZE], char* output_string);

#endif
