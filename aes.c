#include "aes.h"

const int EXPANDED_KEY_SIZE = 176;

// Implementation: S-Box
unsigned char SBOX[256] = {
    // 0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,  // 0
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,  // 1
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,  // 2
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,  // 3
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,  // 4
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,  // 5
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,  // 6
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,  // 7
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,  // 8
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,  // 9
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,  // A
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,  // B
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,  // C
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,  // D
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,  // E
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16   // F
};

// From S-Box, we derive the inverse S-Box
unsigned char INV_SBOX[256] = {
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
    0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
    0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
    0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
    0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
    0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
    0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
    0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
    0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
    0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
    0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
    0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
    0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
    0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
    0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
    0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
};


// Implementation: RCON
unsigned char RCON[255] = {
    0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8,
    0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3,
    0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f,
    0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d,
    0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab,
    0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d,
    0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25,
    0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01,
    0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d,
    0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa,
    0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a,
    0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02,
    0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a,
    0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef,
    0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94,
    0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04,
    0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f,
    0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5,
    0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33,
    0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb
};

// Implement Galois Field Multiplication
unsigned char gf_mul(unsigned char a, unsigned char b)
{
    unsigned char p = 0;
    unsigned char counter;
    unsigned char hi_bit_set;
    for (counter = 0; counter < 8; counter++)
    {
        if ((b & 1) == 1)
            p ^= a;
        hi_bit_set = (a & 0x80);
        a <<= 1;
        if (hi_bit_set == 0x80)
            a ^= 0x1b;
        b >>= 1;
    }
    return p;
}

// Functions to get the value of Sbox index and invSbox index
unsigned char getSBox(unsigned char num) {
    return SBOX[num];
}
unsigned char getSBoxInv(unsigned char num) {
    return INV_SBOX[num];
}

/* Rijndael's key schedule rotate operation
rotate the word eight bits to the left */
void rotate(unsigned char *word) {
    // word = n1_n2_n3_n4
    unsigned char c;
    c = word[0];
    for (int i = 0; i < 3; i++)
        word[i] = word[i + 1];
    word[3] = c;
    // word = n2_n3_n4_n1
}

unsigned char getRCON(unsigned char num) {
    return RCON[num];
}

// Rijndael's Key Schedule Core
void core(unsigned char *word, int iteration)
{
    // rotate left 8 bits
    rotate(word);
    // SBox substitution
    for (int i = 0; i < 4; ++i)
        word[i] = getSBox(word[i]);
    // XOR the output of the rcon operation with i to the first part (leftmost) only
    word[0] = word[0] ^ getRCON(iteration);
}

// Key Expansion
void expandKey(unsigned char *expandedKey, unsigned char *key, enum keySize size, size_t expandedKeySize) {
    
    // current expanded keySize, in bytes
    int currentSize = 0;
    int rconIteration = 1;
    int i;
    unsigned char t[4] = {0}; // tempo 4-byte variable

    // set the 16,24,32 bytes of the expanded key to the input key
    for (i = 0; i < size; i++)
        expandedKey[i] = key[i];
    currentSize += size;

    while (currentSize < expandedKeySize)
    {
        // assign the previous 4 bytes to the temporary value t
        for (i = 0; i < 4; i++)
        {
            t[i] = expandedKey[(currentSize - 4) + i];
        }

        /* every 16,24,32 bytes we apply the core schedule to t
         * and increment rconIteration afterwards
         */
        if (currentSize % size == 0)
        {
            core(t, rconIteration++);
        }

        // For 256-bit keys, we add an extra sbox to the calculation
        if (size == SIZE_32 && ((currentSize % size) == 16))
        {
            for (i = 0; i < 4; i++)
                t[i] = getSBox(t[i]);
        }

        /* We XOR t with the four-byte block 16,24,32 bytes before the new expanded key.
         * This becomes the next four bytes in the expanded key.
         */
        for (i = 0; i < 4; i++)
        {
            expandedKey[currentSize] = expandedKey[currentSize - size] ^ t[i];
            currentSize++;
        }
    }
}

// Create Round Key
void create_round_key(unsigned char *expanded_key, unsigned char *round_key)
{
    // iterate over the columns
    for (int i = 0; i < 4; i++)
    {
        // iterate over the rows
        for (int j = 0; j < 4; j++)
            round_key[(i + (j * 4))] = expanded_key[(i * 4) + j];
    }
}

// sub_bytes
void sub_bytes(unsigned char *state) {
    for (int i = 0; i < 16; i++)
        state[i] = getSBox(state[i]);
}

// inv_sub_bytes
void inv_sub_bytes(unsigned char *state) {
    for (int i = 0; i < 16; i++)
        state[i] = getSBoxInv(state[i]);
}

// Shift rows
void shift_rows(unsigned char *state)
{
    for (int i = 0; i < 4; i++)
        shift_row(state + i * 4, i);
}

void shift_row(unsigned char *state, unsigned char r)
{
    int i, j;
    unsigned char tmp;
    // each iteration shifts the row to the left by 1
    for (i = 0; i < r; i++)
    {
        tmp = state[0];
        for (j = 0; j < 3; j++)
            state[j] = state[j + 1];
        state[3] = tmp;
    }
}

// Inverse Shift Rows
void inv_shift_rows(unsigned char *state)
{
    for (int i = 0; i < 4; i++)
        inv_shift_row(state + i * 4, i);
}

void inv_shift_row(unsigned char *state, unsigned char r)
{
    int i, j;
    unsigned char tmp;
    // each iteration shifts the row to the right by 1
    for (i = 0; i < r; i++)
    {
        tmp = state[3];
        for (j = 3; j > 0; j--)
            state[j] = state[j - 1];
        state[0] = tmp;
    }
}

// Mix Column
void mix_columns(unsigned char *state)
{
    int i, j;
    unsigned char column[4];
    // iterate over the 4 columns
    for (i = 0; i < 4; i++)
    {
        // construct one column by iterating over the 4 rows
        for (j = 0; j < 4; j++)
            column[j] = state[(j * 4) + i];
        // apply the mixColumn on one column
        mix_column(column);
        // put the values back into the state
        for (j = 0; j < 4; j++)
            state[(j * 4) + i] = column[j];

    }
}

void mix_column(unsigned char *column)
{
    unsigned char cpy[4];
    for (int i = 0; i < 4; i++)
        cpy[i] = column[i];

    column[0] = gf_mul(cpy[0], 2) ^
                gf_mul(cpy[3], 1) ^
                gf_mul(cpy[2], 1) ^
                gf_mul(cpy[1], 3);

    column[1] = gf_mul(cpy[1], 2) ^
                gf_mul(cpy[0], 1) ^
                gf_mul(cpy[3], 1) ^
                gf_mul(cpy[2], 3);

    column[2] = gf_mul(cpy[2], 2) ^
                gf_mul(cpy[1], 1) ^
                gf_mul(cpy[0], 1) ^
                gf_mul(cpy[3], 3);

    column[3] = gf_mul(cpy[3], 2) ^
                gf_mul(cpy[2], 1) ^
                gf_mul(cpy[1], 1) ^
                gf_mul(cpy[0], 3);
}

// Inverse Mix Columns
void inv_mix_columns(unsigned char *state)
{
    int i, j;
    unsigned char column[4];

    // iterate over the 4 columns
    for (i = 0; i < 4; i++)
    {
        // construct one column by iterating over the 4 rows
        for (j = 0; j < 4; j++)
            column[j] = state[(j * 4) + i];
        // apply the invMixColumn on one column
        inv_mix_column(column);
        // put the values back into the state
        for (j = 0; j < 4; j++)
            state[(j * 4) + i] = column[j];
    }
}

void inv_mix_column(unsigned char *column)
{
    unsigned char cpy[4];
    for (int i = 0; i < 4; i++)
        cpy[i] = column[i];
    column[0] = gf_mul(cpy[0], 14) ^
                gf_mul(cpy[3], 9) ^
                gf_mul(cpy[2], 13) ^
                gf_mul(cpy[1], 11);

    column[1] = gf_mul(cpy[1], 14) ^
                gf_mul(cpy[0], 9) ^
                gf_mul(cpy[3], 13) ^
                gf_mul(cpy[2], 11);

    column[2] = gf_mul(cpy[2], 14) ^
                gf_mul(cpy[1], 9) ^
                gf_mul(cpy[0], 13) ^
                gf_mul(cpy[3], 11);

    column[3] = gf_mul(cpy[3], 14) ^
                gf_mul(cpy[2], 9) ^
                gf_mul(cpy[1], 13) ^
                gf_mul(cpy[0], 11);
}

// Add Round Key
void add_round_key(unsigned char *state, unsigned char *round_key)
{
    for (int i = 0; i < 16; i++)
        state[i] = state[i] ^ round_key[i];
}

// AES round
void aes_round(unsigned char *state, unsigned char *round_key) {
    sub_bytes(state);
    shift_rows(state);
    mix_columns(state);
    add_round_key(state, round_key);
}

// AES inverse round
void aes_inv_round(unsigned char *state, unsigned char *round_key) {
    inv_shift_rows(state);
    inv_sub_bytes(state);
    add_round_key(state, round_key);
    inv_mix_columns(state);
}

// Main AES Body
void aes_main(unsigned char *state, unsigned char *expanded_key, int no_round) {

    unsigned char round_key[16];
    create_round_key(expanded_key, round_key);
    add_round_key(state, round_key);

    for (int i = 1; i < no_round; i++)
    {
        create_round_key(expanded_key + 16 * i, round_key);
        aes_round(state, round_key);
    }

    // Last round will not mix the columns
    create_round_key(expanded_key + 16 * no_round, round_key);
    sub_bytes(state);
    shift_rows(state);
    add_round_key(state, round_key);
}

// Main AES Body for inverse
void aes_inv_main(unsigned char *state, unsigned char *expanded_key, int no_round) {

    unsigned char round_key[16];
    create_round_key(expanded_key + 16 * no_round, round_key);
    add_round_key(state, round_key);

    for (int i = no_round - 1; i > 0; i--)
    {
        create_round_key(expanded_key + 16 * i, round_key);
        aes_inv_round(state, round_key);
    }

    // The last round will not mix the columns
    create_round_key(expanded_key, round_key);
    inv_shift_rows(state);
    inv_sub_bytes(state);
    add_round_key(state, round_key);
}


// Main function
char aes_encrypt(unsigned char *pt, unsigned char *ct, unsigned char *key, enum keySize size)
{
    // the expanded key size
    int expandedKeySize;
    // # rounds
    int no_round;
    // the expanded key
    unsigned char *expandedKey;
    // the 128 bit block to encode
    unsigned char block[16];

    int i, j;
    // set the number of rounds
    switch (size)
    {
    case SIZE_16:
        no_round = 10;
        break;
    case SIZE_24:
        no_round = 12;
        break;
    case SIZE_32:
        no_round = 14;
        break;
    default:
        return AES_KEYSIZE_MISMATCH;
        break;
    }

    expandedKeySize = (16 * (no_round + 1));
    expandedKey = (unsigned char *)malloc(expandedKeySize * sizeof(unsigned char));

    if (expandedKey == NULL)
        return MEMORY_ALLOCATION_FAILED;
    else
    {
        /* Set the block values, for the block:
         * a0,0 a0,1 a0,2 a0,3
         * a1,0 a1,1 a1,2 a1,3
         * a2,0 a2,1 a2,2 a2,3
         * a3,0 a3,1 a3,2 a3,3
         * the mapping order is a0,0 a1,0 a2,0 a3,0 a0,1 a1,1 ... a2,3 a3,3
         */
        // iterate over the columns
        for (i = 0; i < 4; i++)
        {
            // iterate over the rows
            for (j = 0; j < 4; j++)
                block[(i + (j * 4))] = pt[(i * 4) + j];
        }
        // expand the key into an 176, 208, 240 bytes key
        expandKey(expandedKey, key, size, expandedKeySize);
        // encrypt the block using the expandedKey
        aes_main(block, expandedKey, no_round);
        // unmap the block again into the output
        for (i = 0; i < 4; i++)
        {
            // iterate over the rows
            for (j = 0; j < 4; j++)
                ct[(i * 4) + j] = block[(i + (j * 4))];
        }
        // de-allocate memory for expandedKey
        free(expandedKey);
        expandedKey = NULL;
    }

    return SUCCESS;
}

// Main function for AES decryption
char aes_decrypt(unsigned char *ct, unsigned char *pt, unsigned char *key, enum keySize size) {
    
    // the expanded key size
    int expandedKeySize;
    // # rounds
    int no_round;
    // the expanded key
    unsigned char *expandedKey;
    // the 128 bit block to encode
    unsigned char block[16];

    int i, j;
    // set the number of rounds
    switch (size)
    {
    case SIZE_16:
        no_round = 10;
        break;
    case SIZE_24:
        no_round = 12;
        break;
    case SIZE_32:
        no_round = 14;
        break;
    default:
        return AES_KEYSIZE_MISMATCH;
        break;
    }

    expandedKeySize = (16 * (no_round + 1));
    expandedKey = (unsigned char *)malloc(expandedKeySize * sizeof(unsigned char));

    if (expandedKey == NULL)
        return MEMORY_ALLOCATION_FAILED;
    else
    {
        /* Set the block values, for the block:
         * a0,0 a0,1 a0,2 a0,3
         * a1,0 a1,1 a1,2 a1,3
         * a2,0 a2,1 a2,2 a2,3
         * a3,0 a3,1 a3,2 a3,3
         * the mapping order is a0,0 a1,0 a2,0 a3,0 a0,1 a1,1 ... a2,3 a3,3
         */
        // iterate over the columns
        for (i = 0; i < 4; i++)
        {
            // iterate over the rows
            for (j = 0; j < 4; j++)
                block[(i + (j * 4))] = ct[(i * 4) + j];
        }
        // expand the key into an 176, 208, 240 bytes key
        expandKey(expandedKey, key, size, expandedKeySize);
        // encrypt the block using the expandedKey
        aes_inv_main(block, expandedKey, no_round);
        // unmap the block again into the output
        for (i = 0; i < 4; i++)
        {
            // iterate over the rows
            for (j = 0; j < 4; j++)
                pt[(i * 4) + j] = block[(i + (j * 4))];
        }
        // de-allocate memory for expandedKey
        free(expandedKey);
        expandedKey = NULL;
    }

    return SUCCESS;
}

// Misc function
unsigned char * random_block(int block_len) {
    unsigned char *block = (unsigned char *)malloc(block_len * sizeof(unsigned char));
    for (int i = 0; i < block_len; i++) 
        block[i] = rand() % 256;
    return block;
}

void rand_init() {
    srand(time(NULL));
}

void print_block(unsigned char *block, int block_len) {
    for (int i = 0; i < block_len; i++)
    {
        // Print characters in HEX format, 16 chars per line
        printf("%2.2x\0", block[i]);
    }
    printf("\n");
}

int calc_pad_ammount_aes128(int data_len) {
    for (int i = 0; i < 16; i++)
        if ((data_len + i) % 16 == 0)
            return (i == 0) ? 16 : i;
}

int length_after_pad(int data_len) {
    return data_len + calc_pad_ammount_aes128(data_len);
}

unsigned char * pad(unsigned char *data, int data_len) {

    int pad_amount = calc_pad_ammount_aes128(data_len);
    int new_size = pad_amount + data_len;
    unsigned char *pad_block;
    pad_block = (unsigned char *) malloc(new_size * sizeof(unsigned char));
    
    // Reassign
    for (int i = 0; i < data_len; i++) {
        pad_block[i] = data[i];
    }
    
    // Calc pad byte 
    int pad_byte = (pad_amount % 16 != 0) ? pad_amount : 16;

    // Pad
    for (int i = data_len; i < new_size; i++)
        pad_block[i] = pad_byte;
    
    return pad_block; 
}

// AES-CBC using AES128 with an additional Initialization Vector (IV)
// The plaintext should have the length that is a multiple of 16
unsigned char * aes_cbc_encrypt(unsigned char *pt, int pt_len, unsigned char *key, unsigned char *iv) {

    int pt_pad_len = length_after_pad(pt_len);
    unsigned char *pt_pad = pad(pt, pt_len);

    int num_block = pt_pad_len / 16;

    // State
    unsigned char *state;
    state = (unsigned char *) malloc(16 * sizeof(unsigned char));
    // Intermediate buffer
    unsigned char *buf;
    buf = (unsigned char *) malloc(16 * sizeof(unsigned char));
    // Ciphertext
    unsigned char *ct;
    ct = (unsigned char *) malloc(pt_pad_len * sizeof(unsigned char));


    // Initialize first block of ciphertext
    for (int i = 0; i < 16; i++) 
        state[i] = pt_pad[i];
    // Xor with IV
    for (int i = 0; i < 16; i++)
        state[i] = state[i] ^ iv[i];
    // Encrypt the first block with AES
    char x = aes_encrypt(state, buf, key, 16);
    // Enc block and update state
    for (int i = 0; i < 16; i++) {
        ct[i] = buf[i];
        state[i] = buf[i];
    }

    // For subsequent blocks
    for (int block = 1; block < num_block; block++) {

        // Xor with the plaintext block i
        for (int i = 0; i < 16; i++)
            state[i] = state[i] ^ pt_pad[block * 16 + i];
        // Encrypt
        char x = aes_encrypt(state, buf, key, 16);
        // Enc block and update state
        for (int i = 0; i < 16; i++) {
            ct[block * 16 + i] = buf[i];
            state[i] = buf[i];
        }

    }
    return ct;
}

// AES-CBC using AES128 with an additional Initialization Vector (IV)
// The ciphertext should have the length that is a multiple of 16
unsigned char * aes_cbc_decrypt(unsigned char *ct, int ct_len, unsigned char *key, unsigned char *iv) {

    int num_block = ct_len / 16;

    // State
    unsigned char *state;
    state = (unsigned char *) malloc(16 * sizeof(unsigned char));
    // Intermediate buffer
    unsigned char *buf;
    buf = (unsigned char *) malloc(16 * sizeof(unsigned char));
    // Ciphertext
    unsigned char *pt;
    pt = (unsigned char *) malloc(ct_len * sizeof(unsigned char));

    // Initialize first block of ciphertext
    for (int i = 0; i < 16; i++) 
        state[i] = ct[i];
    // Decrypt the first block with AES
    char x = aes_decrypt(state, buf, key, 16);
    // Xor with IV
    for (int i = 0; i < 16; i++)
        buf[i] = buf[i] ^ iv[i];
    // Decrypt block and update state
    for (int i = 0; i < 16; i++) {
        pt[i] = buf[i];
        state[i] = ct[16 + i];
    }

    // For subsequent blocks
    for (int block = 1; block < num_block; block++) {
        // Decrypt
        char x = aes_decrypt(state, buf, key, 16);
        // Xor with previous ciphertext block i
        for (int i = 0; i < 16; i++)
            buf[i] = buf[i] ^ ct[(block - 1) * 16 + i];
        // Decrypt block and update state
        for (int i = 0; i < 16; i++) {
            pt[block * 16 + i] = buf[i];
            state[i] = ct[(block + 1) * 16 + i];
        }

    }

    return unpad(pt, ct_len);
}

// Unpad a data stream
unsigned char *unpad(unsigned char *data, int data_len) {

    // Check the last char of the data, which
    // reveals the pad bytes
    // and also the padding amount
    int pad_amount = data[data_len - 1];

    int original_size = data_len - pad_amount;
    // Create new data, which trims off some of the last padding bytes
    unsigned char *data_unpad = (unsigned char *) malloc(original_size * sizeof(unsigned char));

    // Assign
    for (int i = 0; i < original_size; i++)
        data_unpad[i] = data[i];
    
    return data_unpad;
}

int length_after_unpad(unsigned char *data_pad, int data_len) {
    return data_len - data_pad[data_len - 1];
}

void test_pad_and_unpad(int round) {

    for (int i = 0; i < round; i++) {

        rand_init();

        // Random length
        int l = rand() % 150 + 16;

        unsigned char *data = random_block(l);

        // Need to know the length after padding
        int l_pad = length_after_pad(l);
        unsigned char *data_pad = pad(data, l);

        // If unpad succeed, the length should be l
        unsigned char *data_unpad = unpad(data_pad, l_pad);

        if (!check_two_arrays(data, data_unpad, l)) 
            printf("Test failed at testcase %d\n", i);    
    }
    printf("Test done\n");
    return;
}

void test_aes(int round) {

    rand_init();

    for (int i = 0; i < round; i++) {
        // Generate random key
        unsigned char *key = random_block(16);
        // the plaintext
        unsigned char *plaintext = random_block(16);
        // Ciphertext
        unsigned char ciphertext[16];
        // Recovered plaintext
        unsigned char recovered[16];
        // Encrypt the plaintext using AES128
        aes_encrypt(plaintext, ciphertext, key, 16);
        // Decrypt the ciphertext using AES128
        aes_decrypt(ciphertext, recovered, key, 16);
        if (!check_two_arrays(plaintext, recovered, 16)) {
            printf("Test failed at testcase %d\n", i + 1);
            return;
        }
    }
    printf("All testcase succeed\n");
    return;
}

void test_aes_cbc(int round, int plaintext_len) {

    rand_init();

    for (int i = 0; i < round; i++) {
        // Generate random key
        unsigned char *key = random_block(16);
        // Generate random iv
        unsigned char *iv = random_block(16);
        // The plaintext, must pad before using
        unsigned char *plaintext = random_block(plaintext_len);

        int ct_len = length_after_pad(plaintext_len);
        // Encrypt the plaintext using AES128-CBC
        unsigned char* ciphertext = aes_cbc_encrypt(plaintext, plaintext_len, key, iv);
        // Decrypt the ciphertext using AES128-CBC
        unsigned char* recovered = aes_cbc_decrypt(ciphertext, ct_len, key, iv);

        if (!check_two_arrays(plaintext, recovered, plaintext_len)) {
            printf("Test failed at testcase %d\n", i + 1);
            return;
        }
    }
    printf("All testcase succeed\n");
    return;
}

bool check_two_arrays(unsigned char *a1, unsigned char *a2, int l) {
    for (int i = 0; i < l; i++)
        if (a1[i] != a2[i])
            return false;
    return true;
}