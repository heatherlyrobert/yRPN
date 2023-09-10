/*============================----beg-of-source---============================*/
#include  "yRPN.h"
#include  "yRPN_priv.h"



/*====================------------------------------------====================*/
/*===----                        initial prep                          ----===*/
/*====================------------------------------------====================*/
static void        o___PREPARE_________________o (void) {;}

char         /*--> check what comes next -----------------[--------[--------]-*/
yrpn_nums__typing       (int a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        x_ch        =    0;     /* current character                   */
   int         x_pos       =    0;     /* updated position in input           */
   char        x_type      =  YRPN_INT;
   char        x_key       =    0;
   /*---(header)------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   DEBUG_YRPN_M  yLOG_sint    (a_pos);
   /*---(simple)------------------------*/
   DEBUG_YRPN_M  yLOG_schar   (myRPN.working [a_pos]);
   x_key  = myRPN.working [a_pos];
   if (myRPN.working [a_pos] == '0')  x_key  = myRPN.working [a_pos + 1];
   DEBUG_YRPN_M  yLOG_schar   (x_key);
   /*---(special/prefixed)--------------*/
   switch (x_key) {
   case  'é'   :
      DEBUG_YRPN_M  yLOG_snote   ("bin");
      x_type = myRPN.t_type = YRPN_BIN;
      DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
      return x_type;
      break;
   case  'ö'   :
      DEBUG_YRPN_M  yLOG_snote   ("oct");
      x_type = myRPN.t_type = YRPN_OCT;
      DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
      return x_type;
      break;
   case  'õ'   :
      DEBUG_YRPN_M  yLOG_snote   ("hex");
      x_type = myRPN.t_type = YRPN_HEX;
      DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
      return x_type;
      break;
   case  'í'   :
      DEBUG_YRPN_M  yLOG_snote   ("mongo");
      x_type = myRPN.t_type = YRPN_MONGO;
      DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
      return x_type;
      break;
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
   /*---(float/int)----------------------*/
   if (myRPN.working [a_pos] == '0' && x_pos == a_pos + 1) {
      DEBUG_YRPN_M  yLOG_snote   ("special");
      switch (x_ch) {
      case '.' :
         DEBUG_YRPN_M  yLOG_snote   ("float");
         x_type = YRPN_FLOAT;   
         break;
      default  :
         DEBUG_YRPN_M  yLOG_snote   ("int1");
         x_type = YRPN_INT;
         break;
      }
   } else if (x_ch == '.') {
      DEBUG_YRPN_M  yLOG_snote   ("float2");
      x_type = YRPN_FLOAT;
   } else {
      DEBUG_YRPN_M  yLOG_snote   ("int2");
      x_type = YRPN_INT;
   }
   /*---(save-back)----------------------*/
   DEBUG_YRPN_M  yLOG_schar   (x_type);
   myRPN.t_type = x_type;
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return x_type;
}



/*====================------------------------------------====================*/
/*===----                  single character validation                 ----===*/
/*====================------------------------------------====================*/
static void        o___SINGLES_________________o (void) {;}

char
yrpn_nums_one_int       (short l, char f, char p, char c)
{
   DEBUG_YRPN_M  yLOG_snote   ("INT");
   if (c == 0)                                        return 0;
   if (l == 0) {
      if (strchr (YSTR_NUMBER, c) == NULL)            return 0;
   } else {
      if      (f == '0')                              return 0;
      else if (strchr (YSTR_NUMBER, c) == NULL)       return 0;
   }
   return 1;
}

char
yrpn_nums_one_float     (short l, char f, char p, char c)
{
   DEBUG_YRPN_M  yLOG_snote   ("FLOAT");
   if (c == 0)                                        return 0;
   if (l == 0) {
      if  (strchr (YSTR_NUMBER, c) ==NULL)            return 0;
   } else if (l == 1 && f == '0' && c != '.')         return 0;
   if (l >  0 && strchr (YSTR_FLOAT, c) == NULL)      return 0;
   return 1;
}

char
yrpn_nums_one_bin       (short l, char f, char p, char c)
{
   DEBUG_YRPN_M  yLOG_snote   ("BINARY");
   if (c == 0)                                        return 0;
   if (l == 0) {
      if (c != 'é')                                   return 0;
   } else if (strchr (YSTR_BINARY, c) == NULL)        return 0;
   return 1;
}

char
yrpn_nums_one_oct       (short l, char f, char p, char c)
{
   DEBUG_YRPN_M  yLOG_snote   ("OCTAL");
   if (c == 0)                                        return 0;
   if (l == 0) {
      if (c != 'ö')                                   return 0;
   } else if (strchr (YSTR_OCTAL , c) == NULL)        return 0;
   return 1;
}

char
yrpn_nums_one_hex       (short l, char f, char p, char c)
{
   DEBUG_YRPN_M  yLOG_snote   ("HEX");
   if (c == 0)                                        return 0;
   if (l == 0) {
      if (c != 'õ')                                   return 0;
   } else if (strchr (YSTR_HEXUP , c) == NULL)        return 0;
   return 1;
}

char
yrpn_nums_one_mongo     (short l, char f, char p, char c)
{
   DEBUG_YRPN_M  yLOG_snote   ("MONGO");
   if (c == 0)                                        return 0;
   if (l == 0) {
      if (c != 'í')                                   return 0;
   } else if (strchr (YSTR_MONGO , c) == NULL)        return 0;
   return 1;
}



/*====================------------------------------------====================*/
/*===----                        overall driver                        ----===*/
/*====================------------------------------------====================*/
static void        o___DRIVER__________________o (void) {;}

short        /*--> check for normal type numbers ---------[--------[--------]-*/
yrpn_nums_any           (short a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /*  begin and end with single quotes, only one character inside.           */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   int         rc          =    0;
   int         x_pos       =    0;     /* updated position in input           */
   int         x_last      =    0;
   char        x_bad       =  '-';
   uchar       x_type      =  '-';
   uchar       x_name      [LEN_FULL]  = "";
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
   --rce;  if (strchr ("éöõí" YSTR_NUMBER, myRPN.working [a_pos]) == 0) {
      DEBUG_YRPN     yLOG_note    ("must start with a number");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN     yLOG_note    ("accumulate characters");
   x_pos          = a_pos;  /* starting point */
   myRPN.t_type   = yrpn_nums__typing (x_pos);
   DEBUG_YRPN     yLOG_char    ("myRPN.t_type", myRPN.t_type);
   while (yrpn_token_accum (&x_pos) == 0);
   DEBUG_YRPN     yLOG_info    ("myRPN.t_name", myRPN.t_name);
   /*---(check if long enough)-------------*/
   DEBUG_YRPN     yLOG_value   ("myRPN.t_len" , myRPN.t_len);
   switch (myRPN.t_type) {
   case YRPN_INT     :
      if (myRPN.t_len < 1)  x_bad = 'y';
      break;
   case YRPN_BIN     :
      if (myRPN.t_len < 2)  x_bad = 'y';
      if ((myRPN.t_len - 1) % 4 != 0)  x_bad = 'y';
      break;
   case YRPN_OCT     :
      if (myRPN.t_len < 2)  x_bad = 'y';
      if ((myRPN.t_len - 1) % 3 != 0)  x_bad = 'y';
      break;
   case YRPN_HEX     :
      if (myRPN.t_len < 2)  x_bad = 'y';
      if ((myRPN.t_len - 1) % 2 != 0)  x_bad = 'y';
      break;
   case YRPN_MONGO   :
      if (myRPN.t_len < 5)  x_bad = 'y';
      if ((myRPN.t_len - 1) % 4 != 0)  x_bad = 'y';
      break;
   default           :
      if (myRPN.t_len < 2)  x_bad = 'y';
      break;
   }
   --rce;  if (x_bad == 'y') {
      yrpn_token_error  ();
      DEBUG_YRPN     yLOG_note    ("number too short");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check float decimals)-------------*/
   --rce;  if (myRPN.t_type == YRPN_FLOAT) {
      rc = ystrldcnt (myRPN.t_name, '.', LEN_FULL);
      --rce;  if (rc != 1) {
         yrpn_token_error  ();
         DEBUG_YRPN     yLOG_note    ("too many decimals");
         DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      --rce;  if (myRPN.t_name [myRPN.t_len - 1] == '.') {
         yrpn_token_error  ();
         DEBUG_YRPN     yLOG_note    ("can not end with decimal");
         DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(mathy situations)-----------------*/
   rc = yrpn_output_peek (&x_type, NULL, x_name, NULL, NULL);
   DEBUG_YRPN     yLOG_char    ("x_type"    , x_type);
   switch (x_type) {
   case YRPN_VARS  : 
      DEBUG_YRPN     yLOG_note    ("number follows a variable, insert multiplier (´)");
      yrpn_stack_push   (YRPN_OPER, 'g', "´", a_pos);
      break;
   case YRPN_OPER  :
      if (strchr ("ÆÇÈÉÊÎÍÌ", x_name [0]) != NULL) {
         DEBUG_YRPN     yLOG_note    ("number follows ÆÇÈÉÊÎÍÌ, must insert a multiplier (´)");
         yrpn_stack_push   (YRPN_OPER, 'g', "´", a_pos);
      }
      break;
   case YRPN_GROUP :
      if (x_name [0] == '¹') {
         DEBUG_YRPN     yLOG_note    ("number follows math paren (¹), must insert a multiplier (´)");
         yrpn_stack_push   (YRPN_OPER, 'g', "´", a_pos);
      }
      break;
   }
   /*---(save)-----------------------------*/
   DEBUG_YRPN     yLOG_note    ("put har literal directly to output");
   ystrlcpy (myRPN.t_token, myRPN.t_name, LEN_FULL);
   yrpn_output_infix (myRPN.t_type, myRPN.t_prec, myRPN.t_name, myRPN.t_token, a_pos);
   yrpn_output_rpn   (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
   myRPN.left_oper  = S_OPER_CLEAR;
   myRPN.combined   = S_NO;
   /*---(complete)-----------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return x_pos;
}


