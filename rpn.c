/*============================----beg-of-source---============================*/
#include  "yRPN.h"
#include  "yRPN_priv.h"

#define     LEN_RECD     2000

/*> char        s_rpn        [LEN_RECD];                                              <*/
/*> char        s_tokens     [LEN_RECD];                                              <*/

char
main               (int argv, char **argc)
{
   char        rc          =    0;
   char       *x_rpn       = NULL;
   int         x_nrpn      = NULL;
   if (argv < 1) {
      printf ("must pass a infix formula string\n");
      return -1;
   }
   rc = yRPN_spreadsheet (argc [1], &x_rpn, &x_nrpn, LEN_RECD, 0);
   if (rc < 0) {
      printf ("could not convert\n");
      return -1;
   }
   printf ("%s\n", x_rpn);
   return 0;
}
