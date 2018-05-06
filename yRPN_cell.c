/*============================----beg-of-source---============================*/
#include  "yRPN.h"
#include  "yRPN_priv.h"



/*====================------------------------------------====================*/
/*===----                     spreadsheet specific                     ----===*/
/*====================------------------------------------====================*/
static void        o___GYGES___________________o (void) {;}

int          /*--> chec for cell addresses ---------------[--------[--------]-*/
yRPN__addresses      (int  a_pos, short a_ctab)
{  /*---(design notes)--------------------------------------------------------*/
   /* addresses are only lowercase alphanumerics plus $ and @.                */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   char        rc          =    0;     /* generic return code                 */
   int         x_pos       =    0;     /* updated position in input           */
   char        x_addr      [S_LEN_LABEL];
   char        x_final     [S_LEN_LABEL];
   /*---(header)------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yRPN__token_error ();
   --rce;  if (zRPN_lang == S_LANG_CBANG) {
      DEBUG_YRPN    yLOG_note    ("skip in c lang mode");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YRPN    yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos <  0) {
      DEBUG_YRPN    yLOG_note    ("start can not be negative");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN    yLOG_note    ("accumulate characters");
   myRPN.t_type   = S_TTYPE_ADDR;
   x_pos        = a_pos;  /* starting point */
   while (yRPN__token_add (&x_pos) == 0);
   /*---(validate the address)-------------*/
   /*> printf ("%s\n", myRPN.t_name);                                                   <*/
   strlcpy (x_addr, myRPN.t_name, S_LEN_LABEL);
   /*> rc = yRPN_cell (x_addr, a_ctab);                                               <*/
   rc = str6gyges (x_addr, a_ctab, x_final);
   /*---(handle misses)------------------*/
   --rce;  if (rc < 0) {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("address not valid");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(handle)-------------------------*/
   strlcpy (myRPN.t_token, x_final, S_LEN_LABEL);
   strlcpy (myRPN.t_name , x_final, S_LEN_LABEL);
   myRPN.t_len = strlen (myRPN.t_name);
   yRPN_stack_tokens ();
   yRPN_stack_shuntd ();
   yRPN_stack_normal (a_pos);
   myRPN.left_oper  = S_OPER_CLEAR;
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return x_pos;
}



/*============================----end-of-source---============================*/
