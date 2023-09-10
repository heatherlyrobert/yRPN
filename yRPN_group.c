/*============================----beg-of-source---============================*/
#include  "yRPN.h"
#include  "yRPN_priv.h"



char
yrpn_group_init         (void)
{
   int         i           =    0;
   myRPN.level = 0;
   for (i = 0; i < LEN_LABEL; ++i)  myRPN.narg [i] = 0;
   return 0;
}

char
yrpn_group_open         (uchar a_type, uchar a_prec, uchar a_name [LEN_FULL], uchar a_token [LEN_FULL], short a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   uchar       x_type      =  '-';
   uchar       x_name      [LEN_FULL]  = "";
   /*---(header)------------------------*/
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(prepare)-----------------------*/
   ++myRPN.level;
   myRPN.narg [myRPN.level] = 0;
   /*> yrpn_output_infix (a_type, a_prec, a_name, a_token, a_pos);                    <*/
   rc = yrpn_stack_peek (&x_type, NULL, x_name, NULL);
   /*---(normal grouping)---------------*/
   if (rc >= 0) {
      DEBUG_YRPN     yLOG_note    ("something in stack");
      DEBUG_YRPN     yLOG_info    ("x_name"    , x_name);
      DEBUG_YRPN     yLOG_char    ("x_type"    , x_type);
      if (x_type == YRPN_FUNC || strcmp (x_name, "sizeof") == 0) {
         DEBUG_YRPN     yLOG_note    ("function openning");
         a_type = YRPN_FUNC   ;
      } else {
         DEBUG_YRPN     yLOG_note    ("normal parenthesis");
         yrpn_oper_mathy (a_pos);
      }
      yrpn_output_infix (YRPN_GROUP, a_prec, a_name, a_token, a_pos);
      yrpn_stack_push   (a_type, a_prec, a_name, a_pos);
      myRPN.left_oper  = S_OPER_LEFT;
   }
   /*---(empty stack)--------------------*/
   else {
      DEBUG_YRPN     yLOG_note    ("empty stack");
      yrpn_output_infix (YRPN_GROUP, a_prec, a_name, a_token, a_pos);
      if (myRPN.line_type == S_LINE_NORMAL) {
         DEBUG_YRPN     yLOG_note    ("normal parenthesis");
         yrpn_stack_push   (a_type, a_prec, a_name, a_pos);
      } else {
         DEBUG_YRPN     yLOG_note    ("function pointer paren (ignore)");
         myRPN.line_type  = S_LINE_DEF_FPTR;
      }
      myRPN.left_oper  = S_OPER_LEFT;
   }
   /*---(complete)-------------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yrpn_group_open_mathy   (uchar a_type, uchar a_prec, uchar a_name [LEN_FULL], uchar a_token [LEN_FULL], short a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =    0;
   uchar       x_type      =  '-';
   uchar       x_name      [LEN_FULL]  = "";
   /*---(header)------------------------*/
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(prepare)-----------------------*/
   ++myRPN.level;
   /*> rc = yrpn_output_peek (&x_type, NULL, x_name, NULL, NULL);                     <*/
   /*---(special)-----------------------*/
   yrpn_oper_mathy (a_pos);
   /*---(outputs)-----------------------*/
   yrpn_output_infix (a_type, a_prec, a_name, a_token, a_pos);
   yrpn_stack_push   (a_type, a_prec, a_name, a_pos);
   myRPN.left_oper  = S_OPER_LEFT;
   /*---(complete)-------------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yrpn_group_close        (uchar a_type, uchar a_prec, uchar a_name [LEN_FULL], uchar a_token [LEN_FULL], short a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   uchar       x_type      =  '-';
   uchar       x_name      [LEN_FULL]  = "";
   char        c           =    0;
   uchar       x_last      [LEN_FULL]  = "";
   int         l           =    0;
   /*---(header)------------------------*/
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(prepare)-----------------------*/
   DEBUG_YRPN     yLOG_value   ("level"     , myRPN.level);
   c = myRPN.narg [myRPN.level];
   DEBUG_YRPN     yLOG_value   ("commas"    , c);
   /*---(prepare)-----------------------*/
   ystrlcpy (x_name, a_name, LEN_FULL);
   /*---(cbang)-------------------------*/
   DEBUG_YRPN     yLOG_char    ("line_type" , myRPN.line_type);
   if (myRPN.line_type == S_LINE_DEF_FPTR || myRPN.line_type == S_LINE_DEF_PRO) {
      if (x_type == YRPN_PTYPE) {
         ystrlcpy (x_name, "?", LEN_LABEL);
         a_type = YRPN_VARS   ;
         a_prec = S_PREC_NONE;
         yrpn_output_infix (a_type, a_prec, x_name, a_token, a_pos);
         yrpn_output_rpn   (a_type, a_prec, x_name, a_pos);
         ystrlcpy (x_name, ")", LEN_LABEL);
         a_type = S_TTYPE_GROUP;
         yrpn_oper_prec ();
      }
   }
   /*---(normal processing)-------------*/
   yrpn_output_peek  (&x_type, NULL, x_last, NULL, NULL);
   DEBUG_YRPN     yLOG_char    ("x_type"    , x_type);
   DEBUG_YRPN     yLOG_info    ("x_last"    , x_last);
   l = strlen (x_last);
   DEBUG_YRPN     yLOG_value   ("l"         , l);
   if (x_type == YRPN_GROUP && l == 1 && strchr ("(¸", x_last [0]) != NULL) {
      DEBUG_YRPN     yLOG_note    ("empty parents (), meaning no args");
   } else {
      DEBUG_YRPN     yLOG_note    ("at least one argument");
      c = ++(myRPN.narg [myRPN.level]);
   }
   DEBUG_YRPN     yLOG_value   ("narg"      , c);
   --myRPN.level;
   DEBUG_YRPN     yLOG_value   ("level"     , myRPN.level);
   yrpn_output_infix (a_type, a_prec, x_name, a_token, a_pos);
   /*---(cbang)-------------------------*/
   if (myRPN.s_type == YRPN_FPTR    &&  myRPN.line_type == S_LINE_DEF_FPTR) {
      DEBUG_YRPN     yLOG_note    ("function pointer paren (ignore)");
   }
   /*---(normal processing)-------------*/
   else {
      DEBUG_YRPN     yLOG_note    ("normal closing parens");
      yrpn_stack_paren      (a_type, a_prec, x_name, a_pos);
      rc = yrpn_stack_peek (&x_type, NULL, NULL, NULL);
      DEBUG_YRPN     yLOG_char    ("before"    , x_type);
      if (x_type == YRPN_FUNC) {
         DEBUG_YRPN     yLOG_note    ("paren was for a function, updating type");
         DEBUG_YRPN     yLOG_value   ("narg"      , c);
         yrpn_stack_update (c + '0', 0, NULL);
      }
   }
   myRPN.left_oper  = S_OPER_CLEAR;
   /*---(complete)-------------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yrpn_group_close_mathy  (uchar a_type, uchar a_prec, uchar a_name [LEN_FULL], uchar a_token [LEN_FULL], short a_pos)
{
   /*---(header)------------------------*/
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(handle)------------------------*/
   --myRPN.level;
   yrpn_output_infix (a_type, a_prec, a_name, a_token, a_pos);
   yrpn_stack_paren  (a_type, a_prec, a_name, a_pos);
   myRPN.left_oper  = S_OPER_CLEAR;
   /*---(complete)-------------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return 0;
}

int          /*--> check for grouping symbols ------------[--------[--------]-*/
yrpn_syms_sequence      (int a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /* grouping symbols are all one-char and specific.                         */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_pos       =    0;
   int         i           =    0;
   int         x_found     =   -1;
   int         x_type      =  '-';
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
   --rce;  if (strchr (YSTR_SEQ, myRPN.working [a_pos]) == 0) {
      DEBUG_YRPN     yLOG_note    ("not a valid grouping symbol");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (myRPN.lang != 'c' && strchr ("[]", myRPN.working [a_pos]) != 0) {
      DEBUG_YRPN     yLOG_note    ("braces not allowed in spreadsheet mode");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN     yLOG_note    ("accumulate characters");
   myRPN.t_name [0] = myRPN.working [a_pos];
   myRPN.t_name [1] = '\0';
   myRPN.t_len      = 1;
   myRPN.t_type     = S_TTYPE_GROUP;
   ystrlcpy (myRPN.t_token, myRPN.t_name, LEN_FULL);
   x_pos          = a_pos + 1;
   DEBUG_YRPN     yLOG_info    ("myRPN.t_name", myRPN.t_name);
   DEBUG_YRPN     yLOG_value   ("myRPN.t_len" , myRPN.t_len);
   yrpn_oper_prec ();
   /*---(handle)---------------------------*/
   DEBUG_YRPN     yLOG_char    ("line_type" , myRPN.line_type);
   switch (myRPN.t_name [0]) {
   case '(' :
      yrpn_group_open        (myRPN.t_type, myRPN.t_prec, myRPN.t_name, myRPN.t_token, a_pos);
      break;
   case '¸' :
      yrpn_group_open_mathy  (myRPN.t_type, myRPN.t_prec, myRPN.t_name, myRPN.t_token, a_pos);
      break;
   case ')' :
      yrpn_group_close       (myRPN.t_type, myRPN.t_prec, myRPN.t_name, myRPN.t_token, a_pos);
      break;
   case '¹' :
      yrpn_group_close_mathy (myRPN.t_type, myRPN.t_prec, myRPN.t_name, myRPN.t_token, a_pos);
      break;
   case '[' :
      DEBUG_YRPN     yLOG_note    ("openning brace");
      yrpn_output_infix (myRPN.t_type, myRPN.t_prec, myRPN.t_name, myRPN.t_token, a_pos);
      yrpn_stack_push   (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
      myRPN.left_oper  = S_OPER_LEFT;
      break;
   case ']' :
      DEBUG_YRPN     yLOG_note    ("closing brace");
      yrpn_output_infix (myRPN.t_type, myRPN.t_prec, myRPN.t_name, myRPN.t_token, a_pos);
      yrpn_stack_paren  (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
      myRPN.t_type     = YRPN_OPER   ;
      yrpn_output_rpn (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
      break;
   case ',' :
      DEBUG_YRPN     yLOG_note    ("comma");
      ++(myRPN.narg [myRPN.level]);
      x_type  = myRPN.s_type;
      myRPN.line_sect = '-';
      DEBUG_YRPN     yLOG_char    ("x_type"    , x_type);
      DEBUG_YRPN     yLOG_char    ("line_sect" , myRPN.line_sect);
      if (myRPN.line_type == S_LINE_DEF_FPTR || myRPN.line_type == S_LINE_DEF_PRO) {
         if (x_type == YRPN_PTYPE) {
            ystrlcpy (myRPN.t_name, "?", LEN_LABEL);
            myRPN.t_type = YRPN_VARS   ;
            myRPN.t_prec = S_PREC_NONE;
            yrpn_output_infix (myRPN.t_type, myRPN.t_prec, myRPN.t_name, myRPN.t_token, a_pos);
            yrpn_output_rpn   (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
            ystrlcpy (myRPN.t_name, ",", LEN_LABEL);
            myRPN.t_type = S_TTYPE_GROUP;
            yrpn_oper_prec ();
         }
      }
      yrpn_output_infix (myRPN.t_type, myRPN.t_prec, myRPN.t_name, myRPN.t_token, a_pos);
      rc = yrpn_stack_comma (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
      myRPN.left_oper  = S_OPER_LEFT;
      break;
   }
   /*---(complete)-------------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return x_pos;
}

int          /*--> check for statement enders ------------[--------[--------]-*/
yrpn_syms_ender         (int  a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /* ending symbols are very limited and specific.                           */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        rc          =    0;
   int         x_pos       =    0;
   int         i           =    0;
   int         x_found     =   -1;
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
   --rce;  if (strchr (v_enders  , myRPN.working [a_pos]) == 0) {
      DEBUG_YRPN     yLOG_note    ("not a valid grouping symbol");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN     yLOG_note    ("accumulate characters");
   myRPN.t_name [0] = myRPN.working [a_pos];
   myRPN.t_name [1] = '\0';
   myRPN.t_len      = 1;
   myRPN.t_type     = S_TTYPE_GROUP;
   ystrlcpy (myRPN.t_token, myRPN.t_name, LEN_FULL);
   x_pos          = a_pos + 1;
   DEBUG_YRPN     yLOG_info    ("myRPN.t_name", myRPN.t_name);
   DEBUG_YRPN     yLOG_value   ("myRPN.t_len" , myRPN.t_len);
   yrpn_oper_prec ();
   /*---(handle)---------------------------*/
   switch (myRPN.t_name [0]) {
   case ';' :
      yrpn_output_infix (myRPN.t_type, myRPN.t_prec, myRPN.t_name, myRPN.t_token, a_pos);
      myRPN.left_oper  = S_OPER_LEFT;
      break;
   case '{' :
      yrpn_output_infix (myRPN.t_type, myRPN.t_prec, myRPN.t_name, myRPN.t_token, a_pos);
      myRPN.left_oper  = S_OPER_CLEAR;
      break;
   }
   /*---(complete)-------------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return x_pos;
}
