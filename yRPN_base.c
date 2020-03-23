/*============================----beg-of-source---============================*/
#include  "yRPN.h"
#include  "yRPN_priv.h"


char      zRPN_olddebug = 'n';

char      zRPN_lang   = YRPN_CBANG;


char    zRPN_ERRORS [100][50] = {
   "input not long enough",
   "starting quote not found",
   "ending quote not found",
   "string literal not long enough",
   "does not start with an alphabetic",
   "does not start with a number",
   "does not start with an operator",
   "zero length token",
   "operator too long",
   "does not start with a grouping character",
   "stack is prematurely empty",
   "can not handle addresses in c-mode",
   "not an address character",
   "badly formatted address",
   "number has multiple decimals",
   "unknown or undefined constant",
};

tRPN      myRPN;
short     s_ctab     = 0;
char      s_note     [20] = "";



/*====================------------------------------------====================*/
/*===----                           utility                            ----===*/
/*====================------------------------------------====================*/
static void        o___UTILITY_________________o (void) {;}

char*        /*--> return library version information ----[-leaf---[--------]-*/
yRPN_version       (void)
{
   char    t [20] = "";
#if    __TINYC__ > 0
   strncpy (t, "[tcc built  ]", 15);
#elif  __GNUC__  > 0
   strncpy (t, "[gnu gcc    ]", 15);
#else
   strncpy (t, "[unknown    ]", 15);
#endif
   snprintf (myRPN.about, 100, "%s   %s : %s", t, P_VERNUM, P_VERTXT);
   return myRPN.about;
}

char
yRPN_init          (cchar a_mode)
{
   DEBUG_RPN     yLOG_enter   (__FUNCTION__);
   switch (a_mode) {
   case YRPN_CBANG : zRPN_lang = a_mode;       break;
   case YRPN_GYGES : zRPN_lang = a_mode;       break;
   default         : zRPN_lang = YRPN_CBANG;   break;
   }
   yrpn_addr__init ();
   DEBUG_RPN     yLOG_exit    (__FUNCTION__);
   return 0;
}


/*====================------------------------------------====================*/
/*===----                            driver                            ----===*/
/*====================------------------------------------====================*/
static void        o___DRIVER__________________o (void) {;}

int          /*--> prepare variables for rpn conversion --[ ------ [ ------ ]-*/
yRPN__load         (char *a_source)   /* source infix string                          */
{
   DEBUG_RPN     yLOG_enter   (__FUNCTION__);
   /*---(set the source/working)---------*/
   DEBUG_RPN     yLOG_note    ("initialize source and working strings");
   if (a_source == NULL || a_source [0] == '\0') {
      strncpy (myRPN.source  , YRPN_TOKEN_NULL, S_LEN_OUTPUT);
      strncpy (myRPN.working , ""             , S_LEN_OUTPUT);
      myRPN.l_source  = myRPN.l_working = 0;
   } else {
      strncpy (myRPN.source  , a_source       , S_LEN_OUTPUT);
      strncpy (myRPN.working , a_source       , S_LEN_OUTPUT);
      myRPN.l_source  = myRPN.l_working = strlen (myRPN.source);
   }
   DEBUG_RPN     yLOG_info    ("source"    , myRPN.source);
   DEBUG_RPN     yLOG_value   ("len"       , myRPN.l_source);
   myRPN.pos        = 0;
   /*---(set the token vars)-------------*/
   DEBUG_RPN     yLOG_note    ("token vars");
   strncpy (myRPN.t_token , YRPN_TOKEN_NULL, S_LEN_OUTPUT);
   strncpy (myRPN.t_name  , YRPN_TOKEN_NULL, S_LEN_OUTPUT);
   myRPN.t_type     = S_TTYPE_ERROR;
   myRPN.t_len      = 0;
   myRPN.t_type     = S_TTYPE_ERROR;
   myRPN.t_prec     = S_PREC_NONE;
   myRPN.t_dir      = S_LEFT;
   myRPN.t_arity    = 0;
   myRPN.t_post     = S_NO;
   myRPN.t_comb     = S_NO;
   myRPN.left_oper  = S_OPER_LEFT;
   myRPN.combined   = S_YES;
   myRPN.pproc      = S_PPROC_NO;
   myRPN.p_type     = S_TTYPE_NONE;
   myRPN.p_prec     = S_PREC_NONE;
   /*---(set the stack vars)-------------*/
   yRPN_stack_init   ();
   /*---(complete)-----------------------*/
   DEBUG_RPN     yLOG_exit    (__FUNCTION__);
   return  myRPN.l_source;
}

char       /* ---- : set c internal ------------------------------------------*/
yRPN_complier      (char *a_src, short a_tab, char **a_rpn, int a_nrpn, int a_max)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char       *x_rpn       = NULL;          /* return string of rpn notation  */
   zRPN_lang    = YRPN_CBANG;
   strcpy (s_divider, ", ");
   strcpy (s_divtech, "\x1F");
   return 0;
}

char         /*--> convert spreadsheet infix to rpn ------[ ------ [ ------ ]-*/
yRPN_interpret     (char *a_src, char **a_rpn, int *a_nrpn, int a_max, int a_def)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_RPN    yLOG_enter   (__FUNCTION__);
   /*---(prepare flags)------------------*/
   DEBUG_RPN    yLOG_note    ("set spreadsheet configuration");
   zRPN_lang    = YRPN_GYGES;
   strcpy (s_divider, ",");
   s_ctab = a_def;
   /*---(convert)------------------------*/
   rc = yRPN__driver (a_src, 's', a_rpn, a_nrpn, a_max);
   DEBUG_RPN    yLOG_value   ("driver"    , rc);
   if (rc < 0) {
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_RPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /* ---- : retrieve the tokenized format ---------------------------*/
yRPN_normal        (char *a_src, char **a_rpn, int *a_nrpn, int a_max)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_RPN    yLOG_enter   (__FUNCTION__);
   /*---(prepare flags)------------------*/
   DEBUG_RPN    yLOG_note    ("set configuration");
   zRPN_lang    = YRPN_CBANG;
   strcpy (s_divider, ", ");
   strcpy (s_divtech, "\x1F");
   /*---(convert)------------------------*/
   rc = yRPN__driver (a_src, 'n', a_rpn, NULL, a_max);
   DEBUG_RPN    yLOG_value   ("driver"    , rc);
   DEBUG_RPN    yLOG_value   ("pos"       , myRPN.pos);
   if (rc < 0) {
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_RPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /* ---- : retrieve the tokenized format ---------------------------*/
yRPN_detail        (char *a_src, char **a_rpn, int *a_nrpn, int a_max)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_RPN    yLOG_enter   (__FUNCTION__);
   /*---(prepare flags)------------------*/
   DEBUG_RPN    yLOG_note    ("set configuration");
   zRPN_lang    = YRPN_CBANG;
   strcpy (s_divider, ", ");
   strcpy (s_divtech, "\x1F");
   /*---(convert)------------------------*/
   rc = yRPN__driver (a_src, 'd', a_rpn, NULL, a_max);
   DEBUG_RPN    yLOG_value   ("driver"    , rc);
   DEBUG_RPN    yLOG_value   ("pos"       , myRPN.pos);
   if (rc < 0) {
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_RPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /* ---- : retrieve the tokenized format ---------------------------*/
yRPN_techtoken     (char *a_src, char **a_rpn, int *a_nrpn, int a_max)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_RPN    yLOG_enter   (__FUNCTION__);
   /*---(prepare flags)------------------*/
   DEBUG_RPN    yLOG_note    ("set configuration");
   zRPN_lang    = YRPN_CBANG;
   strcpy (s_divider, "\x1F");
   /*---(convert)------------------------*/
   rc = yRPN__driver (a_src, 't', a_rpn, NULL, a_max);
   DEBUG_RPN    yLOG_value   ("driver"    , rc);
   DEBUG_RPN    yLOG_value   ("pos"       , myRPN.pos);
   if (rc < 0) {
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_RPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /* ---- : retrieve the tokenized format ---------------------------*/
yRPN_parsed        (char *a_src, char **a_rpn, int *a_nrpn, int a_max)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_RPN    yLOG_enter   (__FUNCTION__);
   /*---(prepare flags)------------------*/
   DEBUG_RPN    yLOG_note    ("set configuration");
   strcpy (s_divider, " ");
   /*---(convert)------------------------*/
   rc = yRPN__driver (a_src, 'p', a_rpn, NULL, a_max);
   DEBUG_RPN    yLOG_value   ("driver"    , rc);
   DEBUG_RPN    yLOG_value   ("pos"       , myRPN.pos);
   if (rc < 0) {
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_RPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /* ---- : retrieve the tokenized format ---------------------------*/
yRPN_tokens        (char *a_src, char **a_rpn, int *a_nrpn, int a_max)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_RPN    yLOG_enter   (__FUNCTION__);
   /*---(prepare flags)------------------*/
   DEBUG_RPN    yLOG_note    ("set configuration");
   /*> zRPN_lang    = YRPN_CBANG;                                                   <*/
   strcpy (s_divider, " ");
   /*---(convert)------------------------*/
   rc = yRPN__driver (a_src, 't', a_rpn, NULL, a_max);
   DEBUG_RPN    yLOG_value   ("driver"    , rc);
   DEBUG_RPN    yLOG_value   ("pos"       , myRPN.pos);
   if (rc < 0) {
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_RPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /* ---- : retrieve the tokenized format ---------------------------*/
yRPN_pretty        (char *a_src, char **a_rpn, int *a_nrpn, int a_max)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_RPN    yLOG_enter   (__FUNCTION__);
   /*---(prepare flags)------------------*/
   DEBUG_RPN    yLOG_note    ("set configuration");
   /*> zRPN_lang    = YRPN_CBANG;                                                   <*/
   strcpy (s_divider, " ");
   /*---(convert)------------------------*/
   rc = yRPN__driver (a_src, 'P', a_rpn, NULL, a_max);
   DEBUG_RPN    yLOG_value   ("driver"    , rc);
   DEBUG_RPN    yLOG_value   ("pos"       , myRPN.pos);
   if (rc < 0) {
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_RPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

int          /*--> convert spreadsheet infix to rpn ------[ ------ [ ------ ]-*/
yRPN_errorpos      (void)
{
   return myRPN.pos;
}

char         /*--> convert spreadsheet infix to rpn ------[ ------ [ ------ ]-*/
yRPN__driver       (char *a_src, char a_type, char **a_rpn, int *a_nrpn, int a_max)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char       *x_rpn       = NULL;          /* return string of rpn notation  */
   int         x_len       =    0;
   /*---(header)-------------------------*/
   DEBUG_RPN    yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_RPN    yLOG_point   ("a_src"     , a_src);
   --rce;  if (a_src == NULL) {
      DEBUG_RPN    yLOG_note    ("no input variable, no point");
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_RPN    yLOG_info    ("a_src"     , a_src);
   DEBUG_RPN    yLOG_point   ("a_rpn"     , a_rpn);
   DEBUG_RPN    yLOG_point   ("*a_rpn"    , *a_rpn);
   if (a_rpn != NULL) strlcpy (a_rpn, "", a_max);
   /*---(convert)------------------------*/
   if (a_type == 's') {
      rc = yRPN__convert (a_src + 1);
      ++myRPN.pos;
      ++myRPN.l_working;
   }
   else {
      rc = yRPN__convert (a_src);
   }
   DEBUG_RPN    yLOG_value   ("convert"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(interpret results)--------------*/
   DEBUG_RPN    yLOG_value   ("max"       , a_max);
   DEBUG_RPN    yLOG_value   ("n_tokens"  , myRPN.n_tokens);
   switch (a_type) {
   case 'p' :
      if (a_rpn  != NULL)  strlcpy (a_rpn, myRPN.parsed  , a_max);
      if (a_nrpn != NULL)  *a_nrpn = myRPN.n_tokens;
      x_len = strlen (myRPN.tokens);
      break;
   case 't' :
         DEBUG_RPN    yLOG_point   ("a_rpn"     , a_rpn);
      if (a_rpn  != NULL) {
         strlcpy (a_rpn, myRPN.tokens  , a_max);
         DEBUG_RPN    yLOG_info    ("a_rpn"     , a_rpn);
      }
         DEBUG_RPN    yLOG_point   ("a_nrpn"    , a_nrpn);
      if (a_nrpn != NULL) {
         DEBUG_RPN    yLOG_value   ("n_tokens"  , myRPN.n_tokens);
         *a_nrpn = myRPN.n_tokens;
         DEBUG_RPN    yLOG_value   ("*a_nrpn"   , *a_nrpn);
      }
      x_len = strlen (myRPN.tokens);
      break;
   case 'P' :
      if (a_rpn  != NULL)  strlcpy (a_rpn, myRPN.pretty  , a_max);
      if (a_nrpn != NULL)  *a_nrpn = myRPN.n_tokens;
      x_len = strlen (myRPN.pretty);
      break;
   case 'd' :
      if (a_rpn  != NULL)  strlcpy (a_rpn, myRPN.detail  , a_max);
      if (a_nrpn != NULL)  *a_nrpn = myRPN.n_shuntd;
      x_len = strlen (myRPN.detail);
      break;
   case 'n' :
      if (a_rpn  != NULL)  strlcpy (a_rpn, myRPN.normal  , a_max);
      if (a_nrpn != NULL)  *a_nrpn = myRPN.n_tokens;
      x_len = strlen (myRPN.normal);
      break;
   case 's' :
      if (a_rpn  != NULL)  strlcpy (a_rpn, myRPN.shuntd  , a_max);
      if (a_nrpn != NULL)  *a_nrpn = myRPN.n_shuntd;
      x_len = strlen (myRPN.shuntd);
      break;
   }
   DEBUG_RPN    yLOG_value   ("l_working" , myRPN.l_working);
   DEBUG_RPN    yLOG_value   ("x_len"     , x_len);
   DEBUG_RPN    yLOG_value   ("pos"       , myRPN.pos);
   /*---(check for truncation)-----------*/
   DEBUG_RPN    yLOG_value   ("length"    , x_len);
   --rce;  if (x_len >= a_max) {
      DEBUG_RPN    yLOG_note    ("output too long for output provided");
      if (a_rpn != NULL) strlcpy (a_rpn, "", a_max);
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_RPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char      /* ---- : convert normal infix notation to postfix/rpn ------------*/
yRPN__convert       (char *a_source)
{
   /*---(design notes)-------------------*/
   /*
    *   convert from normal infix to the more efficient RPN/postfix notation
    *   and at the same time, identify any critical errors
    *      - unbalanced parens
    *      - unknown operator (+-, =+, etc)
    *      - unknown variable/reference name
    *      - unknown function
    *   all presedence should match ansi-c to keep it clear and clean for me
    *
    */
   DEBUG_RPN   yLOG_enter   (__FUNCTION__);
   zRPN_DEBUG  printf("RPN_convert     :: beg ------------------------------\n");
   /*---(locals)-------------------------*/
   char      rce       =  -10;
   int       len       =    0;
   int       rc        =    0;
   char      x_ch      =    0;
   int       x_pass    =    0;
   /*---(defenses)-----------------------*/
   --rce;  if (a_source     == NULL)  {
      DEBUG_RPN   yLOG_exitr   (__FUNCTION__, rce);
      return rce;    /* no source to convert         */
   }
   yRPN__load (a_source);
   /*---(main loop)----------------------*/
   zRPN_DEBUG  printf("   ---process-------------\n");
   myRPN.pproc = S_PPROC_NO;
   DEBUG_RPN   yLOG_note    ("convert loop enter--------------------");
   while (myRPN.pos < myRPN.l_working) {
      DEBUG_RPN   yLOG_value   ("PASS"      , x_pass++);
      /*---(prepare)---------------------*/
      x_ch    = myRPN.working [myRPN.pos];
      DEBUG_RPN   yLOG_char    ("x_ch"      , x_ch);
      rc      = myRPN.pos;
      /*---(pick handler)----------------*/
      if (rc <= myRPN.pos && (x_ch == '\"' || x_ch == '<'))   rc = yRPN__strings    (myRPN.pos);
      if (rc <= myRPN.pos && x_ch == '\'')                    rc = yRPN__chars      (myRPN.pos);
      if (rc <= myRPN.pos && strchr ("#"       , x_ch) != 0)  rc = yRPN__badaddr    (myRPN.pos);
      if (rc <= myRPN.pos && strchr (v_address , x_ch) != 0)  rc = yRPN__addresses  (myRPN.pos, s_ctab);
      if (rc <= myRPN.pos && strchr (v_lower   , x_ch) != 0)  rc = yRPN__keywords   (myRPN.pos);
      if (rc <= myRPN.pos && strchr (v_lower   , x_ch) != 0)  rc = yRPN__types      (myRPN.pos);
      if (rc <= myRPN.pos && strchr (v_alpha   , x_ch) != 0)  rc = yRPN__constants  (myRPN.pos);
      if (rc <= myRPN.pos && strchr (v_alpha   , x_ch) != 0)  rc = yRPN__funcvar    (myRPN.pos);
      if (rc <= myRPN.pos && strchr (v_number  , x_ch) != 0)  rc = yRPN__numbers    (myRPN.pos);
      if (rc <= myRPN.pos && strchr (v_sequence, x_ch) != 0)  rc = yRPN__sequencer  (myRPN.pos);
      if (rc <= myRPN.pos && strchr (v_operator, x_ch) != 0)  rc = yRPN__operators  (myRPN.pos);
      if (rc <= myRPN.pos && strchr (v_enders  , x_ch) != 0)  rc = yRPN__enders     (myRPN.pos);
      /*> if (rc <= myRPN.pos &&                myRPN.pproc == S_PPROC_OTHER) rc = yRPN__text       (myRPN.pos);   <*/
      /*---(unrecognized)----------------*/
      if (rc <= myRPN.pos) {
         if (x_ch == ' ') {
            zRPN_DEBUG  printf ("   whitespace\n");
            ++myRPN.pos;
            continue;
         }
         rc = -1;
         break;
      }
      /*---(output)----------------------*/
      zRPN_DEBUG  printf("   shuntd  = <<%s>>\n", myRPN.shuntd);
      zRPN_DEBUG  printf("   detail  = <<%s>>\n", myRPN.detail);
      zRPN_DEBUG  printf("   normal  = <<%s>>\n", myRPN.normal);
      zRPN_DEBUG  printf("   tokens  = <<%s>>\n", myRPN.tokens);
      zRPN_DEBUG  printf("   pretty  = <<%s>>\n", myRPN.pretty);
      /*---(next)------------------------*/
      myRPN.pos = rc;
   }
   DEBUG_RPN   yLOG_note    ("convert loop exit---------------------");
   /*---(handle errors)------------------*/
   --rce;  if (rc < 0) {
      zRPN_DEBUG  printf ("FATAL %4d : %s\n", rc, zRPN_ERRORS [ -rc - 100]);
      strlcpy (myRPN.detail, YRPN_TOKEN_NULL, S_LEN_OUTPUT);
      strlcpy (myRPN.shuntd, YRPN_TOKEN_NULL, S_LEN_OUTPUT);
      strlcpy (myRPN.tokens, YRPN_TOKEN_NULL, S_LEN_OUTPUT);
      strlcpy (myRPN.normal, YRPN_TOKEN_NULL, S_LEN_OUTPUT);
      strlcpy (myRPN.pretty, YRPN_TOKEN_NULL, S_LEN_OUTPUT);
      myRPN.l_shuntd  = 0;
      myRPN.n_shuntd  = 0;
      myRPN.n_tokens  = 0;
      DEBUG_RPN   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(clear stack)--------------------*/
   zRPN_DEBUG  printf("   ---clear stack---------\n");
   --rce;  while (1) {
      rc = yRPN_stack_peek();
      if (rc < 0)  break;
      if (strcmp(myRPN.p_name, "(") == 0) {
         zRPN_DEBUG  printf ("FATAL %4d : %s\n", zRPN_ERR_UNBALANCED_PARENS, "unbalanced parentheses\n");
         strlcpy (myRPN.detail, YRPN_TOKEN_NULL, S_LEN_OUTPUT);
         strlcpy (myRPN.shuntd, YRPN_TOKEN_NULL, S_LEN_OUTPUT);
         myRPN.l_shuntd  = 0;
         myRPN.n_shuntd  = 0;
         DEBUG_RPN   yLOG_exit    (__FUNCTION__);
         return rce;
      }
      yRPN_stack_pops ();
   }
   zRPN_DEBUG  printf("      done\n");
   /*---(output)-------------------------*/
   zRPN_DEBUG  printf("   shunted = <<%s>>\n", myRPN.shuntd);
   zRPN_DEBUG  printf("   detail  = <<%s>>\n", myRPN.detail);
   zRPN_DEBUG  printf("   normal  = <<%s>>\n", myRPN.normal);
   zRPN_DEBUG  printf("   parsed  = <<%s>>\n", myRPN.parsed);
   zRPN_DEBUG  printf("   tokens  = <<%s>>\n", myRPN.tokens);
   zRPN_DEBUG  printf("   pretty  = <<%s>>\n", myRPN.pretty);
   DEBUG_RPN   yLOG_info    ("shunted"   , myRPN.shuntd);
   DEBUG_RPN   yLOG_info    ("detail"    , myRPN.detail);
   DEBUG_RPN   yLOG_info    ("normal"    , myRPN.normal);
   DEBUG_RPN   yLOG_info    ("parsed"    , myRPN.parsed);
   DEBUG_RPN   yLOG_info    ("tokens"    , myRPN.tokens);
   DEBUG_RPN   yLOG_info    ("pretty"    , myRPN.pretty);
   /*---(complete)-----------------------*/
   zRPN_DEBUG  printf("RPN_convert     :: end ------------------------------\n");
   DEBUG_RPN   yLOG_exit    (__FUNCTION__);
   return 0;
}


/*============================----end-of-source---============================*/
