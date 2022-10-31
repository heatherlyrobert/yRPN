/*============================----beg-of-source---============================*/

#include  "yRPN.h"
#include  "yRPN_priv.h"


char unit_answer [ LEN_RECD ];


/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

char*      /* ---- : answer unit testing gray-box questions ------------------*/
yRPN_accessor      (char *a_question, int a_item)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        x_temp      [LEN_LABEL];
   /*---(input)--------------------------*/
   if          (strcmp (a_question, "source"    )     == 0) {
      snprintf (unit_answer, LEN_RECD, "source string    :%s"  , myRPN.source);
   }
   else if     (strcmp (a_question, "position"  )     == 0) {
      snprintf (unit_answer, LEN_RECD, "source position  : %3d of %3d" , myRPN.pos, myRPN.l_working);
   }
   /*---(processing)---------------------*/
   else if     (strcmp (a_question, "token"     )     == 0) {
      snprintf (unit_answer, LEN_RECD, "current token    :%s"  , myRPN.t_name);
   } else if (strcmp (a_question, "precedence")     == 0) {
      snprintf (unit_answer, LEN_RECD, "precedence       : %c"    , myRPN.t_prec);
   } else if (strcmp (a_question, "type"      )     == 0) {
      snprintf (unit_answer, LEN_RECD, "current type     : %c"    , myRPN.t_type);
   }
   /*---(output)-------------------------*/
   else if    (strcmp (a_question, "n_tokens"     )     == 0) {
      snprintf (unit_answer, LEN_RECD, "infix count      : %d"    , myRPN.n_tokens);
   } else if (strcmp (a_question, "tokens"    )     == 0) {
      snprintf (unit_answer, LEN_RECD, "infix output     :%s"  , myRPN.tokens);
   } else if (strcmp (a_question, "pretty"    )     == 0) {
      snprintf (unit_answer, LEN_RECD, "pretty output    :%s"  , myRPN.pretty);
   } else if (strcmp (a_question, "n_detail"     )     == 0) {
      snprintf (unit_answer, LEN_RECD, "postfix count    : %d"    , myRPN.n_shuntd);
   } else if (strcmp (a_question, "output"    )     == 0) {
      snprintf (unit_answer, LEN_RECD, "postfix output   :%s"  , myRPN.shuntd);
   } else if (strcmp (a_question, "detail"    )     == 0) {
      snprintf (unit_answer, LEN_RECD, "postfix detail   :%s"  , myRPN.detail);
   }
   /*---(stack)--------------------------*/
   /*> else if   (strcmp (a_question, "depth"     )     == 0) {                                          <* 
    *>    snprintf (unit_answer, LEN_RECD, "stack depth      : %d"    , s_nstack);                   <* 
    *> } else if (strcmp (a_question, "stack_list")     == 0) {                                          <* 
    *>    snprintf (unit_answer, LEN_RECD, "stack details    :");                                    <* 
    *>    for (i = 0; i < s_nstack; ++i) {                                                               <* 
    *>       sprintf (x_temp     , " %c,%c,%s", s_stack [i].type, s_stack [i].prec, s_stack [i].name);   <* 
    *>       strlcat (unit_answer, x_temp       , LEN_RECD);                                         <* 
    *>    }                                                                                              <* 
    *> } else if (strcmp (a_question, "stack_terse")    == 0) {                                          <* 
    *>    snprintf (unit_answer, LEN_RECD, "stack terse      :");                                    <* 
    *>    for (i = 0; i < s_nstack && i < 6; ++i) {                                                      <* 
    *>       sprintf (x_temp     , " %c%c", s_stack [i].type, s_stack [i].prec);                         <* 
    *>       strlcat (unit_answer, x_temp       , LEN_RECD);                                         <* 
    *>    }                                                                                              <* 
    *> }                                                                                                 <*/
   /*---(UNKNOWN)------------------------*/
   else {
      snprintf(unit_answer, LEN_RECD, "UNKNOWN          : question is not understood");
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}

char
yrpn__unit_insider     (int b, int x, int y, int z)
{
   return 0;
}

char
yrpn__unit_quick       (void)
{
   str0gyges  (NULL);
   yRPN_init (YRPN_GYGES);
   yRPN_addr_config   (str2gyges, str4gyges, str6gyges, str8gyges, yrpn__unit_insider);
}

char       /*----: set up programgents/debugging -----------------------------*/
yRPN__unit_quiet    (void)
{
   yrpn__unit_quick ();
   return 0;
}

char       /*----: set up programgents/debugging -----------------------------*/
yRPN__unit_loud     (void)
{
   char       *x_args [4]  = { "yRPN_unit","@@kitchen","@@RPN","@@YRPN" };
   yURG_logger (4, x_args);
   yURG_urgs   (4, x_args);
   yrpn__unit_quick ();
   return 0;
}

char       /*----: set up program urgents/debugging --------------------------*/
yRPN__unit_end      (void)
{
   DEBUG_YRPN   yLOGS_end     ();
   return 0;
}



/*============================----end-of-source---============================*/
