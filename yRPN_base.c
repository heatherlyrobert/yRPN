/*============================----beg-of-source---============================*/

#include  "yRPN.h"
#include  "yRPN_priv.h"


char      zRPN_olddebug = 'n';

char      zRPN_lang   = S_LANG_C;


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
   snprintf (myRPN.about, 100, "%s   %s : %s", t, zRPN_VER_NUM, zRPN_VER_TXT);
   return myRPN.about;
}

char         /*--> process urgents/debugging -------------[--------[--------]-*/
yRPN_debug         (char *a_urgent)
{
   /*---(defense)------------------------*/
   if (a_urgent == NULL)  return -1;
   /*---(prepare)------------------------*/
   zRPN_debug.tops    = 'n';
   zRPN_debug.stack   = 'n';
   zRPN_debug.cell    = 'n';
   zRPN_debug.oper    = 'n';
   zRPN_debug.keys    = 'n';
   /*---(parse)--------------------------*/
   if (strcmp ("@@yRPN"       , a_urgent) == 0)  zRPN_debug.tops  = 'y';
   if (strcmp ("@@yRPN_oper"  , a_urgent) == 0)  zRPN_debug.tops  = zRPN_debug.oper  = 'y';
   DEBUG_YRPN  yLOG_char    ("zRPN_tops" , zRPN_debug.tops);
   DEBUG_YRPN  yLOG_char    ("zRPN_oper" , zRPN_debug.oper);
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                            driver                            ----===*/
/*====================------------------------------------====================*/
static void        o___DRIVER__________________o (void) {;}

int          /*--> prepare variables for rpn conversion --[ ------ [ ------ ]-*/
yRPN__load         (char *a_source)   /* source infix string                          */
{
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(set the source/working)---------*/
   DEBUG_YRPN    yLOG_note    ("initialize source and working strings");
   if (a_source == NULL || a_source [0] == '\0') {
      strncpy (myRPN.source  , YRPN_TOKEN_NULL, S_LEN_OUTPUT);
      strncpy (myRPN.working , ""             , S_LEN_OUTPUT);
      myRPN.l_source  = myRPN.l_working = 0;
   } else {
      strncpy (myRPN.source  , a_source       , S_LEN_OUTPUT);
      strncpy (myRPN.working , a_source       , S_LEN_OUTPUT);
      myRPN.l_source  = myRPN.l_working = strlen (myRPN.source);
   }
   DEBUG_YRPN    yLOG_sint    (myRPN.l_source);
   /*---(set the token vars)-------------*/
   DEBUG_YRPN    yLOG_note    ("token vars");
   strncpy (myRPN.t_name  , YRPN_TOKEN_NULL, S_LEN_OUTPUT);
   myRPN.t_type     = S_TTYPE_ERROR;
   myRPN.t_len      = 0;
   myRPN.t_type     = S_TTYPE_ERROR;
   myRPN.t_prec     = S_PREC_NONE;
   myRPN.t_dir      = S_LEFT;
   myRPN.t_arity    = 0;
   myRPN.left_oper  = S_OPER_LEFT;
   myRPN.pproc      = S_PPROC_NO;
   myRPN.p_type     = S_TTYPE_NONE;
   myRPN.p_prec     = S_PREC_NONE;
   /*---(set the stack vars)-------------*/
   yRPN_stack_init   ();
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return  myRPN.l_source;
}

char       /* ---- : set c human readable ------------------------------------*/
yRPN__chuman       (void)
{
   zRPN_lang    = S_LANG_C;
   strcpy (s_divider, " ");
   return 0;
}

char       /* ---- : set spreadsheet human readable --------------------------*/
yRPN__shuman       (int *a_ntoken)
{
   zRPN_lang    = S_LANG_GYGES;
   strcpy (s_divider, " ");
   return 0;
}

char       /* ---- : set c internal ------------------------------------------*/
yRPN_compiler      (void)
{
   zRPN_lang    = S_LANG_C;
   strcpy (s_divider, ", ");
   strcpy (s_divtech, "\x1F");
   return 0;
}

char*        /*--> convert spreadsheet infix to rpn ------[ ------ [ ------ ]-*/
yRPN_spreadsheet   (
      /*----------+-----------+-----------------------------------------------*/
      char       *a_source,   /* source infix string                          */
      int        *a_ntoken,   /* number of rpn tokens in result (return)      */
      short       a_tab)
{
   /*---(locals)-----------+-----------+-*/
   char       *x_rpn       = NULL;          /* return string of rpn notation  */
   /*---(prepare flags)------------------*/
   zRPN_lang    = S_LANG_GYGES;
   strcpy (s_divider, ",");
   /*---(convert)------------------------*/
   s_ctab = a_tab;
   x_rpn = yRPN_convert (a_source);
   zRPN_DEBUG  printf("   ready   = <<%s>>\n", x_rpn);
   /*---(interprete results)-------------*/
   if (x_rpn == NULL)  {
      if (a_ntoken != NULL) *a_ntoken = 0;
      return NULL;
   }
   *a_ntoken = myRPN.n_shuntd - 1;
   /*---(complete)-----------------------*/
   return x_rpn;
}

char*      /* ---- : retrieve the tokenized format ---------------------------*/
yRPN_stokens       (char *a_source)
{
   char     *x_rpn = NULL;
   zRPN_lang    = S_LANG_GYGES;
   strcpy (s_divider, " ");
   x_rpn = yRPN_convert (a_source);
   if (x_rpn == NULL)   return NULL;
   return myRPN.tokens;
}

char*      /* ---- : retrieve the normal format ------------------------------*/
yRPN_normal        (char *a_source, char *a_normal, int *a_ntoken)
{
   char     *x_rpn = NULL;
   zRPN_lang    = S_LANG_C;
   strcpy (s_divider, ", ");
   strcpy (s_divtech, "\x1F");
   x_rpn = yRPN_convert (a_source);
   if (x_rpn == NULL)  {
      if (a_normal != NULL) strcpy (a_normal, "");
      if (a_ntoken != NULL) *a_ntoken = 0;
      return NULL;
   }
   if (a_normal != NULL) strcpy (a_normal, myRPN.normal);
   if (a_ntoken != NULL) *a_ntoken = myRPN.l_normal;
   return myRPN.shuntd;
}

char*      /* ---- : retrieve the detailed format ----------------------------*/
yRPN_detail        (char *a_source, char *a_detail, int *a_ntoken)
{
   char     *x_rpn = NULL;
   zRPN_lang    = S_LANG_C;
   strcpy (s_divider, ", ");
   strcpy (s_divtech, "\x1F");
   x_rpn = yRPN_convert (a_source);
   if (x_rpn == NULL)  {
      if (a_detail != NULL) strcpy (a_detail, "");
      if (a_ntoken != NULL) *a_ntoken = 0;
      return NULL;
   }
   if (a_detail != NULL) strcpy (a_detail, myRPN.detail);
   if (a_ntoken != NULL) *a_ntoken = myRPN.n_shuntd;
   return myRPN.shuntd;
}

char*      /* ---- : retrieve the tokenized format ---------------------------*/
yRPN_techtoken     (char *a_source)
{
   char     *x_rpn = NULL;
   zRPN_lang    = S_LANG_C;
   strcpy (s_divider, "\x1F");
   x_rpn = yRPN_convert (a_source);
   if (x_rpn == NULL)   return NULL;
   return myRPN.tokens;
}

char*      /* ---- : retrieve the tokenized format ---------------------------*/
yRPN_tokens        (char *a_source)
{
   char     *x_rpn = NULL;
   zRPN_lang    = S_LANG_C;
   strcpy (s_divider, " ");
   x_rpn = yRPN_convert (a_source);
   if (x_rpn == NULL)   return NULL;
   return myRPN.tokens;
}

char*      /* ---- : convert normal infix notation to postfix/rpn ------------*/
yRPN_convert       (char *a_source)
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
   DEBUG_YRPN  yLOG_enter   (__FUNCTION__);
   zRPN_DEBUG  printf("RPN_convert     :: beg ------------------------------\n");
   /*---(locals)-------------------------*/
   int       x_pos     = 1;
   int       len       = 0;
   int       rc        = 0;
   char      x_ch      = 0;
   int       x_pass    = 0;
   /*---(defenses)-----------------------*/
   if (a_source     == NULL)  {
      DEBUG_YRPN  yLOG_exit    (__FUNCTION__);
      return 0;    /* no source to convert         */
   }
   yRPN__load (a_source);
   /*---(main loop)----------------------*/
   zRPN_DEBUG  printf("   ---process-------------\n");
   x_pos = 0;
   myRPN.pproc = S_PPROC_NO;
   DEBUG_YRPN  yLOG_note    ("convert loop enter--------------------");
   while (x_pos < myRPN.l_working) {
      DEBUG_YRPN  yLOG_value   ("PASS"      , x_pass++);
      /*---(prepare)---------------------*/
      x_ch    = myRPN.working [x_pos];
      rc      = x_pos;
      /*---(pick handler)----------------*/
      if (rc <= x_pos && (x_ch == '\"' || x_ch == '<'))   rc = yRPN__strings    (x_pos);
      if (rc <= x_pos && x_ch == '\'')                    rc = yRPN__chars      (x_pos);
      if (rc <= x_pos && strchr (v_address , x_ch) != 0)  rc = yRPN__addresses  (x_pos, s_ctab);
      if (rc <= x_pos && strchr (v_lower   , x_ch) != 0)  rc = yRPN__keywords   (x_pos);
      if (rc <= x_pos && strchr (v_lower   , x_ch) != 0)  rc = yRPN__types      (x_pos);
      if (rc <= x_pos && strchr (v_alpha   , x_ch) != 0)  rc = yRPN__constants  (x_pos);
      if (rc <= x_pos && strchr (v_alpha   , x_ch) != 0)  rc = yRPN__funcvar    (x_pos);
      if (rc <= x_pos && strchr (v_number  , x_ch) != 0)  rc = yRPN__numbers    (x_pos);
      if (rc <= x_pos && strchr (v_sequence, x_ch) != 0)  rc = yRPN__sequencer  (x_pos);
      if (rc <= x_pos && strchr (v_operator, x_ch) != 0)  rc = yRPN__operators  (x_pos);
      if (rc <= x_pos && strchr (v_enders  , x_ch) != 0)  rc = yRPN__enders     (x_pos);
      /*> if (rc <= x_pos &&                myRPN.pproc == S_PPROC_OTHER) rc = yRPN__text       (x_pos);   <*/
      /*---(unrecognized)----------------*/
      if (rc <= x_pos) {
         if (x_ch == ' ')  zRPN_DEBUG  printf ("   whitespace\n");
         ++x_pos;
         continue;
      }
      /*---(output)----------------------*/
      zRPN_DEBUG  printf("   shuntd  = <<%s>>\n", myRPN.shuntd);
      zRPN_DEBUG  printf("   detail  = <<%s>>\n", myRPN.detail);
      zRPN_DEBUG  printf("   normal  = <<%s>>\n", myRPN.normal);
      zRPN_DEBUG  printf("   tokens  = <<%s>>\n", myRPN.tokens);
      /*---(error handling)--------------*/
      if (rc < 0)  break;
      /*---(next)------------------------*/
      x_pos = rc;
   }
   DEBUG_YRPN  yLOG_note    ("convert loop exit---------------------");
   /*---(handle errors)------------------*/
   if (rc < 0) {
      zRPN_DEBUG  printf ("FATAL %4d : %s\n", rc, zRPN_ERRORS [ -rc - 100]);
      strlcpy (myRPN.detail, YRPN_TOKEN_NULL, S_LEN_OUTPUT);
      strlcpy (myRPN.shuntd, YRPN_TOKEN_NULL, S_LEN_OUTPUT);
      myRPN.l_shuntd  = 0;
      myRPN.n_shuntd  = 0;
      DEBUG_YRPN  yLOG_exit    (__FUNCTION__);
      return NULL;
   }
   /*---(clear stack)--------------------*/
   zRPN_DEBUG  printf("   ---clear stack---------\n");
   while (1) {
      rc = yRPN_stack_peek();
      if (rc < 0)  break;
      if (strcmp(myRPN.p_name, "(") == 0) {
         zRPN_DEBUG  printf ("FATAL %4d : %s\n", zRPN_ERR_UNBALANCED_PARENS, "unbalanced parentheses\n");
         strlcpy (myRPN.detail, YRPN_TOKEN_NULL, S_LEN_OUTPUT);
         strlcpy (myRPN.shuntd, YRPN_TOKEN_NULL, S_LEN_OUTPUT);
         myRPN.l_shuntd  = 0;
         myRPN.n_shuntd  = 0;
         DEBUG_YRPN  yLOG_exit    (__FUNCTION__);
         return NULL;
      }
      yRPN_stack_pops ();
   }
   zRPN_DEBUG  printf("      done\n");
   /*---(output)-------------------------*/
   zRPN_DEBUG  printf("   shunted = <<%s>>\n", myRPN.shuntd);
   zRPN_DEBUG  printf("   detail  = <<%s>>\n", myRPN.detail);
   zRPN_DEBUG  printf("   normal  = <<%s>>\n", myRPN.normal);
   zRPN_DEBUG  printf("   tokens  = <<%s>>\n", myRPN.tokens);
   DEBUG_YRPN  yLOG_info    ("shunted"   , myRPN.shuntd);
   DEBUG_YRPN  yLOG_info    ("detail"    , myRPN.detail);
   DEBUG_YRPN  yLOG_info    ("normal"    , myRPN.normal);
   DEBUG_YRPN  yLOG_info    ("tokens"    , myRPN.tokens);
   /*---(complete)-----------------------*/
   zRPN_DEBUG  printf("RPN_convert     :: end ------------------------------\n");
   DEBUG_YRPN  yLOG_exit    (__FUNCTION__);
   /*> return  strndup (myRPN.shuntd, S_LEN_OUTPUT);                                    <*/
   return  myRPN.shuntd;
}


/*============================----end-of-source---============================*/
