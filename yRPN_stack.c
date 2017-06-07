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
   /*---(line variables)-----------------*/
   rpn.line_done  = S_LINE_OPEN;
   rpn.line_type  = S_LINE_NORMAL;
   rpn.paren_lvl  = 0;
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
   /*---(saved vars)---------------------*/
   strlcpy (rpn.l_name  ,"" , S_LEN_TOKEN);
   rpn.l_type     = S_TTYPE_NONE;
   rpn.l_prec     = S_PREC_NONE;
   strlcpy (rpn.s_name  ,"" , S_LEN_TOKEN);
   rpn.s_type     = S_TTYPE_NONE;
   rpn.s_prec     = S_PREC_NONE;
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
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YRPN_M  yLOG_sint    (s_nstack);
   --rce;  if ((s_nstack + 1) >= S_MAX_STACK) {
      DEBUG_YRPN_M  yLOG_snote   ("full");
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(save entry)---------------------*/
   s_stack [s_nstack].type = rpn.t_type;
   s_stack [s_nstack].prec = rpn.t_prec;
   strlcpy (s_stack [s_nstack].name, rpn.t_name, S_LEN_TOKEN);
   s_stack [s_nstack].pos  = a_pos;
   DEBUG_YRPN_M  yLOG_snote   (s_stack [s_nstack].name);
   DEBUG_YRPN_M  yLOG_schar   (s_stack [s_nstack].type);
   DEBUG_YRPN_M  yLOG_schar   (s_stack [s_nstack].prec);
   DEBUG_YRPN_M  yLOG_sint    (s_stack [s_nstack].pos );
   /*---(update counters)----------------*/
   ++s_nstack;
   DEBUG_YRPN_M  yLOG_sint    (s_nstack);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> update the top entry ------------------[--------[--------]-*/
yRPN_stack_update    (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YRPN_M  yLOG_sint    (s_nstack);
   --rce;  if (s_nstack <= 0) {
      DEBUG_YRPN_M  yLOG_snote   ("empty, nothing to do");
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YRPN_M  yLOG_snote   ("before");
   DEBUG_YRPN_M  yLOG_snote   (s_stack [s_nstack - 1].name);
   DEBUG_YRPN_M  yLOG_schar   (s_stack [s_nstack - 1].type);
   DEBUG_YRPN_M  yLOG_schar   (s_stack [s_nstack - 1].prec);
   /*---(update except pos)--------------*/
   strlcpy (s_stack [s_nstack - 1].name, rpn.p_name, S_LEN_TOKEN);
   s_stack [s_nstack - 1].type = rpn.p_type;
   s_stack [s_nstack - 1].prec = rpn.p_prec;
   DEBUG_YRPN_M  yLOG_snote   ("after");
   DEBUG_YRPN_M  yLOG_snote   (s_stack [s_nstack - 1].name);
   DEBUG_YRPN_M  yLOG_schar   (s_stack [s_nstack - 1].type);
   DEBUG_YRPN_M  yLOG_schar   (s_stack [s_nstack - 1].prec);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
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
   rpn.p_type = S_TTYPE_NONE;
   rpn.p_prec = S_TTYPE_FUNC;
   strlcpy (rpn.p_name, "", S_LEN_TOKEN);
   /*---(defense)------------------------*/
   DEBUG_YRPN_M  yLOG_sint    (s_nstack);
   --rce;  if (s_nstack <= 0) {
      DEBUG_YRPN_M  yLOG_snote   ("empty, defaults");
      DEBUG_YRPN_M  yLOG_schar   (rpn.p_type);
      DEBUG_YRPN_M  yLOG_schar   (rpn.p_prec);
      DEBUG_YRPN_M  yLOG_snote   (rpn.p_name);
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(retreive data)------------------*/
   DEBUG_YRPN_M  yLOG_snote   ("success");
   rpn.p_type = s_stack [s_nstack - 1].type;
   rpn.p_prec = s_stack [s_nstack - 1].prec;
   strlcpy (rpn.p_name, s_stack [s_nstack - 1].name, S_LEN_TOKEN);
   DEBUG_YRPN_M  yLOG_schar   (rpn.p_type);
   DEBUG_YRPN_M  yLOG_schar   (rpn.p_prec);
   DEBUG_YRPN_M  yLOG_snote   (rpn.p_name);
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
   /*---(check for casting)--------------*/
   DEBUG_YRPN_M  yLOG_snote   (s_stack [s_nstack].name);
   if (strcmp ("(:", s_stack [s_nstack].name) == 0) {
      DEBUG_YRPN_M  yLOG_snote   ("skip cast paren");
      DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
      return 0;
   }
   if (strcmp ("):", s_stack [s_nstack].name) == 0) {
      DEBUG_YRPN_M  yLOG_snote   ("skip cast paren");
      DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
      return 0;
   }
   /*---(shuntd output)------------------*/
   if (rpn.n_shuntd == 0)    strlcpy (x_div, ""        , S_LEN_LABEL);
   else                      strlcpy (x_div, s_divider , S_LEN_LABEL);
   strlcat (rpn.shuntd, x_div           , S_LEN_OUTPUT);
   strlcat (rpn.shuntd, s_stack [s_nstack].name, S_LEN_OUTPUT);
   DEBUG_YRPN_M  yLOG_snote   (s_stack [s_nstack].name);
   /*> printf ("%s\n", rpn.shuntd);                                                   <*/
   /*---(normal stack)-------------------*/
   if (rpn.n_shuntd == 0)    strlcpy (x_div, ""        , S_LEN_LABEL);
   else                      strlcpy (x_div, s_divtech , S_LEN_LABEL);
   strlcat (rpn.detail, x_div           , S_LEN_OUTPUT);
   sprintf (x_token, "%c,%s", s_stack [s_nstack].type, s_stack [s_nstack].name);
   strlcat (rpn.detail, x_token         , S_LEN_OUTPUT);
   DEBUG_YRPN_M  yLOG_snote   (x_token);
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

char
yRPN_stack_oper      (int a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   char        x_prec      = S_PREC_NONE;
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
}

char
yRPN_stack_paren     (int a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_enter   (__FUNCTION__);
   DEBUG_YRPN_M  yLOG_char    ("t_type"    , rpn.t_type);
   DEBUG_YRPN_M  yLOG_char    ("t_prec"    , rpn.t_prec);
   /*---(check for stack)----------------*/
   rc = yRPN_stack_peek ();
   --rce;  if (rc < 0) {
      DEBUG_YRPN_M  yLOG_note    ("stack empty");
      DEBUG_YRPN_M  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(cycle)--------------------------*/
   DEBUG_YRPN_M  yLOG_char    ("p_type"    , rpn.p_type);
   if (rpn.p_type != S_TTYPE_CAST) {
      DEBUG_YRPN_M  yLOG_note    ("normal parenthesis closing");
      DEBUG_YRPN_M  yLOG_char    ("p_prec"    , rpn.p_prec);
      while (rc >= 0  &&  rpn.p_prec < rpn.t_prec) {
         rc = yRPN_stack_pops  ();
         rc = yRPN_stack_peek ();
         DEBUG_YRPN_M  yLOG_char    ("p_prec"    , rpn.p_prec);
      }
      /*---(throw away open paren)----------*/
      if (strcmp (rpn.t_name, ")") == 0)  rc = yRPN_stack_toss ();
      if (strcmp (rpn.t_name, "]") == 0)  rc = yRPN_stack_toss ();
   } else {
      DEBUG_YRPN_M  yLOG_note    ("casting parenthesis closing");
      strlcpy (rpn.t_name, "):", S_LEN_LABEL);
      DEBUG_YRPN_M  yLOG_info    ("new name"  , rpn.t_name);
      rpn.t_type = S_TTYPE_CAST;
      DEBUG_YRPN_M  yLOG_char    ("new type"  , rpn.t_type);
      yRPN_stack_push   (a_pos);
   }
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_exit    (__FUNCTION__);
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
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   /*---(shuntd output)------------------*/
   DEBUG_YRPN_M  yLOG_snote   ("write shuntd");
   if (rpn.n_shuntd == 0)    strlcpy (x_div, ""        , S_LEN_LABEL);
   else                      strlcpy (x_div, s_divider , S_LEN_LABEL);
   strlcat (rpn.shuntd, x_div           , S_LEN_OUTPUT);
   strlcat (rpn.shuntd, rpn.t_name      , S_LEN_OUTPUT);
   /*> printf ("%s\n", rpn.shuntd);                                                   <*/
   /*---(normal stack)-------------------*/
   DEBUG_YRPN_M  yLOG_snote   ("write detail");
   if (rpn.n_shuntd == 0)    strlcpy (x_div, ""        , S_LEN_LABEL);
   else                      strlcpy (x_div, s_divtech , S_LEN_LABEL);
   strlcat (rpn.detail, x_div           , S_LEN_OUTPUT);
   sprintf (x_token, "%c,%s", rpn.t_type, rpn.t_name);
   strlcat (rpn.detail, x_token         , S_LEN_OUTPUT);
   /*---(save this token)----------------*/
   DEBUG_YRPN_M  yLOG_snote   ("save in s_");
   strlcpy (rpn.s_name, rpn.t_name, S_LEN_LABEL);
   rpn.s_type  = rpn.t_type;
   rpn.s_prec  = rpn.t_prec;
   /*---(update counters)----------------*/
   DEBUG_YRPN_M  yLOG_snote   ("counters");
   ++rpn.n_shuntd;
   rpn.l_shuntd = strlen (rpn.shuntd);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return 0;
}

char             /* [------] put current item on normal output ---------------*/
yRPN_stack_normal       (int a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        x_div       [S_LEN_LABEL];
   char        x_token     [S_LEN_TOKEN];
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   /*---(shuntd output)------------------*/
   if (rpn.n_shuntd == 0)    strlcpy (x_div, ""        , S_LEN_LABEL);
   else                      strlcpy (x_div, s_divtech , S_LEN_LABEL);
   /*---(add token)----------------------*/
   DEBUG_YRPN_M  yLOG_snote   ("write normal");
   strlcat (rpn.normal, x_div      , S_LEN_OUTPUT);
   /*> if (rpn.t_type == S_TTYPE_GROUP)  return 0;                                                <*/
   /*> if (rpn.t_type == S_TTYPE_OPER && strcmp(rpn.t_name, ",") == 0)  return 0;                 <*/
   /*> printf ("found a comma, skipping  %c, %c, %s\n", zRPN_lang, rpn.t_type, rpn.t_name);    <* 
    *> if (zRPN_lang == S_LANG_GYGES && rpn.t_type == S_TTYPE_OPER && strcmp(rpn.t_name, ",") == 0)  return 0;   <* 
    *> printf ("just didn't skip\n");                                                       <*/
   sprintf (x_token, "%c,%04d,%s", rpn.t_type, a_pos, rpn.t_name);
   strlcat (rpn.normal, x_token    , S_LEN_OUTPUT);
   rpn.l_normal = strlen (rpn.normal);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return 0;
}

char       /*----: save a token to the tokens output -------------------------*/
yRPN_stack_tokens        (void)
{
   /*---(locals)-----------+-----------+-*/
   char        x_div       [S_LEN_LABEL];
   /*---(check line done)----------------*/
   if (strcmp (";"     , rpn.t_name) == 0)     rpn.line_done = S_LINE_DONE;
   if (strcmp ("{"     , rpn.t_name) == 0)     rpn.line_done = S_LINE_DONE;
   /*---(check line types)---------------*/
   if (rpn.n_tokens == 0 && rpn.t_type == S_TTYPE_TYPE) {
      if (strcmp ("extern", rpn.t_name) == 0)  rpn.line_type = S_LINE_EXTERN;
      else                                     rpn.line_type = S_LINE_DEF;
   }
   if (rpn.n_tokens >  0 && rpn.line_type == S_LINE_DEF) {
      if (strcmp ("("     , rpn.t_name) == 0)  rpn.line_type = S_LINE_DEF_FPTR;
      if (strcmp ("="     , rpn.t_name) == 0)  rpn.line_type = S_LINE_DEF_VAR;
      if (strcmp (";"     , rpn.t_name) == 0)  rpn.line_type = S_LINE_DEF_VAR;
      if (rpn.t_type == S_TTYPE_FUNC)          rpn.line_type = S_LINE_DEF_FUN;
      if (rpn.t_type == S_TTYPE_VARS)          rpn.line_type = S_LINE_DEF_VAR;
   }
   if (rpn.n_tokens >  0 && rpn.line_type == S_LINE_DEF_FUN) {
      if (strcmp ("{"     , rpn.t_name) == 0)  rpn.line_type = S_LINE_DEF_FUN;
      if (strcmp (";"     , rpn.t_name) == 0)  rpn.line_type = S_LINE_DEF_PRO;
   }
   /*---(adapt divider)------------------*/
   if (rpn.n_tokens == 0)    strlcpy (x_div, ""        , S_LEN_LABEL);
   else                      strlcpy (x_div, s_divider , S_LEN_LABEL);
   /*> printf ("rpn.tokens before %2d:%s\n", rpn.n_tokens, rpn.tokens);               <*/
   /*---(add token)----------------------*/
   strlcat (rpn.tokens, x_div      , S_LEN_OUTPUT);
   strlcat (rpn.tokens, rpn.t_name , S_LEN_OUTPUT);
   /*---(save this token)----------------*/
   strlcpy (rpn.l_name, rpn.t_name, S_LEN_LABEL);
   rpn.l_type  = rpn.t_type;
   rpn.l_prec  = rpn.t_prec;
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
   /*---(line type)----------------------*/
   else if   (strcmp (a_question, "line_type"     )  == 0) {
      strlcpy (unit_answer, "source decision  : ", S_LEN_OUTPUT);
      switch (rpn.line_done) {
      case S_LINE_OPEN :
         strlcat (unit_answer, "OPEN ", S_LEN_OUTPUT);
         break;
      case S_LINE_DONE :
         strlcat (unit_answer, "DONE ", S_LEN_OUTPUT);
         break;
      default          :
         strlcat (unit_answer, "???? ", S_LEN_OUTPUT);
         break;
      }
      switch (rpn.line_type) {
      case S_LINE_EXTERN   :
         strlcat (unit_answer, "external variable definition", S_LEN_OUTPUT);
         break;
      case S_LINE_DEF      :
         strlcat (unit_answer, "unknown type of definition", S_LEN_OUTPUT);
         break;
      case S_LINE_DEF_VAR  :
         strlcat (unit_answer, "variable definition", S_LEN_OUTPUT);
         break;
      case S_LINE_DEF_PRO  :
         strlcat (unit_answer, "function prototype", S_LEN_OUTPUT);
         break;
      case S_LINE_DEF_FUN  :
         strlcat (unit_answer, "function definition", S_LEN_OUTPUT);
         break;
      case S_LINE_DEF_FPTR :
         strlcat (unit_answer, "function pointer definition", S_LEN_OUTPUT);
         break;
      case S_LINE_NORMAL   :
         strlcat (unit_answer, "normal statement", S_LEN_OUTPUT);
         break;
      default          :
         strlcat (unit_answer, "unknown", S_LEN_OUTPUT);
         break;
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
