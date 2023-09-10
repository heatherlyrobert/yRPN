/*============================----beg-of-source---============================*/
#include  "yRPN.h"
#include  "yRPN_priv.h"


/*> char        s_tokens     [LEN_RECD];                                              <*/

char        s_mode      = '-';
char        s_rpn        [LEN_RECD] = "";

char
PROG_urgents            (int a_argc, char *a_argv [])
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_PROG  yLOG_enter   (__FUNCTION__);
   /*---(set mute)-----------------------*/
   yURG_all_mute ();
   /*---(start logger)-------------------*/
   rc = yURG_logger  (a_argc, a_argv);
   DEBUG_PROG   yLOG_value    ("logger"    , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   /*---(process urgents)----------------*/
   rc = yURG_urgs    (a_argc, a_argv);
   DEBUG_PROG   yLOG_value    ("logger"    , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_PROG  yLOG_exit  (__FUNCTION__);
   return rc;
}

char
PROG__init         (int a_argc, char *a_argv[])
{
   return 0;
}

char
PROG__args              (int a_argc, char *a_argv[])
{
   /*---(locals)-------------------------*/
   char        rce         =  -10;
   char        rc          =    0;
   int         i           =    0;
   char       *a           = NULL;
   char       *b           = NULL;
   int         x_total     =    0;
   int         x_args      =    0;
   char        x_name      [LEN_FULL]   = "";
   char        t           [LEN_FULL]   = "";
   /*---(header)-------------------------*/
   DEBUG_PROG  yLOG_enter   (__FUNCTION__);
   /*---(process)------------------------*/
   for (i = 1; i < a_argc; ++i) {
      /*---(set up args)-----------------*/
      DEBUG_ARGS  yLOG_value   ("check----" , i);
      a = a_argv [i];
      if (i + 1 < a_argc)  b = a_argv [i + 1];
      else                 b = NULL;
      DEBUG_ARGS  yLOG_info    ("a"         , a);
      DEBUG_ARGS  yLOG_info    ("b"         , b);
      ++x_total;
      /*---(filter)----------------------*/
      if (a[0] == '@') {
         DEBUG_ARGS  yLOG_note    ("skipped urgent");
         continue;
      }
      /*---(local)-----------------------*/
      DEBUG_ARGS  yLOG_note    ("check for local argument handling");
      ++x_args;
      /*---(prefixes)--------------------*/
      if      (strcmp (a, "--default") == 0) s_mode = 's';
      else if (strcmp (a, "--gyges"  ) == 0) s_mode = 'g';
      else if (strcmp (a, "--pretty" ) == 0) s_mode = 'p';
      else if (strcmp (a, "--all"    ) == 0) s_mode = 'a';
      else if (strcmp (a, "--symbols") == 0) s_mode = 'S';
      /*---(other)-----------------------*/
      else if (a [0] != '-')   ystrlcpy (s_rpn , a, LEN_RECD);
   }
   DEBUG_ARGS  yLOG_char   ("s_mode"    , s_mode);
   DEBUG_ARGS  yLOG_info   ("s_rpn"     , s_rpn);
   DEBUG_ARGS  yLOG_value  ("entries"   , x_total);
   DEBUG_ARGS  yLOG_value  ("arguments" , x_args);
   /*---(complete)-----------------------*/
   DEBUG_PROG  yLOG_exit  (__FUNCTION__);
   return 0;
}

char
PROG__begin        (void)
{
   return 0;
}

char
PROG_startup            (int a_argc, char *a_argv [])
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   yURG_stage_check (YURG_BEG);
   DEBUG_PROG  yLOG_enter   (__FUNCTION__);
   /*---(initialize)---------------------*/
   rc = PROG__init   (a_argc, a_argv);
   DEBUG_PROG   yLOG_value    ("init"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   /*---(arguments)----------------------*/
   rc = PROG__args   (a_argc, a_argv);
   DEBUG_PROG   yLOG_value    ("args"      , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   /*---(begin)--------------------------*/
   rc = PROG__begin  ();
   DEBUG_PROG   yLOG_value    ("begin"     , rc);
   --rce;  if (rc < 0) {
      DEBUG_PROG   yLOG_exitr    (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_PROG  yLOG_exit  (__FUNCTION__);
   yURG_stage_check (YURG_MID);
   return rc;
}

char
main               (int a_argc, char **a_argv)
{
   char        rce         =  -10;
   char        rc          =    0;
   char        x_infix     [LEN_RECD];
   char        x_gyges     [LEN_RECD];
   char        x_rpn       [LEN_RECD];
   int         x_nrpn      = NULL;
   rc = PROG_urgents (a_argc, a_argv);
   --rce;  if (rc < 0) {
      printf ("FATAL, failed during urgents\n");
      return rce;
   }
   rc = PROG_startup (a_argc, a_argv);
   --rce;  if (rc < 0) {
      printf ("FATAL, failed during startup\n");
      return rce;
   }
   switch (s_mode) {
   case 'S' :
      rc = yRPN_symbols ();
      break;
   case 'a' :
      yRPN_formula   (s_rpn, YRPN_SHUNTED, &x_rpn, &x_nrpn);
      printf ("rpn, reverse polish notation converter\n");
      printf ("\n");
      printf ("orig     ·· å%sæ\n", s_rpn);
      printf ("\n");
      printf ("/*---(infix)---------------*/\n");
      yRPN_get       (YRPN_TOKENS , &x_rpn, &x_nrpn);
      printf ("tokens   %2d å%sæ\n", x_nrpn, x_rpn);
      yRPN_get       (YRPN_PARSED , &x_rpn, &x_nrpn);
      printf ("parsed   %2d å%sæ\n", x_nrpn, x_rpn);
      yRPN_get       (YRPN_PRETTY , &x_rpn, &x_nrpn);
      printf ("pretty   %2d å%sæ\n", x_nrpn, x_rpn);
      yRPN_get       (YRPN_MATHY  , &x_rpn, &x_nrpn);
      printf ("mathy    %2d å%sæ\n", x_nrpn, x_rpn);
      yRPN_get       (YRPN_EXACT  , &x_rpn, &x_nrpn);
      printf ("exact    %2d å%sæ\n", x_nrpn, x_rpn);
      printf ("\n");
      printf ("/*---(rpn)-----------------*/\n");
      yRPN_get       (YRPN_SHUNTED, &x_rpn, &x_nrpn);
      printf ("shunted  %2d å%sæ\n", x_nrpn, x_rpn);
      yRPN_get       (YRPN_DETAIL , &x_rpn, &x_nrpn);
      ystrldchg (x_rpn, '', '·', LEN_RECD);
      printf ("detail   %2d å%sæ\n", x_nrpn, x_rpn);
      yRPN_get       (YRPN_DEBUG  , &x_rpn, &x_nrpn);
      ystrldchg (x_rpn, '', '·', LEN_RECD);
      printf ("debug    %2d å%sæ\n", x_nrpn, x_rpn);
      printf ("\n");
      break;
   }
   return 0;
}
