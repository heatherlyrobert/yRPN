/*============================----beg-of-source---============================*/
#include  "yRPN.h"
#include  "yRPN_priv.h"





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
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   switch (a_mode) {
   case YRPN_CBANG : myRPN.lang = a_mode;       break;
   case YRPN_GYGES : myRPN.lang = a_mode;       break;
   default         : myRPN.lang = YRPN_CBANG;   break;
   }
   myRPN.math = '-';
   yrpn_addr_init   ();
   yrpn_output_init ();
   yrpn_stack_init  ();
   yrpn_oper_init   ();
   yrpn_group_init   ();
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return 0;
}

int          /*--> convert spreadsheet infix to rpn ------[ ------ [ ------ ]-*/
yRPN_errorpos      (void)
{
   return myRPN.pos;
}


/*====================------------------------------------====================*/
/*===----                       external access                        ----===*/
/*====================------------------------------------====================*/
static void        o___EXTERNAL________________o (void) {;}

char
yRPN_get           (uchar a_type, uchar r_rpn [LEN_RECD], uchar *r_nrpn)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(default)------------------------*/
   if (r_rpn  != NULL)  strlcpy (r_rpn, "", LEN_RECD);
   if (r_nrpn != NULL)  *r_nrpn = 0;
   /*---(defense)------------------------*/
   --rce;  if (myRPN.n_tokens <= 0 && myRPN.n_shunted <= 0) {
      return rce;
   }
   /*---(save-back)----------------------*/
   --rce;  switch (a_type) {
   case YRPN_TOKENS  :
      if (r_rpn  != NULL)  strlcpy (r_rpn, myRPN.tokens , LEN_RECD);
      if (r_nrpn != NULL)  *r_nrpn = myRPN.n_tokens;
      break;
   case YRPN_PARSED  :
      if (r_rpn  != NULL)  strlcpy (r_rpn, myRPN.parsed , LEN_RECD);
      if (r_nrpn != NULL)  *r_nrpn = myRPN.n_tokens;
      break;
   case YRPN_PRETTY  :
      if (r_rpn  != NULL)  strlcpy (r_rpn, myRPN.pretty , LEN_RECD);
      if (r_nrpn != NULL)  *r_nrpn = myRPN.n_tokens;
      break;
   case YRPN_MATHY   :
      if (r_rpn  != NULL)  strlcpy (r_rpn, myRPN.mathy  , LEN_RECD);
      if (r_nrpn != NULL)  *r_nrpn = myRPN.n_tokens;
      break;
   case YRPN_SHUNTED :
      if (r_rpn  != NULL)  strlcpy (r_rpn, myRPN.shunted, LEN_RECD);
      if (r_nrpn != NULL)  *r_nrpn = myRPN.n_shunted;
      break;
   case YRPN_DETAIL  :
      if (r_rpn  != NULL)  strlcpy (r_rpn, myRPN.detail , LEN_RECD);
      if (r_nrpn != NULL)  *r_nrpn = myRPN.n_shunted;
      break;
   case YRPN_DEBUG   :
      if (r_rpn  != NULL)  strlcpy (r_rpn, myRPN.debug  , LEN_RECD);
      if (r_nrpn != NULL)  *r_nrpn = myRPN.n_shunted;
      break;
   case YRPN_EXACT   :
      if (r_rpn  != NULL)  strlcpy (r_rpn, myRPN.exact  , LEN_RECD);
      if (r_nrpn != NULL)  *r_nrpn = myRPN.n_shunted;
      break;
   default           :
      return rce;
      break;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char
yRPN_formula       (uchar *a_src, uchar a_type, uchar r_rpn [LEN_RECD], uchar *r_nrpn)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(prepare flags)------------------*/
   DEBUG_YRPN    yLOG_note    ("set spreadsheet configuration");
   myRPN.lang    = YRPN_GYGES;
   strcpy (s_divider, " ");
   strcpy (s_divtech, " ");
   s_ctab = 0;
   /*---(convert)------------------------*/
   rc = yrpn_base__driver (a_src, YRPN_SHUNTED, r_rpn, r_nrpn, LEN_RECD);
   DEBUG_YRPN    yLOG_value   ("driver"    , rc);
   if (rc < 0) {
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /* ---- : set c internal ------------------------------------------*/
yRPN_compiler      (char *a_src, short a_tab, char **a_rpn, int a_nrpn, int a_max)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char       *x_rpn       = NULL;          /* return string of rpn notation  */
   myRPN.lang    = YRPN_CBANG;
   strcpy (s_divider, ", ");
   sprintf (s_divtech, "%c", G_KEY_FIELD);
   return 0;
}

char         /*--> convert spreadsheet infix to rpn ------[ ------ [ ------ ]-*/
yRPN_interpret     (char *a_src, char **a_rpn, int *a_nrpn, int a_max, int a_def)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(prepare flags)------------------*/
   DEBUG_YRPN    yLOG_note    ("set spreadsheet configuration");
   myRPN.lang    = YRPN_GYGES;
   strcpy (s_divider, ",");
   s_ctab = a_def;
   /*---(convert)------------------------*/
   rc = yrpn_base__driver (a_src, 's', a_rpn, a_nrpn, a_max);
   DEBUG_YRPN    yLOG_value   ("driver"    , rc);
   if (rc < 0) {
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yRPN_gyges         (char *a_src, char **r_rpn, int *r_nrpn, int a_max, int a_def)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(prepare flags)------------------*/
   DEBUG_YRPN    yLOG_note    ("set spreadsheet configuration");
   myRPN.lang    = YRPN_GYGES;
   strcpy  (s_divider, " "   );
   /*> sprintf (s_divtech, "%c", G_KEY_SHIFT);  /+ totally unused, shift in +/        <*/
   strcpy (s_divtech, "Œ");
   s_ctab = a_def;
   if (a_src [0] == '¼') {
      DEBUG_YRPN    yLOG_note    ("math mode ON");
      myRPN.math = 'y';
   }
   else {
      DEBUG_YRPN    yLOG_note    ("normal gyges function");
      myRPN.math = '-';
   }
   /*---(convert)------------------------*/
   rc = yrpn_base__driver (a_src + 1, 'd', r_rpn, r_nrpn, a_max);
   DEBUG_YRPN    yLOG_value   ("driver"    , rc);
   DEBUG_YRPN    yLOG_value   ("pos"       , myRPN.pos);
   if (rc < 0) {
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /* ---- : retrieve the tokenized format ---------------------------*/
yRPN_normal        (char *a_src, char **a_rpn, int *a_nrpn, int a_max)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(prepare flags)------------------*/
   DEBUG_YRPN    yLOG_note    ("set configuration");
   myRPN.lang    = YRPN_CBANG;
   strcpy (s_divider, ", ");
   sprintf (s_divtech, "%c", G_KEY_FIELD);
   /*---(convert)------------------------*/
   rc = yrpn_base__driver (a_src, 'n', a_rpn, a_nrpn, a_max);
   DEBUG_YRPN    yLOG_value   ("driver"    , rc);
   DEBUG_YRPN    yLOG_value   ("pos"       , myRPN.pos);
   if (rc < 0) {
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /* ---- : retrieve the tokenized format ---------------------------*/
yRPN_detail        (char *a_src, char **a_rpn, int *a_nrpn, int a_max)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(prepare flags)------------------*/
   DEBUG_YRPN    yLOG_note    ("set configuration");
   myRPN.lang    = YRPN_CBANG;
   strcpy (s_divider, ", ");
   sprintf (s_divtech, "%c", G_KEY_FIELD);
   /*---(convert)------------------------*/
   rc = yrpn_base__driver (a_src, 'd', a_rpn, a_nrpn, a_max);
   DEBUG_YRPN    yLOG_value   ("driver"    , rc);
   DEBUG_YRPN    yLOG_value   ("pos"       , myRPN.pos);
   if (rc < 0) {
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /* ---- : retrieve the tokenized format ---------------------------*/
yRPN_techtoken     (char *a_src, char **a_rpn, int *a_nrpn, int a_max)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(prepare flags)------------------*/
   DEBUG_YRPN    yLOG_note    ("set configuration");
   myRPN.lang    = YRPN_CBANG;
   sprintf (s_divider, "%c", G_KEY_FIELD);
   sprintf (s_divtech, "%c", G_KEY_FIELD);
   /*---(convert)------------------------*/
   rc = yrpn_base__driver (a_src, 't', a_rpn, a_nrpn, a_max);
   DEBUG_YRPN    yLOG_value   ("driver"    , rc);
   DEBUG_YRPN    yLOG_value   ("pos"       , myRPN.pos);
   if (rc < 0) {
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /* ---- : retrieve the tokenized format ---------------------------*/
yRPN_parsed        (char *a_src, char **a_rpn, int *a_nrpn, int a_max)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(prepare flags)------------------*/
   DEBUG_YRPN    yLOG_note    ("set configuration");
   strcpy (s_divider, " ");
   /*---(convert)------------------------*/
   rc = yrpn_base__driver (a_src, 'p', a_rpn, a_nrpn, a_max);
   DEBUG_YRPN    yLOG_value   ("driver"    , rc);
   DEBUG_YRPN    yLOG_value   ("pos"       , myRPN.pos);
   if (rc < 0) {
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /* ---- : retrieve the tokenized format ---------------------------*/
yRPN_tokens        (char *a_src, char **a_rpn, int *a_nrpn, int a_max)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(prepare flags)------------------*/
   DEBUG_YRPN    yLOG_note    ("set configuration");
   /*> myRPN.lang    = YRPN_CBANG;                                                   <*/
   strcpy (s_divider, " ");
   /*---(convert)------------------------*/
   rc = yrpn_base__driver (a_src, 't', a_rpn, a_nrpn, a_max);
   DEBUG_YRPN    yLOG_value   ("driver"    , rc);
   DEBUG_YRPN    yLOG_value   ("pos"       , myRPN.pos);
   if (rc < 0) {
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char       /* ---- : retrieve the tokenized format ---------------------------*/
yRPN_pretty        (char *a_src, char **a_rpn, int *a_nrpn, int a_max)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(prepare flags)------------------*/
   DEBUG_YRPN    yLOG_note    ("set configuration");
   /*> myRPN.lang    = YRPN_CBANG;                                                   <*/
   strcpy (s_divider, " ");
   /*---(convert)------------------------*/
   rc = yrpn_base__driver (a_src, 'P', a_rpn, a_nrpn, a_max);
   DEBUG_YRPN    yLOG_value   ("driver"    , rc);
   DEBUG_YRPN    yLOG_value   ("pos"       , myRPN.pos);
   if (rc < 0) {
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rc);
      return rc;
   }
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        main driver                           ----===*/
/*====================------------------------------------====================*/
static void        o___DRIVER__________________o (void) {;}

int          /*--> prepare variables for rpn conversion --[ ------ [ ------ ]-*/
yrpn_base__prepare      (char *a_source)   /* source infix string                          */
{
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(set the source/working)---------*/
   DEBUG_YRPN     yLOG_note    ("initialize source and working strings");
   if (a_source == NULL || a_source [0] == '\0') {
      strncpy (myRPN.source  , YRPN_TOKEN_NULL, LEN_RECD);
      strncpy (myRPN.working , ""             , LEN_RECD);
      myRPN.l_source  = myRPN.l_working = 0;
   } else {
      strncpy (myRPN.source  , a_source       , LEN_RECD);
      strncpy (myRPN.working , a_source       , LEN_RECD);
      myRPN.l_source  = myRPN.l_working = strlen (myRPN.source);
   }
   DEBUG_YRPN     yLOG_info    ("source"    , myRPN.source);
   DEBUG_YRPN     yLOG_value   ("len"       , myRPN.l_source);
   myRPN.pos        = 0;
   /*---(set the token vars)-------------*/
   DEBUG_YRPN     yLOG_note    ("token vars");
   strncpy (myRPN.t_token , YRPN_TOKEN_NULL, LEN_RECD);
   strncpy (myRPN.t_name  , YRPN_TOKEN_NULL, LEN_RECD);
   myRPN.t_type     = YRPN_ERROR   ;
   myRPN.t_len      = 0;
   myRPN.t_type     = YRPN_ERROR   ;
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
   yrpn_output_init  ();
   yrpn_stack_init   ();
   yrpn_group_init   ();
   /*---(complete)-----------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return  myRPN.l_source;
}

char         /*--> convert spreadsheet infix to rpn ------[ ------ [ ------ ]-*/
yrpn_base__driver       (char *a_src, char a_type, char **a_rpn, int *a_nrpn, int a_max)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char       *x_rpn       = NULL;          /* return string of rpn notation  */
   int         x_len       =    0;
   /*---(header)-------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YRPN    yLOG_point   ("a_src"     , a_src);
   --rce;  if (a_src == NULL) {
      DEBUG_YRPN    yLOG_note    ("no input variable, no point");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YRPN    yLOG_info    ("a_src"     , a_src);
   DEBUG_YRPN    yLOG_point   ("a_rpn"     , a_rpn);
   DEBUG_YRPN    yLOG_point   ("*a_rpn"    , *a_rpn);
   if (a_rpn != NULL) strlcpy (a_rpn, "", a_max);
   /*---(configure)----------------------*/
   /*> if (a_type == YRPN_MATHY)  myRPN.math = 'y';                                   <* 
    *> else                       myRPN.math = '-';                                   <*/
   /*---(convert)------------------------*/
   DEBUG_YRPN    yLOG_char    ("lang"      , myRPN.lang);
   if (a_type != 'P' && myRPN.lang == YRPN_GYGES) {
      rc = yrpn_base__convert (a_src);
      /*> ++myRPN.pos;                                                                <*/
      /*> ++myRPN.l_wor;                                                          <*/
   }
   else {
      rc = yrpn_base__convert (a_src);
   }
   DEBUG_YRPN    yLOG_value   ("convert"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(interpret results)--------------*/
   DEBUG_YRPN    yLOG_value   ("max"       , a_max);
   DEBUG_YRPN    yLOG_value   ("n_tokens"  , myRPN.n_tokens);
   switch (a_type) {
   case YRPN_PARSED  :
      if (a_rpn  != NULL)  strlcpy (a_rpn, myRPN.parsed  , a_max);
      if (a_nrpn != NULL)  *a_nrpn = myRPN.n_tokens;
      x_len = strlen (myRPN.tokens);
      break;
   case YRPN_TOKENS  :
      DEBUG_YRPN    yLOG_point   ("a_rpn"     , a_rpn);
      if (a_rpn  != NULL) {
         strlcpy (a_rpn, myRPN.tokens  , a_max);
         DEBUG_YRPN    yLOG_info    ("a_rpn"     , a_rpn);
      }
      DEBUG_YRPN    yLOG_point   ("a_nrpn"    , a_nrpn);
      if (a_nrpn != NULL) {
         DEBUG_YRPN    yLOG_value   ("n_tokens"  , myRPN.n_tokens);
         *a_nrpn = myRPN.n_tokens;
         DEBUG_YRPN    yLOG_value   ("*a_nrpn"   , *a_nrpn);
      }
      x_len = strlen (myRPN.tokens);
      break;
   case YRPN_PRETTY  :
      if (a_rpn  != NULL)  strlcpy (a_rpn, myRPN.pretty  , a_max);
      if (a_nrpn != NULL)  *a_nrpn = myRPN.n_tokens;
      x_len = strlen (myRPN.pretty);
      break;
   case YRPN_DETAIL  :
      if (a_rpn  != NULL)  strlcpy (a_rpn, myRPN.detail  , a_max);
      if (a_nrpn != NULL)  *a_nrpn = myRPN.n_shunted;
      x_len = strlen (myRPN.detail);
      break;
   case YRPN_DEBUG   :
      if (a_rpn  != NULL)  strlcpy (a_rpn, myRPN.debug   , a_max);
      if (a_nrpn != NULL)  *a_nrpn = myRPN.n_tokens;
      x_len = strlen (myRPN.debug);
      break;
   case YRPN_SHUNTED :
      if (a_rpn  != NULL)  strlcpy (a_rpn, myRPN.shunted , a_max);
      if (a_nrpn != NULL)  *a_nrpn = myRPN.n_shunted;
      x_len = strlen (myRPN.shunted);
      break;
   }
   DEBUG_YRPN    yLOG_value   ("l_working" , myRPN.l_working);
   DEBUG_YRPN    yLOG_value   ("x_len"     , x_len);
   DEBUG_YRPN    yLOG_value   ("pos"       , myRPN.pos);
   /*---(check for truncation)-----------*/
   DEBUG_YRPN    yLOG_value   ("length"    , x_len);
   --rce;  if (x_len >= a_max) {
      DEBUG_YRPN    yLOG_note    ("output too long for output provided");
      if (a_rpn != NULL) strlcpy (a_rpn, "", a_max);
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char      /* ---- : convert normal infix notation to postfix/rpn ------------*/
yrpn_base__convert      (char *a_source)
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
   DEBUG_YRPN   yLOG_enter   (__FUNCTION__);
   zRPN_DEBUG  printf("RPN_convert     :: beg ------------------------------\n");
   /*---(locals)-------------------------*/
   char      rce       =  -10;
   int       len       =    0;
   int       rc        =    0;
   char      x_ch      =    0;
   int       x_pass    =    0;
   char      x_name    [LEN_FULL]  = "";
   /*---(defenses)-----------------------*/
   --rce;  if (a_source     == NULL)  {
      DEBUG_YRPN   yLOG_exitr   (__FUNCTION__, rce);
      return rce;    /* no source to convert         */
   }
   yrpn_base__prepare (a_source);
   /*---(main loop)----------------------*/
   zRPN_DEBUG  printf("   ---process-------------\n");
   myRPN.pproc = S_PPROC_NO;
   DEBUG_YRPN   yLOG_note    ("convert loop enter--------------------");
   while (myRPN.pos < myRPN.l_working) {
      DEBUG_YRPN   yLOG_value   ("PASS"      , x_pass++);
      /*---(prepare)---------------------*/
      x_ch    = myRPN.working [myRPN.pos];
      DEBUG_YRPN   yLOG_char    ("x_ch"      , x_ch);
      rc      = myRPN.pos;
      /*---(pick handler)----------------*/
      if (rc <= myRPN.pos && (x_ch == '\"' || x_ch == '<'))   rc = yrpn_fixed_string  (myRPN.pos);
      if (rc <= myRPN.pos && x_ch == '\'')                    rc = yrpn_fixed_char    (myRPN.pos);
      if (rc <= myRPN.pos && strchr ("#"       , x_ch) != 0)  rc = yrpn_addr_badref   (myRPN.pos);
      if (rc <= myRPN.pos && strchr (YSTR_ADDR , x_ch) != 0)  rc = yrpn_addr          (myRPN.pos, s_ctab);
      if (rc <= myRPN.pos && strchr (YSTR_LOWER, x_ch) != 0)  rc = yrpn_cbang_keyword (myRPN.pos);
      if (rc <= myRPN.pos && strchr (YSTR_LOWER, x_ch) != 0)  rc = yrpn_cbang_type    (myRPN.pos);
      if (rc <= myRPN.pos && strchr (YSTR_NUML , x_ch) != 0)  rc = yrpn_nums_any      (myRPN.pos);
      if (rc <= myRPN.pos && strchr (YSTR_VARL , x_ch) != 0)  rc = yrpn_syms_funcvar  (myRPN.pos);
      if (rc <= myRPN.pos && strchr ("@•ç"     , x_ch) != 0)  rc = yrpn_syms_funcvar  (myRPN.pos);
      if (rc <= myRPN.pos && strchr (YSTR_SEQ  , x_ch) != 0)  rc = yrpn_syms_sequence (myRPN.pos);
      if (rc <= myRPN.pos && strchr (YSTR_OPER , x_ch) != 0)  rc = yrpn_oper_any      (myRPN.pos);
      if (rc <= myRPN.pos && strchr (v_enders  , x_ch) != 0)  rc = yrpn_syms_ender    (myRPN.pos);
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
      zRPN_DEBUG  printf("   shunted = <<%s>>\n", myRPN.shunted);
      zRPN_DEBUG  printf("   detail  = <<%s>>\n", myRPN.detail);
      zRPN_DEBUG  printf("   normal  = <<%s>>\n", myRPN.debug);
      zRPN_DEBUG  printf("   tokens  = <<%s>>\n", myRPN.tokens);
      zRPN_DEBUG  printf("   pretty  = <<%s>>\n", myRPN.pretty);
      zRPN_DEBUG  printf("   mathy   = <<%s>>\n", myRPN.mathy);
      zRPN_DEBUG  printf("   exact   = <<%s>>\n", myRPN.exact);
      /*---(next)------------------------*/
      myRPN.pos = rc;
   }
   DEBUG_YRPN   yLOG_note    ("convert loop exit---------------------");
   /*---(handle errors)------------------*/
   --rce;  if (rc < 0) {
      zRPN_DEBUG  printf ("FATAL %4d : %s\n", rc, zRPN_ERRORS [ -rc - 100]);
      strlcpy (myRPN.detail , YRPN_TOKEN_NULL, LEN_RECD);
      strlcpy (myRPN.shunted, YRPN_TOKEN_NULL, LEN_RECD);
      strlcpy (myRPN.tokens , YRPN_TOKEN_NULL, LEN_RECD);
      strlcpy (myRPN.debug  , YRPN_TOKEN_NULL, LEN_RECD);
      strlcpy (myRPN.pretty , YRPN_TOKEN_NULL, LEN_RECD);
      strlcpy (myRPN.mathy  , YRPN_TOKEN_NULL, LEN_RECD);
      strlcpy (myRPN.exact  , YRPN_TOKEN_NULL, LEN_RECD);
      myRPN.l_shunted  = 0;
      myRPN.n_shunted  = 0;
      myRPN.n_tokens  = 0;
      DEBUG_YRPN   yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(clear stack)--------------------*/
   DEBUG_YRPN   yLOG_note    ("clear stack---------------------------");
   zRPN_DEBUG  printf("   ---clear stack---------\n");
   --rce;  while (1) {
      rc = yrpn_stack_peek (NULL, NULL, x_name, NULL);
      if (rc < 0)  break;
      if (strcmp (x_name, "(") == 0) {
         zRPN_DEBUG  printf ("FATAL %4d : %s\n", zRPN_ERR_UNBALANCED_PARENS, "unbalanced parentheses\n");
         strlcpy (myRPN.detail , YRPN_TOKEN_NULL, LEN_RECD);
         strlcpy (myRPN.shunted, YRPN_TOKEN_NULL, LEN_RECD);
         myRPN.l_shunted  = 0;
         myRPN.n_shunted  = 0;
         DEBUG_YRPN   yLOG_exit    (__FUNCTION__);
         return rce;
      }
      yrpn_stack_pop  ();
   }
   /*---(run exact)----------------------*/
   DEBUG_YRPN   yLOG_note    ("run exact-----------------------------");
   rc = yrpn_exact (myRPN.exact);
   DEBUG_YRPN   yLOG_value   ("exact"     , rc);
   /*---(run exact)----------------------*/
   zRPN_DEBUG  printf("      done\n");
   /*---(output)-------------------------*/
   zRPN_DEBUG  printf("   shunted = <<%s>>\n", myRPN.shunted);
   zRPN_DEBUG  printf("   detail  = <<%s>>\n", myRPN.detail);
   zRPN_DEBUG  printf("   normal  = <<%s>>\n", myRPN.debug );
   zRPN_DEBUG  printf("   parsed  = <<%s>>\n", myRPN.parsed);
   zRPN_DEBUG  printf("   tokens  = <<%s>>\n", myRPN.tokens);
   zRPN_DEBUG  printf("   pretty  = <<%s>>\n", myRPN.pretty);
   zRPN_DEBUG  printf("   mathy   = <<%s>>\n", myRPN.mathy);
   zRPN_DEBUG  printf("   exact   = <<%s>>\n", myRPN.exact);
   DEBUG_YRPN   yLOG_info    ("shunted"   , myRPN.shunted);
   DEBUG_YRPN   yLOG_info    ("detail"    , myRPN.detail);
   DEBUG_YRPN   yLOG_info    ("normal"    , myRPN.debug );
   DEBUG_YRPN   yLOG_info    ("parsed"    , myRPN.parsed);
   DEBUG_YRPN   yLOG_info    ("tokens"    , myRPN.tokens);
   DEBUG_YRPN   yLOG_info    ("pretty"    , myRPN.pretty);
   DEBUG_YRPN   yLOG_info    ("mathy"     , myRPN.mathy);
   DEBUG_YRPN   yLOG_info    ("exact"     , myRPN.exact);
   /*---(complete)-----------------------*/
   zRPN_DEBUG  printf("RPN_convert     :: end ------------------------------\n");
   DEBUG_YRPN   yLOG_exit    (__FUNCTION__);
   return 0;
}


/*============================----end-of-source---============================*/
