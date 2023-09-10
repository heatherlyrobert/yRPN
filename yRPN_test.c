/*============================----beg-of-source---============================*/

#include  "yRPN.h"
#include  "yRPN_priv.h"


char unit_answer [ LEN_RECD ];


/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

char*      /* ---- : answer unit testing gray-box questions ------------------*/
yrpn__unit              (char *a_question, int a_item)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        s           [LEN_LABEL] = "";
   char        t           [LEN_RECD]  = "";
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
      snprintf (unit_answer, LEN_RECD, "precedence       : %c" , myRPN.t_prec);
   } else if (strcmp (a_question, "type"      )     == 0) {
      snprintf (unit_answer, LEN_RECD, "current type     : %c" , myRPN.t_type);
   }
   /*---(token detail)-------------------*/
   else if    (strcmp (a_question, "n_tokens"     )     == 0) {
      snprintf (unit_answer, LEN_RECD, "infix count      : %d" , myRPN.n_tokens);
   } else if (strcmp (a_question, "tokens"    )     == 0) {
      ystrlcpy  (t, myRPN.tokens, LEN_RECD);
      ystrldchg (t, 0x0F, '·', LEN_RECD);
      snprintf (unit_answer, LEN_RECD, "infix output     :%s"  , t);
   } else if (strcmp (a_question, "pretty"    )     == 0) {
      snprintf (unit_answer, LEN_RECD, "pretty output    :%s"  , myRPN.pretty);
   } else if (strcmp (a_question, "n_detail"     )     == 0) {
      snprintf (unit_answer, LEN_RECD, "postfix count    : %d" , myRPN.n_shunted);
   } else if (strcmp (a_question, "output"    )     == 0) {
      snprintf (unit_answer, LEN_RECD, "postfix output   :%s"  , myRPN.shunted);
   } else if (strcmp (a_question, "detail"    )     == 0) {
      ystrlcpy  (t, myRPN.detail, LEN_RECD);
      ystrldchg (t, G_KEY_SHIFT, '´', LEN_RECD);
      snprintf (unit_answer, LEN_RECD, "postfix detail   :%s"  , t);
   }
   /*---(infix order)--------------------*/
   else if     (strcmp (a_question, "o_tokens"       )     == 0) {
      snprintf (unit_answer, LEN_RECD, "yRPN tokens (%2d) : %3då%sæ" , myRPN.n_tokens , strlen (myRPN.tokens ), myRPN.tokens );
   } else if  (strcmp (a_question, "o_parsed"       )     == 0) {
      snprintf (unit_answer, LEN_RECD, "yRPN parsed (%2d) : %3då%sæ" , myRPN.n_tokens , strlen (myRPN.parsed ), myRPN.parsed );
   } else if  (strcmp (a_question, "o_pretty"       )     == 0) {
      snprintf (unit_answer, LEN_RECD, "yRPN pretty (%2d) : %3då%sæ" , myRPN.n_tokens , strlen (myRPN.pretty ), myRPN.pretty );
   } else if  (strcmp (a_question, "o_mathy"        )     == 0) {
      snprintf (unit_answer, LEN_RECD, "yRPN mathy  (%2d) : %3då%sæ" , myRPN.n_tokens , strlen (myRPN.mathy  ), myRPN.mathy  );
   } else if  (strcmp (a_question, "o_exact"        )     == 0) {
      snprintf (unit_answer, LEN_RECD, "yRPN exact  (%2d) : %3då%sæ" , myRPN.n_shunted, strlen (myRPN.exact  ), myRPN.exact  );
   }
   /*---(infix order)--------------------*/
   else if   (strcmp (a_question, "o_shunted"    )     == 0) {
      snprintf (unit_answer, LEN_RECD, "yRPN shunted(%2d) : %3då%sæ" , myRPN.n_shunted, strlen (myRPN.shunted), myRPN.shunted);
   } else if (strcmp (a_question, "o_detail"     )     == 0) {
      snprintf (unit_answer, LEN_RECD, "yRPN detail (%2d) : %3då%sæ" , myRPN.n_shunted, strlen (myRPN.detail ), myRPN.detail );
   } else if (strcmp (a_question, "o_debug"      )     == 0) {
      snprintf (unit_answer, LEN_RECD, "yRPN debug  (%2d) : %3då%sæ" , myRPN.n_shunted, strlen (myRPN.debug  ), myRPN.debug  );
   }
   /*---(stack)--------------------------*/
   else if   (strcmp (a_question, "stack"         )  == 0) {
      snprintf (unit_answer, LEN_RECD, "yRPN stack  (%2d) : %2d ", g_nstack, myRPN.level);
      for (i = 0; i < g_nstack; ++i) {
         sprintf (s , "å%.5sæ", g_stack [i].name);
         sprintf (t , " %c%c%s%d", g_stack [i].type, g_stack [i].prec, s, g_stack [i].pos);
         ystrlcat (unit_answer, t, LEN_RECD);
      }
      ystrlcat (unit_answer, " ´", LEN_RECD);
   }
   /*> else if   (strcmp (a_question, "depth"     )     == 0) {                                          <* 
    *>    snprintf (unit_answer, LEN_RECD, "stack depth      : %d"    , g_nstack);                   <* 
    *> } else if (strcmp (a_question, "stack_list")     == 0) {                                          <* 
    *>    snprintf (unit_answer, LEN_RECD, "stack details    :");                                    <* 
    *>    for (i = 0; i < g_nstack; ++i) {                                                               <* 
    *>       sprintf (x_temp     , " %c,%c,%s", g_stack [i].type, g_stack [i].prec, g_stack [i].name);   <* 
    *>       ystrlcat (unit_answer, x_temp       , LEN_RECD);                                         <* 
    *>    }                                                                                              <* 
    *> } else if (strcmp (a_question, "stack_terse")    == 0) {                                          <* 
    *>    snprintf (unit_answer, LEN_RECD, "stack terse      :");                                    <* 
    *>    for (i = 0; i < g_nstack && i < 6; ++i) {                                                      <* 
    *>       sprintf (x_temp     , " %c%c", g_stack [i].type, g_stack [i].prec);                         <* 
    *>       ystrlcat (unit_answer, x_temp       , LEN_RECD);                                         <* 
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
   ystr0gyges  (NULL);
   yRPN_init (YRPN_GYGES);
   yRPN_addr_config   (ystr2gyges, ystr4gyges, ystr6gyges, ystr8gyges, yrpn__unit_insider);
}

char       /*----: set up programgents/debugging -----------------------------*/
yrpn__unit_loud     (void)
{
   char       *x_args [4]  = { "yRPN_unit","@@kitchen","@@RPN","@@YRPN" };
   yURG_logger (4, x_args);
   yURG_urgs   (4, x_args);
   yrpn__unit_quick ();
   return 0;
}

char       /*----: set up programgents/debugging -----------------------------*/
yrpn__unit_quiet    (void)
{
   yrpn__unit_quick ();
   return 0;
}

char       /*----: set up program urgents/debugging --------------------------*/
yrpn__unit_end      (void)
{
   DEBUG_YRPN   yLOGS_end     ();
   return 0;
}



/*============================----end-of-source---============================*/
