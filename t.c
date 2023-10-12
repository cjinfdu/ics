#include <stdio.h>
int main()
{
  int version;
  __asm__("mov %0, 2023\n\t"
         :"=r"(version)  /* output */
  );

  printf("Hello, ICS %d!\n", version);
}
