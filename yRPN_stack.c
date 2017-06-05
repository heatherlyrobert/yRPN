/*============================----beg-of-source---============================*/

#include  "yRPN.h"
#include  "yRPN_priv.h"



char s_divider [5] = " ";
char s_divtech [5] = " ";



/*===[[ STACK ]]==============================================================*/
#define   S_MAX_STACK         100
typedef     struct cSTACK    tSTACK;
struct cSTACK {
   char        type;
   char        prec;
   char        name        [S_LEN_TOKEN];
   int         pos;
};
static tSTACK  s_stack     [S_MAX_STACK];
static int     s_nstack    = 0;



/*====================------------------------------------====================*/
/*===----                     program wide                             ----===*/
/*====================------------------------------------====================*/
static void        o___PROGRAM_________o () { return; }

char         /*--> initialize the stack ------------------[--------[--------]-*/
yRPN_stack_init      (void)
{
   /*---(header)-------------------------*/
   DEBUG_YRPN    yLOG_senter  (__FUNCTION__);
   /*---(stack)--------------------------*/
   DEBUG_YRPN    yLOG_snote   ("clear stack");
   s_nstack       = 0;
   DEBUG_YRPN    yLOG_sint    (s_nstack);
   /*---(output)-------------------------*/
   DEBUG_YRPN    yLOG_snote   ("clear output");
   strlcpy (rpn.shuntd  ,"" , S_LEN_OUTPUT);
   strlcpy (rpn.detail  ,"" , S_LEN_OUTPUT);
   strlcpy (rpn.normal  ,"" , S_LEN_OUTPUT);
   strlcpy (rpn.tokens  ,"" , S_LEN_OUTPUT);
   rpn.l_shuntd   = 0;
   rpn.l_normal   = 0;
   rpn.n_shuntd   = 0;
   rpn.n_tokens   = 0;
   DEBUG_YRPN    yLOG_sint    (rpn.n_shuntd);
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_sexit   (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       stack additions                        ----===*/
/*====================------------------------------------====================*/
static void        o___STACK_ON________o () { return; }

char         /*--> push a token onto the stack -----------[--------[--------]-*/
yRPN_stack_push      (int a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   /*---(defense)------------------------*/
   --rce;  if ((s_nstack + 1) >= S_MAX_STACK)  return rce;  /* no room */
   /*---(save entry)---------------------*/
   s_stack [s_nstack].type = rpn.t_type;
   s_stack [s_nstack].prec = rpn.t_prec;
   strlcpy (s_stack [s_nstack].name, rpn.t_name, S_LEN_TOKEN);
   s_stack [s_nstack].pos  = a_pos;
   /*---(update counters)----------------*/
   ++s_nstack;
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        stack removals                        ----===*/
/*====================------------------------------------====================*/
static void        o___STACK_OFF_______o () { return; }

char         /*--> peek at the top of the stack ----------[-leaf---[--------]-*/
yRPN_stack_peek         (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   /*---(default)------------------------*/
   rpn.p_type = '-';
   rpn.p_prec = 'a';
   strlcpy (rpn.p_token, "", S_LEN_TOKEN);
   /*---(defense)------------------------*/
   DEBUG_YRPN_M  yLOG_sint    (s_nstack);
   --rce;  if (s_nstack <= 0) {
      DEBUG_YRPN_M  yLOG_snote   ("empty, defaults");
      DEBUG_YRPN_M  yLOG_schar   (rpn.p_type);
      DEBUG_YRPN_M  yLOG_schar   (rpn.p_prec);
      DEBUG_YRPN_M  yLOG_snote   (rpn.p_token);
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(retreive data)------------------*/
   DEBUG_YRPN_M  yLOG_snote   ("success");
   rpn.p_type = s_stack [s_nstack - 1].type;
   rpn.p_prec = s_stack [s_nstack - 1].prec;
   strlcpy (rpn.p_token, s_stack [s_nstack - 1].name, S_LEN_TOKEN);
   DEBUG_YRPN_M  yLOG_schar   (rpn.p_type);
   DEBUG_YRPN_M  yLOG_schar   (rpn.p_prec);
   DEBUG_YRPN_M  yLOG_snote   (rpn.p_token);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> pop and save the top entry ------------[-leaf---[--------]-*/
yRPN_stack_pops     (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        x_div       [S_LEN_LABEL];
   char        x_token     [S_LEN_TOKEN];
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YRPN_M  yLOG_sint    (s_nstack);
   --rce;  if (s_nstack <= 0) {
      DEBUG_YRPN_M  yLOG_snote   ("empty");
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(adjust stack)-------------------*/
   --s_nstack;
   DEBUG_YRPN_M  yLOG_sint    (s_nstack);
   /*---(shuntd output)------------------*/
   DEBUG_YRPN_M  yLOG_snote   ("write shuntd");
   if (rpn.n_shuntd == 0)    strlcpy (x_div, ""        , S_LEN_LABEL);
   else                      strlcpy (x_div, s_divider , S_LEN_LABEL);
   strlcat (rpn.shuntd, x_div           , S_LEN_OUTPUT);
   strlcat (rpn.shuntd, s_stack [s_nstack].name, S_LEN_OUTPUT);
   /*---(normal stack)-------------------*/
   DEBUG_YRPN_M  yLOG_snote   ("write detail");
   if (rpn.n_shuntd == 0)    strlcpy (x_div, ""        , S_LEN_LABEL);
   else                      strlcpy (x_div, s_divtech , S_LEN_LABEL);
   strlcat (rpn.detail, x_div           , S_LEN_OUTPUT);
   sprintf (x_token, "%c,%s", s_stack [s_nstack].type, s_stack [s_nstack].name);
   strlcat (rpn.detail, x_token         , S_LEN_OUTPUT);
   /*---(update counters)----------------*/
   DEBUG_YRPN_M  yLOG_snote   ("update counters");
   ++rpn.n_shuntd;
   rpn.l_shuntd = strlen (rpn.shuntd);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> pop and toss the top entry ------------[-leaf---[--------]-*/
yRPN_stack_toss   (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YRPN_M  yLOG_sint    (s_nstack);
   --rce;  if (s_nstack <= 0) {
      DEBUG_YRPN_M  yLOG_snote   ("empty");
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(update counters)----------------*/
   DEBUG_YRPN_M  yLOG_snote   ("update counters");
   --s_nstack;
   DEBUG_YRPN_M  yLOG_sint    (s_nstack);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       specialty moves                        ----===*/
/*====================------------------------------------====================*/
static void        o___SPECIALTY_______o () { return; }


/*> /+---(handle it)------------------------+/                                                    <* 
 *> yRPN__precedence ();                                                                          <* 
 *> yRPN_stack_infix      ();                                                                          <* 
 *> yRPN_stack_peek       ();                                                                     <* 
 *> DEBUG_OPER  yLOG_complex ("prec"      , "curr=%c, stack=%c", rpn.t_prec, rpn.p_prec);         <* 
 *> zRPN_DEBUG  printf("      precedence %c versus stack top of %c\n", rpn.t_prec, rpn.p_prec);   <* 
 *> if ( (rpn.t_dir == S_LEFT && rpn.t_prec >= rpn.p_prec) ||                                     <* 
 *>       (rpn.t_dir == S_RIGHT && rpn.t_prec >  rpn.p_prec)) {                                   <* 
 *>    while ((rpn.t_dir == S_LEFT && rpn.t_prec >= rpn.p_prec) ||                                <* 
 *>          (rpn.t_dir == S_RIGHT && rpn.t_prec >  rpn.p_prec)) {                                <* 
 *>       /+> if (rpn__last != 'z') RPN__pops();                                       <+/        <* 
 *>       if (rpn.p_prec == 'z') break;                                                           <* 
 *>       yRPN_stack_pops ();                                                                     <* 
 *>       yRPN_stack_peek();                                                                      <* 
 *>    }                                                                                          <* 
 *>    yRPN_stack_push(a_pos);                                                                    <* 
 *>    yRPN_stack_normal (a_pos);                                                                      <* 
 *> } else {                                                                                      <* 
 *>    yRPN_stack_push(a_pos);                                                                    <* 
 *>    yRPN_stack_normal (a_pos);                                                                      <* 
 *> }                                                                                             <*/

char
yRPN_stack_oper      (int a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   char        x_prec      = '-';
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_enter   (__FUNCTION__);
   DEBUG_YRPN_M  yLOG_char    ("t_dir"     , rpn.t_dir);
   DEBUG_YRPN_M  yLOG_char    ("t_prec"    , rpn.t_prec);
   /*---(prepare cutoff)-----------------*/
   if (rpn.t_dir == S_LEFT)  x_prec = rpn.t_prec + 1;
   else                      x_prec = rpn.t_prec;
   DEBUG_YRPN_M  yLOG_char    ("x_prec"    , x_prec);
   /*---(check stack)--------------------*/
   --rce;
   rc = yRPN_stack_peek ();
   DEBUG_YRPN_M  yLOG_value   ("peek_rc"   , rc);
   while (rc >= 0  &&  rpn.p_prec < x_prec) {
      DEBUG_YRPN_M  yLOG_char    ("p_type"    , rpn.p_type);
      DEBUG_YRPN_M  yLOG_char    ("p_prec"    , rpn.p_prec);
      rc = yRPN_stack_pops  ();
      DEBUG_YRPN_M  yLOG_value   ("pops_rc"   , rc);
      /*> if (rc < 0)  return rce;                                                    <*/
      rc = yRPN_stack_peek ();
      DEBUG_YRPN_M  yLOG_value   ("peek_rc"   , rc);
   }
   /*---(push operator)------------------*/
   rc = yRPN_stack_push   (a_pos);
   DEBUG_YRPN_M  yLOG_value   ("push_rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_YRPN_M  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   rc = yRPN_stack_normal (a_pos);
   DEBUG_YRPN_M  yLOG_value   ("normal_rc" , rc);
   --rce;  if (rc < 0) {
      DEBUG_YRPN_M  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_exit    (__FUNCTION__);
   return 0;
   /*---(OLD LOGIC)----------------------*/
   /*> rc = yRPN_stack_peek ();                                                                 <* 
    *> if ( (rpn.t_dir == S_LEFT && rpn.p_prec <  rpn.t_prec) ||                                <* 
    *>       (rpn.t_dir == S_RIGHT && rpn.t_prec >  rpn.p_prec)) {                              <* 
    *>    while ((rpn.t_dir == S_LEFT && rpn.t_prec >= rpn.p_prec) ||                           <* 
    *>          (rpn.t_dir == S_RIGHT && rpn.t_prec >  rpn.p_prec)) {                           <* 
    *>       /+> if (rpn__last != 'z') RPN__pops();                                       <+/   <* 
    *>       if (rpn.p_prec == 'z') break;                                                      <* 
    *>       rc = yRPN_stack_pops ();                                                           <* 
    *>       rc = yRPN_stack_peek();                                                            <* 
    *>    }                                                                                     <* 
    *>    rc = yRPN_stack_push(a_pos);                                                          <* 
    *>    yRPN_stack_normal (a_pos);                                                            <* 
    *> } else {                                                                                 <* 
    *>    rc = yRPN_stack_push(a_pos);                                                          <* 
    *>    yRPN_stack_normal (a_pos);                                                            <* 
    *> }                                                                                        <*/
}

char
yRPN_stack_paren     (int a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   /*---(cycle)--------------------------*/
   rc = yRPN_stack_peek ();
   while (rc >= 0  &&  rpn.p_prec < rpn.t_prec) {
      rc = yRPN_stack_pops  ();
      rc = yRPN_stack_peek ();
   }
   /*---(check for errors)---------------*/
   if (rc < 0) {
      zRPN_DEBUG  printf ("      FATAL :: nothing more on stack\n");
      return rc;
   }
   /*---(throw away open paren)----------*/
   rc = yRPN_stack_toss ();
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       moving to output                       ----===*/
/*====================------------------------------------====================*/
static void        o___OUTPUT__________o () { return; }

char             /* [------] save current item to final output ---------------*/
yRPN_stack_shuntd         (void)
{
   /*---(locals)-----------+-----------+-*/
   char        x_div       [S_LEN_LABEL];
   char        x_token     [S_LEN_TOKEN];
   /*---(shuntd output)------------------*/
   if (rpn.n_shuntd == 0)    strlcpy (x_div, ""        , S_LEN_LABEL);
   else                      strlcpy (x_div, s_divider , S_LEN_LABEL);
   strlcat (rpn.shuntd, x_div           , S_LEN_OUTPUT);
   strlcat (rpn.shuntd, rpn.t_name      , S_LEN_OUTPUT);
   /*---(normal stack)-------------------*/
   if (rpn.n_shuntd == 0)    strlcpy (x_div, ""        , S_LEN_LABEL);
   else                      strlcpy (x_div, s_divtech , S_LEN_LABEL);
   strlcat (rpn.detail, x_div           , S_LEN_OUTPUT);
   sprintf (x_token, "%c,%s", rpn.t_type, rpn.t_name);
   strlcat (rpn.detail, x_token         , S_LEN_OUTPUT);
   /*---(update counters)----------------*/
   ++rpn.n_shuntd;
   rpn.l_shuntd = strlen (rpn.shuntd);
   /*---(complete)-----------------------*/
   return 0;
}

char             /* [------] put current item on normal output ---------------*/
yRPN_stack_normal       (int a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        x_div       [S_LEN_LABEL];
   char        x_token     [S_LEN_TOKEN];
   /*---(shuntd output)------------------*/
   if (rpn.n_shuntd == 0)    strlcpy (x_div, ""        , S_LEN_LABEL);
   else                      strlcpy (x_div, s_divtech , S_LEN_LABEL);
   /*---(add token)----------------------*/
   strlcat (rpn.normal, x_div      , S_LEN_OUTPUT);
   /*> if (rpn.t_type == S_TTYPE_GROUP)  return 0;                                                <*/
   /*> if (rpn.t_type == S_TTYPE_OPER && strcmp(rpn.t_name, ",") == 0)  return 0;                 <*/
   /*> printf ("found a comma, skipping  %c, %c, %s\n", zRPN_lang, rpn.t_type, rpn.t_name);    <* 
    *> if (zRPN_lang == S_LANG_GYGES && rpn.t_type == S_TTYPE_OPER && strcmp(rpn.t_name, ",") == 0)  return 0;   <* 
    *> printf ("just didn't skip\n");                                                       <*/
   sprintf (x_token, "%c,%04d,%s", rpn.t_type, a_pos, rpn.t_name);
   strlcat (rpn.normal, x_token    , S_LEN_OUTPUT);
   rpn.l_normal = strlen (rpn.normal);
   zRPN_DEBUG  printf("      RPN__normal     :: (---) %s\n", x_token);
   return 0;
}

char       /*----: save a token to the tokens output -------------------------*/
yRPN_stack_infix        (void)
{
   /*---(locals)-----------+-----------+-*/
   char        x_div       [S_LEN_LABEL];
   /*---(adapt divider)------------------*/
   if (rpn.n_tokens == 0)    strlcpy (x_div, ""        , S_LEN_LABEL);
   else                      strlcpy (x_div, s_divider , S_LEN_LABEL);
   /*> printf ("rpn.tokens before %2d:%s\n", rpn.n_tokens, rpn.tokens);               <*/
   /*---(add token)----------------------*/
   strlcat (rpn.tokens, x_div      , S_LEN_OUTPUT);
   strlcat (rpn.tokens, rpn.t_name , S_LEN_OUTPUT);
   /*---(update counters)----------------*/
   ++(rpn.n_tokens);
   /*> printf ("rpn.tokens after  %2d:%s\n", rpn.n_tokens, rpn.tokens);               <*/
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

char*      /* ---- : answer unit testing gray-box questions ------------------*/
yRPN_stack_unit      (char *a_question, int a_item)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        x_temp      [S_LEN_LABEL];
   /*---(stack)--------------------------*/
   if        (strcmp (a_question, "stack_depth"   )  == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "stack depth      : %d"    , s_nstack);
   } else if (strcmp (a_question, "stack_list"    )  == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "stack details    :");
      for (i = 0; i < s_nstack; ++i) {
         sprintf (x_temp     , " %c,%c,%s", s_stack [i].type, s_stack [i].prec, s_stack [i].name);
         strlcat (unit_answer, x_temp       , S_LEN_OUTPUT);
      }
   } else if (strcmp (a_question, "stack_terse"   )  == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "stack terse      :");
      for (i = 0; i < s_nstack && i < 6; ++i) {
         sprintf (x_temp     , " %c%c", s_stack [i].type, s_stack [i].prec);
         strlcat (unit_answer, x_temp       , S_LEN_OUTPUT);
      }
   }
   /*---(UNKNOWN)------------------------*/
   else {
      snprintf(unit_answer, S_LEN_OUTPUT, "UNKNOWN          : question is not understood");
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}





/*============================----end-of-source---============================*/
