//Written by: Joshua Bittel
//Cipher Block Chaining Stream Cipher

#include "../common.h"
#include <stdio.h>

//argv: configFile

int main(int argc, char **argv){
  //char * blockfunc_name;
  FILE *fp;
  char title[255];
  char value[255];
  char action;
  char filein[255];
  char blockfunc[255];
  int keylen;
  int func_index;
  //void (*func[2])(const uint8_t*, const uint8_t*, const uint8_t*, const uint32_t) = {AES_encrypt, AES_decrypt};
  uint8_t * key;
  
  uint8_t key128[16] =        { (uint8_t) 0x2b, (uint8_t) 0x7e, (uint8_t) 0x15, (uint8_t) 0x16, (uint8_t) 0x28, (uint8_t) 0xae, (uint8_t) 0xd2, (uint8_t) 0xa6, (uint8_t) 0xab, (uint8_t) 0xf7, (uint8_t) 0x15, (uint8_t) 0x88, (uint8_t) 0x09, (uint8_t) 0xcf, (uint8_t) 0x4f, (uint8_t) 0x3c };
  uint8_t key192[] = { 0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52, 0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
		       0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b };
  uint8_t key256[] = { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
		       0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4 };

  uint8_t iv[16] = {0x3a, 0x5a, 0x68, 0xb6, 0xb0, 0x66, 0x5b, 0x2e, 0xe2, 0xc0, 0xac, 0x59, 0xf8, 0x4e, 0x0a, 0x48};
  
  
  //8 byte with appended 8 byte counter == 16 byte cipher iv
  
  if (argc < 2){
    printf("Usage: ./cbc configFile\n");
    exit(EXIT_FAILURE);
  }

  fp = fopen(argv[1], "r");
  while(fscanf(fp, "%s %s", title, value) != EOF){
    if (!strcmp(title, "action")){
      action = value[0];
    }
    else if (!strcmp(title, "blockfunc")){
      strcpy(blockfunc, value);
    }
    else if (!strcmp(title, "keylen")){
      keylen = atoi(value);
    }
    else if (!strcmp(title, "filein")){
      strcpy(filein, value);
    }
  }

  if (!strcmp(blockfunc, "aes")){
    func_index = 0;
  }
  else if (!strcmp(blockfunc, "test1")){
    func_index = 0;
  }
  else if (!strcmp(blockfunc, "test2")){
    func_index = 0;
  }

  
  if (keylen == 128){
    key = key128;
  }
  else if (keylen == 192){
    key = key192;
  }
    else{
    key = key256;
  }
  
  //TODO: do a switch based on blockCipherType
  
  
  char fileout[255];
  if (action == 'e')
    snprintf(fileout, sizeof(fileout), "%s.encrypt", filein);
  if (action == 'd')
    snprintf(fileout, sizeof(fileout), "%s.decrypt", filein);
  uint8_t buffer[16];
  uint8_t ciphered[16];
  memset(buffer, 1, 16*sizeof(buffer[0]));
  memset(ciphered, 1, 16*sizeof(ciphered[0]));
  FILE* file_open, * out_open;
  file_open = fopen(filein,"r");  // r for read, b for binary
  out_open = fopen(fileout, "w");

  
  while(fread(buffer, 1, 16, file_open)){
    
    if (action == 'e'){
      //call usage function here ie: encrypt, decrypt, sign, authenticate
      uint8_t i;
      for(i = 0; i < 16; ++i){
	buffer[i] ^= iv[i];
      }
      
      AES_encrypt(buffer, key, ciphered, keylen/8);
      memcpy(iv, ciphered, sizeof(iv));
    }
    else if (action == 'd'){
      AES_decrypt(buffer, key, ciphered, keylen/8);
      uint8_t i;
      for(i = 0; i < 16; ++i){
	ciphered[i] ^= iv[i];
      }
      memcpy(iv, buffer, sizeof(iv));
    }
    
    /*for (i = 0; i < 16; ++i)
      printf("%x", ciphered[i]);
      printf("\n");*/
    size_t count_write = fwrite(ciphered, 1, 16, out_open);
    if (count_write != 16){
      perror("Write error\n");
    }

    //prepare buffer and cipher for next round
    memset(buffer, 1, 16*sizeof(buffer[0]));
    memset(ciphered, 1, 16*sizeof(ciphered[0]));
  }
  
  fclose(file_open);
  fclose(out_open);
  
  return 0;
}
