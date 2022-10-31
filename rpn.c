/*============================----beg-of-source---============================*/
#include  "yRPN.h"
#include  "yRPN_priv.h"


/*> char        s_rpn        [LEN_RECD];                                              <*/
/*> char        s_tokens     [LEN_RECD];                                              <*/

char
main               (int a_argc, char **a_argv)
{
   char        rc          =    0;
   char        x_infix     [LEN_RECD];
   char        x_gyges     [LEN_RECD];
   char        x_rpn       [LEN_RECD];
   int         x_nrpn      = NULL;
   if (a_argc < 1) {
      printf ("must pass a infix formula string\n");
      return -1;
   }
   if (a_argc == 2) {
      if (strcmp (a_argv [1], "symbols") == 0) {
         yRPN_symbols ();
      } else {
         sprintf (x_infix, "=%s", a_argv [1]);
         rc = yRPN_interpret (x_infix, &x_rpn, &x_nrpn, LEN_RECD, 0);
         printf ("%s\n", x_rpn);
      }
   }
   if (a_argc == 3) {
      sprintf (x_gyges, "=%s", a_argv [2]);
      sprintf (x_infix, "%s", a_argv [2]);
      if      (strcmp (a_argv [1], "normal") == 0) rc = yRPN_interpret (x_infix, &x_rpn, &x_nrpn, LEN_RECD, 0);
      else if (strcmp (a_argv [1], "pretty") == 0) rc = yRPN_pretty    (x_infix, &x_rpn, &x_nrpn, LEN_RECD);
      else if (strcmp (a_argv [1], "parsed") == 0) rc = yRPN_parsed    (x_infix, &x_rpn, &x_nrpn, LEN_RECD);
      else if (strcmp (a_argv [1], "detail") == 0) {
         rc = yRPN_detail    (x_infix, &x_rpn, &x_nrpn, LEN_RECD);
         strldchg (x_rpn, '', '·', LEN_RECD);
      }
      else if (strcmp (a_argv [1], "all"   ) == 0) {
         printf ("\n");
         printf ("orig    %s\n\n", x_infix);
         yRPN_parsed    (x_infix, &x_rpn, &x_nrpn, LEN_RECD);
         printf ("tokens  %d\n\n", x_nrpn);
         printf ("parsed  %s\n\n", x_rpn);
         yRPN_pretty    (x_infix, &x_rpn, &x_nrpn, LEN_RECD);
         printf ("pretty  %s\n\n", x_rpn);
         yRPN_interpret (x_gyges, &x_rpn, &x_nrpn, LEN_RECD, 0);
         printf ("normal  %s\n\n", x_rpn);
         yRPN_detail    (x_infix, &x_rpn, &x_nrpn, LEN_RECD);
         strldchg (x_rpn, '', '·', LEN_RECD);
         printf ("detail  %s\n\n", x_rpn);
         return 0;
      }
      printf ("%s\n", x_rpn + 2);
   }
   if (rc < 0) {
      printf ("could not convert\n");
      return -1;
   }
   return 0;
}
