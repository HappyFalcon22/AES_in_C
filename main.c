#include "aes.h"

/// @brief This is an example to run AES128-CBC Encryption and Decryption
/// @param plaintext : Your plaintext, prefered to be meaningful
/// @param plaintext_len : The length of the plaintext
void example_aes128_cbc(uint8_t* plaintext, int plaintext_len) {

    // Initialize the seed for random oracle
    rand_init();

    // Generate a random key and iv
    uint8_t *key = random_block(16);
    uint8_t *iv = random_block(16);

    // Find the length after pad (ciphertext_len)
    int ciphertext_len = length_after_pad(plaintext_len);

    // Use AES128-CBC to encrypt your plaintext pad
    uint8_t *ct = aes_cbc_encrypt(plaintext, plaintext_len, key, iv);
    
    // Use AES128-CBC to decrypt your ciphertext
    uint8_t *recovered = aes_cbc_decrypt(ct, ciphertext_len, key, iv);
    // Print your plaintext
    for (int i = 0; i < plaintext_len; i++)
    {
        // Print characters in HEX format, 16 chars per line
        printf("%c\0", recovered[i]);
    }
    printf("\n");
    return;
}

void example_encrypt_to_python_file() {

    rand_init();

    uint8_t *key = random_block(16);
    uint8_t *iv = random_block(16);

    uint8_t pt[32] = {'A', 'E', 'S', '-', 'C', 'B', 'C', ' ', 'u', 's', 'e', 'd', ' ', 'f', 'o', 'r', ' ', 'i', 'm', 'a', 'g', 'e', ' ', 's', 'e', 'c', 'u', 'r', 'i', 't', 'y', '.'};

    uint8_t *ct = aes_cbc_encrypt(pt, 32, key, iv);

    int length_after_encrypt = length_after_pad(32);

    FILE *fptr;

    // Open a file in writing mode
    fptr = fopen("output.py", "w");

    // Write key
    fprintf(fptr, "key = bytes.fromhex('");
    for (int i = 0; i < 16; i++) 
        fprintf(fptr, "%02x", key[i]);
    fprintf(fptr, "')\n");

    // Write iv
    fprintf(fptr, "iv = bytes.fromhex('");
    for (int i = 0; i < 16; i++) 
        fprintf(fptr, "%02x", iv[i]);
    fprintf(fptr, "')\n");

    // Write ciphertext
    fprintf(fptr, "ct = bytes.fromhex('");
    for (int i = 0; i < length_after_encrypt; i++) 
        fprintf(fptr, "%02x", ct[i]);
    fprintf(fptr, "')\n");

    // Close the file
    fclose(fptr);
}

int main() {

    example_encrypt_to_python_file();
    return 0;
}