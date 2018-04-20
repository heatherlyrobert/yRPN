/*============================----beg-of-source---============================*/

#include  "yRPN.h"
#include  "yRPN_priv.h"


char unit_answer [ S_LEN_OUTPUT ];


/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

char*      /* ---- : answer unit testing gray-box questions ------------------*/
yRPN_accessor      (char *a_question, int a_item)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        x_temp      [S_LEN_LABEL];
   /*---(input)--------------------------*/
   if        (strcmp (a_question, "source"    )     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "source string    :%s"  , myRPN.source);
   }
   /*---(processing)---------------------*/
   else if     (strcmp (a_question, "token"     )     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "current token    :%s"  , myRPN.t_name);
   } else if (strcmp (a_question, "precedence")     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "precedence       : %c"    , myRPN.t_prec);
   } else if (strcmp (a_question, "type"      )     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "current type     : %c"    , myRPN.t_type);
   }
   /*---(output)-------------------------*/
   else if    (strcmp (a_question, "n_tokens"     )     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "infix count      : %d"    , myRPN.n_tokens);
   } else if (strcmp (a_question, "tokens"    )     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "infix output     :%s"  , myRPN.tokens);
   } else if (strcmp (a_question, "n_detail"     )     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "postfix count    : %d"    , myRPN.n_shuntd);
   } else if (strcmp (a_question, "output"    )     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "postfix output   :%s"  , myRPN.shuntd);
   } else if (strcmp (a_question, "detail"    )     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "postfix detail   :%s"  , myRPN.detail);
   }
   /*---(stack)--------------------------*/
   /*> else if   (strcmp (a_question, "depth"     )     == 0) {                                          <* 
    *>    snprintf (unit_answer, S_LEN_OUTPUT, "stack depth      : %d"    , s_nstack);                   <* 
    *> } else if (strcmp (a_question, "stack_list")     == 0) {                                          <* 
    *>    snprintf (unit_answer, S_LEN_OUTPUT, "stack details    :");                                    <* 
    *>    for (i = 0; i < s_nstack; ++i) {                                                               <* 
    *>       sprintf (x_temp     , " %c,%c,%s", s_stack [i].type, s_stack [i].prec, s_stack [i].name);   <* 
    *>       strlcat (unit_answer, x_temp       , S_LEN_OUTPUT);                                         <* 
    *>    }                                                                                              <* 
    *> } else if (strcmp (a_question, "stack_terse")    == 0) {                                          <* 
    *>    snprintf (unit_answer, S_LEN_OUTPUT, "stack terse      :");                                    <* 
    *>    for (i = 0; i < s_nstack && i < 6; ++i) {                                                      <* 
    *>       sprintf (x_temp     , " %c%c", s_stack [i].type, s_stack [i].prec);                         <* 
    *>       strlcat (unit_answer, x_temp       , S_LEN_OUTPUT);                                         <* 
    *>    }                                                                                              <* 
    *> }                                                                                                 <*/
   /*---(spreadsheet)--------------------*/
   /*> else if   (strcmp (a_question, "address"   )     == 0) {                                                                        <* 
    *>    snprintf (unit_answer, S_LEN_OUTPUT, "yRPN address     : tab=%4d, col=%4d, row=%4d, abs=%1d", v_tab, v_col, v_row, v_abs);   <* 
    *> }                                                                                                                               <*/
   /*---(UNKNOWN)------------------------*/
   else {
      snprintf(unit_answer, S_LEN_OUTPUT, "UNKNOWN          : question is not understood");
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}

char       /*----: set up programgents/debugging -----------------------------*/
yRPN__testquiet     (void)
{
   char       *x_args [2]  = { "yRPN_debug","@@quiet" };
   yURG_logger (2, x_args);
   yURG_urgs   (2, x_args);
   return 0;
}

char       /*----: set up programgents/debugging -----------------------------*/
yRPN__testloud      (void)
{
   char       *x_args [3]  = { "yRPN_debug","@@kitchen","@@YRPN" };
   yURG_logger (3, x_args);
   yURG_urgs   (3, x_args);
   return 0;
}

char       /*----: set up program urgents/debugging --------------------------*/
yRPN__testend       (void)
{
   DEBUG_TOPS   yLOG_end     ();
   return 0;
}



/*============================----end-of-source---============================*/
