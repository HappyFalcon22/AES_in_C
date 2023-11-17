#ifndef _AES_H_
#define _AES_H_

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>

enum keySize
{
    SIZE_16 = 16,
    SIZE_24 = 24,
    SIZE_32 = 32
};

enum Error
{
    SUCCESS = 0,
    AES_KEYSIZE_MISMATCH,
    MEMORY_ALLOCATION_FAILED,
};

extern const int EXPANDED_KEY_SIZE;

// Implementation: S-Box
extern unsigned char SBOX[256];
// Implementation: Inverse S-Box
extern unsigned char INV_SBOX[256];
// Implementation: RCON
extern unsigned char RCON[255];


// Functions to get the value of Sbox index and invSbox index
unsigned char getSBox(unsigned char num);
unsigned char getSBoxInv(unsigned char num);

// Implementation: Rotate
void rotate(unsigned char *word);

// Impl : get value from RCON
unsigned char getRCON(unsigned char num);

// Key Schedule Core
void core(unsigned char *word, int iteration);

// Key Expansion
void expandKey(unsigned char *expandedKey, unsigned char *key, enum keySize, size_t expandedKeySize);

// Supporting functions for AES Encryption

// sub_bytes
void sub_bytes(unsigned char *state);

// shift_rows
void shift_rows(unsigned char *state);
void shift_row(unsigned char *state, unsigned char r);

// Multiplication under Galois field GF(256)
unsigned char gf_mul(unsigned char a, unsigned char b);

// mix_columns
void mix_columns(unsigned char *state);
void mix_column(unsigned char *col);

// Implement a round in AES
void aes_round(unsigned char *state, unsigned char *round_key);

// The main AES body
void create_round_key(unsigned char *expanded_key, unsigned char *round_key);
void aes_main(unsigned char *state, unsigned char *expanded_key, int no_round);

// The main function for encryption
char aes_encrypt(unsigned char *pt, unsigned char *ct, unsigned char *key, enum keySize size);

// Supporting function for AES decryption

// sub_bytes
void inv_sub_bytes(unsigned char *state);

// shift_rows
void inv_shift_rows(unsigned char *state);
void inv_shift_row(unsigned char *state, unsigned char r);

// mix_columns
void inv_mix_columns(unsigned char *state);
void inv_mix_column(unsigned char *column);

// Implementation for an AES decryption rounf
void aes_inv_round(unsigned char *state, unsigned char *round_key);

// Main AES body
void aes_inv_main(unsigned char *state, unsigned char *expanded_key, int no_round);

// Main function for AES decryption
char aes_decrypt(unsigned char *ct, unsigned char *pt, unsigned char *key, enum keySize size);

// Implementation: addRoundKey
void add_round_key(unsigned char *state, unsigned char *round_key);

// Some misc functions
void rand_init();


// Random function and display function
unsigned char * random_block(int block_len);
void print_block(unsigned char *block, int block_len);

// Padding function, return data_len + pad_ammount = multiple of 16
int calc_pad_ammount(int data_len);

// Padding functions and misc functions
unsigned char * pad(unsigned char *block, int block_len);
unsigned char * unpad(unsigned char *data, int data_len);
int length_after_unpad(unsigned char *data_pad, int data_len);
int length_after_pad(int data_len);

// AES-CBC using AES128 with an additional Initialization Vector (IV)
unsigned char * aes_cbc_encrypt(unsigned char *pt, int pt_len, unsigned char *key, unsigned char *iv);
unsigned char * aes_cbc_decrypt(unsigned char *ct, int ct_len, unsigned char *key, unsigned char *iv);

// Test functions
void test_aes(int round);
void test_aes_cbc(int round, int plaintext_len);
void test_pad_and_unpad(int round);
bool check_two_arrays(unsigned char *a1, unsigned char *a2, int l);


#endif
