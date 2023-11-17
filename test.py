from Crypto.Cipher import AES

Key = bytes.fromhex("a06a89670a9bf17f10e2500bba3985f7")
IV = bytes.fromhex("950c21215649290ec1b718dcc9882477")
Plaintext = bytes.fromhex("f931c21012796ccbf6d0a2b8c4e32f05456dad7287504b04c7e4cefe948fd60d")

cipher = AES.new(Key, AES.MODE_CBC, iv=IV)
ciphertext = cipher.encrypt(Plaintext)

print(bytes.hex(ciphertext))