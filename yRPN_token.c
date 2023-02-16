/*============================----beg-of-source---============================*/

#include  "yRPN.h"
#include  "yRPN_priv.h"



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___SUPPORT_________________o (void) {;};

char         /*--> set token to error --------------------[--------[--------]-*/
yrpn_token_error     (void)
{
   strncpy (myRPN.t_name, YRPN_TOKEN_NULL, LEN_FULL);
   myRPN.t_len    = 0;
   myRPN.t_type   = YRPN_ERROR   ;
   myRPN.t_prec   = S_PREC_NONE;
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        parsing tokens                        ----===*/
/*====================------------------------------------====================*/
static void      o___PARSE___________________o (void) {;};

char         /*--> add a char to current token -----------[--------[--------]-*/
yrpn_token_accum        (int *a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   char        rc          =    0;
   char        x_ch        =    0;     /* current character                   */
   char        x_prev      =    0;     /* last character                      */
   char        x_esc       =  '-';     /* is the previous char an escape      */
   char        x_bad       =  '-';
   char       *x_norm      = "\"";
   char       *x_ppre      = "\"<";
   char       *x_psuf      = "\">";
   int         l           =    0;
   char        f           =    0;
   char        p           =    0;
   char        c           =    0;
   char        e           =    0;
   /*---(header)------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   DEBUG_YRPN_M  yLOG_schar   (myRPN.t_type);
   DEBUG_YRPN_M  yLOG_sint    (*a_pos);
   DEBUG_YRPN_M  yLOG_sint    (myRPN.l_working);
   /*---(defense)-----------------------*/
   --rce;  if (*a_pos >= myRPN.l_working) {
      DEBUG_YRPN_M  yLOG_snote   ("past max");
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(get character)-----------------*/
   c = x_ch    = myRPN.working [*a_pos];
   DEBUG_YRPN_M  yLOG_schar   (x_ch);
   l = myRPN.t_len;
   if (l > 0)   f = myRPN.t_name [0];
   if (l > 0)   p = myRPN.t_name [l - 1];
   if (*a_pos == myRPN.l_working - 1)  e = 'y';
   if (myRPN.t_len > 0 && myRPN.t_name [myRPN.t_len - 1] == '\\')  x_esc = 'y';
   DEBUG_YRPN_M  yLOG_schar   (x_esc);
   /*---(check character)---------------*/
   --rce;  switch (myRPN.t_type) {
   case YRPN_KEYW      : rc = yrpn_cbang_one       (l, f, p, c);    break;
   case YRPN_TYPE      : rc = yrpn_cbang_one       (l, f, p, c);    break;
   case YRPN_INT       : rc = yrpn_nums_one_int    (l, f, p, c);    break;
   case YRPN_FLOAT     : rc = yrpn_nums_one_float  (l, f, p, c);    break;
   case YRPN_BIN       : rc = yrpn_nums_one_bin    (l, f, p, c);    break;
   case YRPN_OCT       : rc = yrpn_nums_one_oct    (l, f, p, c);    break;
   case YRPN_HEX       : rc = yrpn_nums_one_hex    (l, f, p, c);    break;
   case YRPN_MONGO     : rc = yrpn_nums_one_mongo  (l, f, p, c);    break;
   case YRPN_VARS      : rc = yrpn_syms_one        (l, f, p, c);    break;
   case YRPN_FUNC      : rc = yrpn_syms_one        (l, f, p, c);    break;
   case YRPN_ADDR      : rc = yrpn_addr_one        (l, f, p, c);    break;
   case YRPN_CHAR      : rc = yrpn_fixed_one_char  (l, f, p, c);    break;
   case YRPN_STR       : rc = yrpn_fixed_one_str   (l, f, p, c, e); break;
   case YRPN_OPER      : rc = yrpn_oper_one        (l, f, p, c, '-'); break;
   default             :
      DEBUG_YRPN_M  yLOG_snote   ("illegal type");
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
      /*---(done)------------------------*/
   }
   /*---(set the bad code)--------------*/
   if (rc ==  0)  x_bad = 'y';  /* normal failure                      */
   if (rc == -1)  x_bad = '#';  /* + restores position pointer         */
   if (rc == -2)  x_bad = 's';  /* special helper for string endings   */
   /*---(bad char/string)---------------*/
   --rce;  if (x_bad == '#') {
      DEBUG_YRPN_M  yLOG_snote   ("misformed char/string");
      *a_pos      -= myRPN.t_len;
      myRPN.t_len  = 0;
      strlcpy (myRPN.t_name, "", LEN_FULL);
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(stop if bad)-------------------*/
   DEBUG_YRPN_M  yLOG_schar   (x_bad);
   --rce;  if (x_bad == 'y') {
      DEBUG_YRPN_M  yLOG_snote   ("illegal char");
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(add to token name)-------------*/
   myRPN.t_name [myRPN.t_len]    = x_ch;
   myRPN.t_name [++myRPN.t_len]  = '\0';
   DEBUG_YRPN_M  yLOG_snote   (myRPN.t_name);
   DEBUG_YRPN_M  yLOG_sint    (myRPN.t_len);
   ++(*a_pos);
   DEBUG_YRPN_M  yLOG_sint    (*a_pos);
   /*---(end of string)-----------------*/
   --rce;  if (x_bad == 's') {
      DEBUG_YRPN_M  yLOG_snote   ("end of string");
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare for next char)-------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITEST_________________o (void) {;};

char
yrpn_token__unit_prep   (char a_type, char *a_working, int *a_pos, int a_start)
{
   myRPN.t_len    = 0;
   strlcpy (myRPN.t_name, "", LEN_FULL);
   myRPN.t_type   = a_type;
   if (a_working == NULL) strlcpy (myRPN.working, ""       , LEN_RECD);
   else                   strlcpy (myRPN.working, a_working, LEN_RECD);
   myRPN.l_working = strlen (myRPN.working);
   if (a_pos != NULL)  *a_pos = a_start;
   return 0;
}




/*============================----end-of-source---============================*/
