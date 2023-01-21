/*============================----beg-of-source---============================*/

#include  "yRPN.h"
#include  "yRPN_priv.h"




char         /*--> set token to error --------------------[--------[--------]-*/
yRPN__token_error    (void)
{
   strncpy (myRPN.t_name, YRPN_TOKEN_NULL, LEN_FULL);
   myRPN.t_len    = 0;
   myRPN.t_type   = S_TTYPE_ERROR;
   myRPN.t_prec   = S_PREC_NONE;
   return 0;
}

char
yrpn__token_unit_prep   (char a_type, char *a_working, int *a_pos, int a_start)
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

char         /*--> add a char to current token -----------[--------[--------]-*/
yrpn_token_add          (int *a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   char        x_ch        =    0;     /* current character                   */
   char        x_prev      =    0;     /* last character                      */
   char        x_esc       =  '-';     /* is the previous char an escape      */
   char        x_bad       =  '-';
   char       *x_norm      = "\"";
   char       *x_ppre      = "\"<";
   char       *x_psuf      = "\">";
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
   x_ch    = myRPN.working [*a_pos];
   DEBUG_YRPN_M  yLOG_schar   (x_ch);
   if (myRPN.t_len > 0 && myRPN.t_name [myRPN.t_len - 1] == '\\')  x_esc = 'y';
   DEBUG_YRPN_M  yLOG_schar   (x_esc);
   /*---(check character)---------------*/
   --rce;  switch (myRPN.t_type) {
   case S_TTYPE_KEYW   : case S_TTYPE_TYPE   :
      DEBUG_YRPN_M  yLOG_snote   ("YSTR_LOWER");
      if (strchr (YSTR_LOWER, x_ch) ==NULL)                          x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_CONST  :
      DEBUG_YRPN_M  yLOG_snote   ("YSTR_UPNUM");
      if (myRPN.t_len == 0 && strchr (YSTR_UPPER "_", x_ch) ==NULL)  x_bad = 'y';
      if (myRPN.t_len >  0 && strchr (YSTR_UPNUM "_", x_ch) ==NULL)  x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_INT    :
      DEBUG_YRPN_M  yLOG_snote   ("YSTR_NUMBER");
      if (strchr (YSTR_NUMBER, x_ch) ==NULL)                         x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_FLOAT  :
      DEBUG_YRPN_M  yLOG_snote   ("YSTR_FLOAT");
      if (myRPN.t_len == 0 && strchr (YSTR_NUMBER, x_ch) ==NULL)     x_bad = 'y';
      if (myRPN.t_len >  0 && strchr (YSTR_FLOAT , x_ch) ==NULL)     x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_BIN    :
      DEBUG_YRPN_M  yLOG_snote   ("YSTR_BINARY");
      if (myRPN.t_len == 0) {
         if (x_ch != 'é')                                            x_bad = 'y';
      } else if (strchr (YSTR_BINARY, x_ch) == NULL)                 x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_OCT    :
      DEBUG_YRPN_M  yLOG_snote   ("YSTR_OCTAL");
      if (myRPN.t_len == 0) {
         if (x_ch != 'ö')                                            x_bad = 'y';
      } else if (strchr (YSTR_OCTAL , x_ch) == NULL)                 x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_HEX    :
      DEBUG_YRPN_M  yLOG_snote   ("YSTR_HEXUP");
      if (myRPN.t_len == 0) {
         if( x_ch != 'õ')                                            x_bad = 'y';
      } else if (strchr (YSTR_HEXUP , x_ch) == NULL)                 x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_VARS   : case S_TTYPE_FUNC   :
      DEBUG_YRPN_M  yLOG_snote   ("YSTR_VAR");
      if (myRPN.t_len == 0 && strchr ("•@" YSTR_VARL, x_ch) == NULL) x_bad = 'y';
      else if (myRPN.t_len > 0) {
         x_prev = myRPN.t_name [myRPN.t_len - 1];
         if (myRPN.t_name [0] == '‘') {  /* locals */
            if (myRPN.t_len == 1 && strchr ("¯>!?" YSTR_LOWER YSTR_NUMBER, x_ch) == NULL) x_bad = 'y';
            if (myRPN.t_len >  1)                                    x_bad = 'y';
         } else { /* normal */
            if (strchr ("•" YSTR_SUBS , x_prev) != NULL)             x_bad = 'y';
            if (strchr (YSTR_ALNUM YSTR_SUBS, x_ch)   == NULL)       x_bad = 'y';
         }
      }
      break;
      /*---(done)------------------------*/
   case S_TTYPE_ADDR   :
      DEBUG_YRPN_M  yLOG_snote   ("YSTR_ADDR");
      if (strchr (YSTR_ADDR , x_ch) == NULL)                         x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_STR    :
      /*---(check pre-processor)---------*/
      if (myRPN.pproc == S_PPROC_INCL) {
         DEBUG_YRPN_M  yLOG_snote   ("include str");
         if (myRPN.t_len == 0 && strchr ("\"<", x_ch) == NULL)       x_bad = 'y';
         if (myRPN.t_len >  1) {
            x_prev = myRPN.t_name [myRPN.t_len - 1];
            if (x_prev != '\\' && strchr ("\">", x_ch) != NULL)      x_bad = 's';
            if (*a_pos == myRPN.l_working - 1) {
               if (strchr ("\">", x_ch) == NULL)                     x_bad = '#';
            }
         }
      }
      /*---(check normal)----------------*/
      else {
         DEBUG_YRPN_M  yLOG_snote   ("normal str");
         if (myRPN.t_len == 0 && x_ch != '"')                        x_bad = 'y';
         if (myRPN.t_len >  1) {
            x_prev = myRPN.t_name [myRPN.t_len - 1];
            if (x_prev != '\\' && x_ch == '"')                       x_bad = 's';
            if (*a_pos == myRPN.l_working - 1 && x_ch != '"')        x_bad = '#';
         }
      }
      /*---(done)------------------------*/
      break;
   case S_TTYPE_CHAR   :
      if (myRPN.t_len == 0 && x_ch != '\'')                          x_bad = 'y';
      if (myRPN.t_len >  1)  x_prev = myRPN.t_name [1];
      if (myRPN.t_len >= 2) {
         if (x_prev != '\\') {
            if (myRPN.t_len == 2 && x_ch != '\'')                    x_bad = '#';
            if (myRPN.t_len > 2)                                     x_bad = 'y';
         } else {
            if (myRPN.t_len == 3 && x_ch != '\'')                    x_bad = '#';
            if (myRPN.t_len > 3)                                     x_bad = 'y';
         }
      }
      break;
      /*---(done)------------------------*/
   case S_TTYPE_OPER   :
      if (myRPN.t_len >= 2)                                          x_bad = 'y';
      if (strchr (YSTR_OPER  , x_ch) == NULL)                        x_bad = 'y';
      break;
      /*---(done)------------------------*/
   default             :
      DEBUG_YRPN_M  yLOG_snote   ("illegal type");
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
      /*---(done)------------------------*/
   }
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

char         /*--> check what comes next -----------------[--------[--------]-*/
yRPN__token_paren    (int a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        x_ch        =    0;     /* current character                   */
   int         x_pos       =    0;     /* updated position in input           */
   char        x_type      =  S_TTYPE_VARS;
   /*---(skip whitespace)----------------*/
   x_pos = a_pos;
   while (x_pos <  myRPN.l_working) {
      x_ch = myRPN.working [x_pos];
      if (x_ch != ' ')  break;
      ++x_pos;
   }
   /*---(classify)-----------------------*/
   if (x_ch == '(')  x_type = S_TTYPE_FUNC;
   else              x_type = S_TTYPE_VARS;
   /*---(complete)-----------------------*/
   return x_type;
}

char         /*--> check what comes next -----------------[--------[--------]-*/
yrpn_token_numtype      (int a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        x_ch        =    0;     /* current character                   */
   int         x_pos       =    0;     /* updated position in input           */
   char        x_type      =  S_TTYPE_INT;
   char        x_key       =    0;
   /*---(header)------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   DEBUG_YRPN_M  yLOG_sint    (a_pos);
   /*---(simple)------------------------*/
   DEBUG_YRPN_M  yLOG_schar   (myRPN.working [a_pos]);
   x_key  = myRPN.working [a_pos];
   if (myRPN.working [a_pos] == '0')  x_key  = myRPN.working [a_pos + 1];
   DEBUG_YRPN_M  yLOG_schar   (x_key);
   switch (x_key) {
   case  'b'   : case  'B'   : case  'é'   :
      DEBUG_YRPN_M  yLOG_snote   ("bin");
      x_type = myRPN.t_type = S_TTYPE_BIN;
      DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
      return x_type;
      break;
   case  'o'   : case  'O'   : case  'ö'   :
      DEBUG_YRPN_M  yLOG_snote   ("oct");
      x_type = myRPN.t_type = S_TTYPE_OCT;
      DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
      return x_type;
      break;
   case  'x'   : case  'X'   : case  'õ'   :
      DEBUG_YRPN_M  yLOG_snote   ("hex");
      x_type = myRPN.t_type = S_TTYPE_HEX;
      DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
      return x_type;
   }
   /*---(check for non-number)-----------*/
   x_pos = a_pos;
   while (x_pos <  myRPN.l_working) {
      x_ch = myRPN.working [x_pos];
      DEBUG_YRPN_M  yLOG_schar   (x_ch);
      if (strchr (YSTR_NUMBER, x_ch) == 0) break;
      ++x_pos;
   }
   DEBUG_YRPN_M  yLOG_sint    (x_pos);
   /*---(classify)-----------------------*/
   if (myRPN.working [a_pos] == '0' && x_pos == a_pos + 1) {
      DEBUG_YRPN_M  yLOG_snote   ("special");
      switch (x_ch) {
         /*> case 'x' : case 'X' :                                                       <* 
          *>    DEBUG_YRPN_M  yLOG_snote   ("hex");                                      <* 
          *>    x_type = S_TTYPE_HEX;                                                    <* 
          *>    break;                                                                   <*/
         /*> case 'b' : case 'B' :                                                       <* 
          *>    DEBUG_YRPN_M  yLOG_snote   ("bin");                                      <* 
          *>    x_type = S_TTYPE_BIN;                                                    <* 
          *>    break;                                                                   <*/
         /*> case 'o' : case 'O' :                                                       <* 
          *>    DEBUG_YRPN_M  yLOG_snote   ("oct");                                      <* 
          *>    x_type = S_TTYPE_OCT;                                                    <* 
          *>    break;                                                                   <*/
      case '.' :
         DEBUG_YRPN_M  yLOG_snote   ("float");
         x_type = S_TTYPE_FLOAT;
         break;
      default  :
         DEBUG_YRPN_M  yLOG_snote   ("int1");
         x_type = S_TTYPE_INT;
         break;
      }
   } else if (myRPN.working [a_pos] == '0' && x_pos > a_pos + 1) {
      DEBUG_YRPN_M  yLOG_snote   ("oct2");
      x_type = S_TTYPE_OCT;
   } else if (x_ch == '.') {
      DEBUG_YRPN_M  yLOG_snote   ("float2");
      x_type = S_TTYPE_FLOAT;
   } else {
      DEBUG_YRPN_M  yLOG_snote   ("int2");
      x_type = S_TTYPE_INT;
   }
   /*---(save-back)----------------------*/
   DEBUG_YRPN_M  yLOG_schar   (x_type);
   myRPN.t_type = x_type;
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return x_type;
}

char         /*--> save token to output ------------------[--------[--------]-*/
yRPN__token_save     (char a_pos)
{
   /*> yRPN_stack_tokens  ();         /+ strait to tokens list                          +/   <*/
   yRPN_stack_shuntd   ();         /* strait to shunted and output lists (no stack)  */
   yRPN_stack_normal (a_pos);    /* strait to normal list                          */
   return 0;
}

char         /*--> push token on stack -------------------[--------[--------]-*/
yRPN__token_push     (char a_pos)
{
   /*> yRPN_stack_tokens  ();         /+ strait to tokens list                          +/   <*/
   yRPN_stack_push   (a_pos);    /* move token to stack                            */
   yRPN_stack_normal (a_pos);    /* strait to normal list                          */
   return 0;
}




/*============================----end-of-source---============================*/
