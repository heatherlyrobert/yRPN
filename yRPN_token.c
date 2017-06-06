/*============================----beg-of-source---============================*/

#include  "yRPN.h"
#include  "yRPN_priv.h"



char         /*--> set token to error --------------------[--------[--------]-*/
yRPN__token_error    (void)
{
   strncpy (rpn.t_name, YRPN_TOKEN_NULL, S_LEN_TOKEN);
   rpn.t_len    = 0;
   rpn.t_type   = S_TTYPE_ERROR;
   rpn.t_prec   = S_PREC_NONE;
   return 0;
}

char         /*--> add a char to current token -----------[--------[--------]-*/
yRPN__token_add      (int *a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   char        x_ch        =    0;     /* current character                   */
   char        x_esc       =  '-';     /* is the previous char an escape      */
   char        x_bad       =  '-';
   char       *x_norm      = "\"";
   char       *x_ppre      = "\"<";
   char       *x_psuf      = "\">";
   /*---(header)------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   DEBUG_YRPN_M  yLOG_schar   (rpn.t_type);
   DEBUG_YRPN_M  yLOG_sint    (*a_pos);
   DEBUG_YRPN_M  yLOG_sint    (rpn.l_working);
   /*---(defense)-----------------------*/
   --rce;  if (*a_pos >= rpn.l_working) {
      DEBUG_YRPN_M  yLOG_snote   ("past max");
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(get character)-----------------*/
   x_ch    = rpn.working [*a_pos];
   DEBUG_YRPN_M  yLOG_schar   (x_ch);
   if (rpn.t_len > 0 && rpn.t_name [rpn.t_len - 1] == '\\')  x_esc = 'y';
   DEBUG_YRPN_M  yLOG_schar   (x_esc);
   /*---(check character)---------------*/
   --rce;  switch (rpn.t_type) {
   case S_TTYPE_KEYW   : case S_TTYPE_TYPE   :
      DEBUG_YRPN_M  yLOG_snote   ("v_lower");
      if (strchr (v_lower , x_ch) == 0)                         x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_CONST  :
      DEBUG_YRPN_M  yLOG_snote   ("v_upnum");
      if (rpn.t_len == 0 && (x_ch < 'A' || x_ch > 'Z'))         x_bad = 'y';
      if (strchr (v_upnum , x_ch) == 0)                         x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_INT    :
      DEBUG_YRPN_M  yLOG_snote   ("v_number");
      if (strchr (v_number, x_ch) == 0)                         x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_FLOAT  :
      DEBUG_YRPN_M  yLOG_snote   ("v_float");
      if (strchr (v_float , x_ch) == 0)                         x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_BIN    :
      DEBUG_YRPN_M  yLOG_snote   ("v_binary");
      if (rpn.t_len != 1 && strchr (v_binary, x_ch) == 0)       x_bad = 'y';
      if (rpn.t_len == 1 && (x_ch != 'b' && x_ch != 'B'))       x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_OCT    :
      DEBUG_YRPN_M  yLOG_snote   ("v_octal");
      if (rpn.t_len != 1 && strchr (v_octal + 1, x_ch) == 0)    x_bad = 'y';
      if (rpn.t_len == 1 && strchr (v_octal    , x_ch) == 0)    x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_HEX    :
      DEBUG_YRPN_M  yLOG_snote   ("v_hex");
      if (rpn.t_len != 1 && strchr (v_hex   , x_ch) == 0)       x_bad = 'y';
      if (rpn.t_len == 1 && (x_ch != 'x' && x_ch != 'X'))       x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_VARS   : case S_TTYPE_FUNC   :
      DEBUG_YRPN_M  yLOG_snote   ("v_alphanum");
      if (rpn.t_len == 0 && strchr (v_alpha, x_ch) == 0)        x_bad = 'y';
      if (strchr (v_alphanum, x_ch) == 0)                       x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_STR    :
      /*---(check pre-processor)---------*/
      if (rpn.pproc == S_PPROC_INCL) {
         DEBUG_YRPN_M  yLOG_snote   ("include str");
         if (rpn.t_len == 0) {
            if (strchr (x_ppre, x_ch) == 0)                     x_bad = 'y';
         } else {
            if (x_esc != 'y' && strchr (x_psuf, x_ch) != 0)     x_bad = '#';
         }
      }
      /*---(check normal)----------------*/
      else {
         DEBUG_YRPN_M  yLOG_snote   ("normal str");
         if (rpn.t_len == 0) {
            if (strchr (x_norm, x_ch) == 0)                     x_bad = 'y';
         } else {
            if (x_esc != 'y' && strchr (x_norm, x_ch) != 0)     x_bad = '#';
         }
      }
      /*---(done)------------------------*/
      break;
   case S_TTYPE_CHAR   :
      if (rpn.t_len == 0 && x_ch != '\'')                       x_bad = 'y';
      if (x_esc != 'y' && rpn.t_len == 2 && x_ch == '\'')       x_bad = '#';
      if (x_esc == 'y' && rpn.t_len == 3 && x_ch == '\'')       x_bad = '#';
      if (rpn.t_len >= 3)                                       x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_OPER   :
      if (rpn.t_len >= 2)                                       x_bad = 'y';
      if (strchr (v_operator , x_ch) == 0)                      x_bad = 'y';
      break;
      /*---(done)------------------------*/
   default             :
      DEBUG_YRPN_M  yLOG_snote   ("illegal type");
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
      /*---(done)------------------------*/
   }
   /*---(stop if bad)-------------------*/
   --rce;  if (x_bad == 'y') {
      DEBUG_YRPN_M  yLOG_snote   ("illegal char");
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(add to token name)-------------*/
   rpn.t_name [rpn.t_len]    = x_ch;
   rpn.t_name [++rpn.t_len]  = '\0';
   DEBUG_YRPN_M  yLOG_snote   (rpn.t_name);
   DEBUG_YRPN_M  yLOG_sint    (rpn.t_len);
   ++(*a_pos);
   DEBUG_YRPN_M  yLOG_sint    (*a_pos);
   /*---(stop if string ended)----------*/
   --rce;  if (x_bad == '#') {
      DEBUG_YRPN_M  yLOG_snote   ("string end");
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
   while (x_pos <  rpn.l_working) {
      x_ch = rpn.working [x_pos];
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
yRPN__token_nums     (int a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        x_ch        =    0;     /* current character                   */
   int         x_pos       =    0;     /* updated position in input           */
   char        x_type      =  S_TTYPE_INT;
   /*---(header)------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   DEBUG_YRPN_M  yLOG_sint    (a_pos);
   /*---(check for non-number)-----------*/
   x_pos = a_pos;
   while (x_pos <  rpn.l_working) {
      x_ch = rpn.working [x_pos];
      DEBUG_YRPN_M  yLOG_schar   (x_ch);
      if (strchr (v_number, x_ch) == 0) break;
      ++x_pos;
   }
   DEBUG_YRPN_M  yLOG_sint    (x_pos);
   /*---(classify)-----------------------*/
   if (rpn.working [a_pos] == '0' && x_pos == a_pos + 1) {
      DEBUG_YRPN_M  yLOG_snote   ("special");
      switch (x_ch) {
      case 'x' : case 'X' :
         DEBUG_YRPN_M  yLOG_snote   ("hex");
         x_type = S_TTYPE_HEX;
         break;
      case 'b' : case 'B' :
         DEBUG_YRPN_M  yLOG_snote   ("bin");
         x_type = S_TTYPE_BIN;
         break;
      case 'o' :
         DEBUG_YRPN_M  yLOG_snote   ("oct");
         x_type = S_TTYPE_OCT;
         break;
      case '.' :
         DEBUG_YRPN_M  yLOG_snote   ("float");
         x_type = S_TTYPE_FLOAT;
         break;
      default  :
         DEBUG_YRPN_M  yLOG_snote   ("int1");
         x_type = S_TTYPE_INT;
         break;
      }
   } else if (rpn.working [a_pos] == '0' && x_pos > a_pos + 1) {
      DEBUG_YRPN_M  yLOG_snote   ("oct2");
      x_type = S_TTYPE_OCT;
   } else if (x_ch == '.') {
      DEBUG_YRPN_M  yLOG_snote   ("float2");
      x_type = S_TTYPE_FLOAT;
   } else {
      DEBUG_YRPN_M  yLOG_snote   ("int2");
      x_type = S_TTYPE_INT;
   }
   DEBUG_YRPN_M  yLOG_schar   (x_type);
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
