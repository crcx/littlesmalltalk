# include "env.h"
main() {
  int i;
  uchar c;

   i = 250;
   c = itouc(i);
   i = uctoi(c);
   if (i == 250) printf("success\n");
   else printf("failure\n");
}

