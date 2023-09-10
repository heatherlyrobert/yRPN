/*============================----beg-of-source---============================*/
#include  "yRPN.h"
#include  "yRPN_priv.h"




/*====================------------------------------------====================*/
/*===----                  single character validation                 ----===*/
/*====================------------------------------------====================*/
static void        o___SINGLES_________________o (void) {;}

char
yrpn_fixed_one_char     (short l, char f, char p, char c)
{
   DEBUG_YRPN_M  yLOG_snote   ("CHAR");
   if (c == 0)                                        return 0;
   if (l == 0 && c != '\'')                           return 0;
   if (l == 2) {
      if (p != '\\') {
         if (c != '\'')                               return -1;
      }
   }
   if (l == 3) {
      if (myRPN.t_name [1] != '\\')                   return 0;
      else if (c != '\'')                             return -1;
   }
   if (l > 3)                                         return 0;
   return 1;
}

char
yrpn_fixed_one_str      (short l, char f, char p, char c, char e)
{
   DEBUG_YRPN_M  yLOG_snote   ("STRING");
   if (c == 0)                                        return 0;
   /*---(check pre-processor)---------*/
   if (myRPN.pproc == S_PPROC_INCL) {
      DEBUG_YRPN_M  yLOG_snote   ("include str");
      if (l == 0 && strchr ("\"<", c) == NULL)        return 0;
      if (l >  1) {
         if (p != '\\' && strchr ("\">", c) != NULL)  return -2;
         if (e == 'y') {
            if (strchr ("\">", c) == NULL)            return -1;
         }
      }
   }
   /*---(check normal)----------------*/
   else {
      DEBUG_YRPN_M  yLOG_snote   ("normal str");
      if (l == 0 && c != '"')                         return 0;
      if (l >  1) {
         if (p != '\\' && c == '"')                   return -2;
         if (e == 'y' && c != '"')                    return -1;
      }
   }
   /*---(done)------------------------*/
   return 1;
}



/*====================------------------------------------====================*/
/*===----                        overall drivers                       ----===*/
/*====================------------------------------------====================*/
static void        o___DRIVERS_________________o (void) {;}

short
yrpn_fixed_char         (short a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /*  begin and end with single quotes, only one character inside.           */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   int         x_pos       =    0;     /* updated position in input           */
   int         i           =    0;     /* iterator for keywords               */
   int         x_found     =   -1;     /* index of keyword                    */
   int         x_last      =    0;
   /*---(header)------------------------*/
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yrpn_token_error  ();
   DEBUG_YRPN     yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos <  0) {
      DEBUG_YRPN     yLOG_note    ("start can not be negative");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN     yLOG_note    ("accumulate characters");
   myRPN.t_type   = YRPN_CHAR;
   x_pos        = a_pos;  /* starting point */
   while (yrpn_token_accum (&x_pos) == 0);
   DEBUG_YRPN     yLOG_info    ("myRPN.t_name", myRPN.t_name);
   /*---(check if long enough)-------------*/
   DEBUG_YRPN     yLOG_value   ("myRPN.t_len" , myRPN.t_len);
   --rce;  if (myRPN.t_len <=  0) {
      yrpn_token_error  ();
      DEBUG_YRPN     yLOG_note    ("char was illegal");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)-----------------------------*/
   DEBUG_YRPN     yLOG_note    ("put char literal directly to output");
   ystrlcpy (myRPN.t_token, myRPN.t_name, LEN_FULL);
   yrpn_output_infix (myRPN.t_type, myRPN.t_prec, myRPN.t_name, myRPN.t_token, a_pos);
   yrpn_output_rpn   (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
   myRPN.left_oper  = S_OPER_CLEAR;
   myRPN.combined   = S_NO;
   /*---(complete)-----------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return x_pos;
}

short
yrpn_fixed_string       (short  a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /*  begin and end with double quotes, can escape quotes inside             */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   int         x_pos       =    0;     /* updated position in input           */
   int         i           =    0;     /* iterator for keywords               */
   int         x_found     =   -1;     /* index of keyword                    */
   int         x_last      =    0;
   /*---(header)------------------------*/
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yrpn_token_error  ();
   DEBUG_YRPN     yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos <  0) {
      DEBUG_YRPN     yLOG_note    ("start can not be negative");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN     yLOG_note    ("accumulate characters");
   myRPN.t_type   = YRPN_STR;
   x_pos        = a_pos;  /* starting point */
   while (yrpn_token_accum (&x_pos) == 0);
   DEBUG_YRPN     yLOG_info    ("myRPN.t_name", myRPN.t_name);
   /*---(check if long enough)-------------*/
   DEBUG_YRPN     yLOG_value   ("myRPN.t_len" , myRPN.t_len);
   --rce;  if (myRPN.t_len <  2) {
      yrpn_token_error  ();
      DEBUG_YRPN     yLOG_note    ("string too short");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check matching quotes)------------*/
   x_last = myRPN.t_len - 1;
   --rce;  if (myRPN.t_name [0] == '\"' && myRPN.t_name [x_last] != '\"') {
      yrpn_token_error  ();
      DEBUG_YRPN     yLOG_note    ("string does not end properly");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (myRPN.t_name [0] == '<' && myRPN.t_name [x_last] != '>') {
      yrpn_token_error  ();
      DEBUG_YRPN     yLOG_note    ("include does not end properly");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)-----------------------------*/
   DEBUG_YRPN     yLOG_note    ("put string literal directly to output");
   ystrlcpy (myRPN.t_token, myRPN.t_name, LEN_FULL);
   yrpn_output_infix (myRPN.t_type, myRPN.t_prec, myRPN.t_name, myRPN.t_token, a_pos);
   yrpn_output_rpn   (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
   myRPN.left_oper  = S_OPER_CLEAR;
   myRPN.combined   = S_NO;
   /*---(complete)-----------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return x_pos;
}

