#include "aes.h"

/// @brief This is an example to run AES128-CBC Encryption and Decryption
/// @param plaintext : Your plaintext, prefered to be meaningful
/// @param plaintext_len : The length of the plaintext
void example_aes128_cbc(unsigned char* plaintext, int plaintext_len) {

    // Initialize the seed for random oracle
    rand_init();

    // Generate a random key and iv
    unsigned char *key = random_block(16);
    unsigned char *iv = random_block(16);

    // Find the length after pad (ciphertext_len)
    int ciphertext_len = length_after_pad(plaintext_len);

    // Use AES128-CBC to encrypt your plaintext pad
    unsigned char *ct = aes_cbc_encrypt(plaintext, plaintext_len, key, iv);
    
    // Use AES128-CBC to decrypt your ciphertext
    unsigned char *recovered = aes_cbc_decrypt(ct, ciphertext_len, key, iv);
    // Print your plaintext
    for (int i = 0; i < plaintext_len; i++)
    {
        // Print characters in HEX format, 16 chars per line
        printf("%c\0", recovered[i]);
    }
    printf("\n");
    return;
}

int main() {
    test_aes(10000);
    test_aes_cbc(100, 100);
    test_pad_and_unpad(10000);
    return 0;
}