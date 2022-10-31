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
   char        name        [LEN_FULL];
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
   DEBUG_YRPN     yLOG_senter  (__FUNCTION__);
   /*---(stack)--------------------------*/
   DEBUG_YRPN     yLOG_snote   ("clear stack");
   s_nstack       = 0;
   DEBUG_YRPN     yLOG_sint    (s_nstack);
   /*---(line variables)-----------------*/
   myRPN.line_done  = S_LINE_OPEN;
   myRPN.line_type  = S_LINE_NORMAL;
   myRPN.line_sect  = '-';
   myRPN.paren_lvl  = 0;
   /*---(output)-------------------------*/
   DEBUG_YRPN     yLOG_snote   ("clear output");
   strlcpy (myRPN.shuntd  ,"" , LEN_RECD);
   strlcpy (myRPN.detail  ,"" , LEN_RECD);
   strlcpy (myRPN.normal  ,"" , LEN_RECD);
   strlcpy (myRPN.parsed  ,"" , LEN_RECD);
   strlcpy (myRPN.tokens  ,"" , LEN_RECD);
   strlcpy (myRPN.pretty  ,"" , LEN_RECD);
   myRPN.l_shuntd   = 0;
   myRPN.l_normal   = 0;
   myRPN.n_shuntd   = 0;
   myRPN.n_tokens   = 0;
   DEBUG_YRPN     yLOG_sint    (myRPN.n_shuntd);
   /*---(saved vars)---------------------*/
   strlcpy (myRPN.l_name  ,"" , LEN_FULL);
   myRPN.l_type     = S_TTYPE_NONE;
   myRPN.l_prec     = S_PREC_NONE;
   strlcpy (myRPN.s_name  ,"" , LEN_FULL);
   myRPN.s_type     = S_TTYPE_NONE;
   myRPN.s_prec     = S_PREC_NONE;
   /*---(complete)-----------------------*/
   DEBUG_YRPN     yLOG_sexit   (__FUNCTION__);
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
   s_stack [s_nstack].type = myRPN.t_type;
   s_stack [s_nstack].prec = myRPN.t_prec;
   strlcpy (s_stack [s_nstack].name, myRPN.t_name, LEN_FULL);
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
   strlcpy (s_stack [s_nstack - 1].name, myRPN.p_name, LEN_FULL);
   s_stack [s_nstack - 1].type = myRPN.p_type;
   s_stack [s_nstack - 1].prec = myRPN.p_prec;
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
   myRPN.p_type = S_TTYPE_NONE;
   myRPN.p_prec = S_TTYPE_FUNC;
   strlcpy (myRPN.p_name, "", LEN_FULL);
   /*---(defense)------------------------*/
   DEBUG_YRPN_M  yLOG_sint    (s_nstack);
   --rce;  if (s_nstack <= 0) {
      DEBUG_YRPN_M  yLOG_snote   ("empty, defaults");
      DEBUG_YRPN_M  yLOG_schar   (myRPN.p_type);
      DEBUG_YRPN_M  yLOG_schar   (myRPN.p_prec);
      DEBUG_YRPN_M  yLOG_snote   (myRPN.p_name);
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(retreive data)------------------*/
   DEBUG_YRPN_M  yLOG_snote   ("success");
   myRPN.p_type = s_stack [s_nstack - 1].type;
   myRPN.p_prec = s_stack [s_nstack - 1].prec;
   strlcpy (myRPN.p_name, s_stack [s_nstack - 1].name, LEN_FULL);
   DEBUG_YRPN_M  yLOG_schar   (myRPN.p_type);
   DEBUG_YRPN_M  yLOG_schar   (myRPN.p_prec);
   DEBUG_YRPN_M  yLOG_snote   (myRPN.p_name);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> pop and save the top entry ------------[-leaf---[--------]-*/
yRPN_stack_pops     (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        x_div       [LEN_LABEL];
   char        x_token     [LEN_FULL];
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
   if (myRPN.n_shuntd == 0)    strlcpy (x_div, ""        , LEN_LABEL);
   else                        strlcpy (x_div, s_divider , LEN_LABEL);
   strlcat (myRPN.shuntd, x_div           , LEN_RECD);
   strlcat (myRPN.shuntd, s_stack [s_nstack].name, LEN_RECD);
   DEBUG_YRPN_M  yLOG_snote   (s_stack [s_nstack].name);
   /*> printf ("%s\n", myRPN.shuntd);                                                   <*/
   /*---(normal stack)-------------------*/
   if (myRPN.n_shuntd == 0)    strlcpy (x_div, ""        , LEN_LABEL);
   else                        strlcpy (x_div, s_divtech , LEN_LABEL);
   strlcat (myRPN.detail, x_div           , LEN_RECD);
   sprintf (x_token, "%c,%s", s_stack [s_nstack].type, s_stack [s_nstack].name);
   strlcat (myRPN.detail, x_token         , LEN_RECD);
   DEBUG_YRPN_M  yLOG_snote   (x_token);
   /*---(update counters)----------------*/
   DEBUG_YRPN_M  yLOG_snote   ("update counters");
   ++myRPN.n_shuntd;
   myRPN.l_shuntd = strlen (myRPN.shuntd);
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
   DEBUG_YRPN_M  yLOG_char    ("t_dir"     , myRPN.t_dir);
   DEBUG_YRPN_M  yLOG_char    ("t_prec"    , myRPN.t_prec);
   /*---(prepare cutoff)-----------------*/
   if (myRPN.t_dir == S_LEFT)  x_prec = myRPN.t_prec + 1;
   else                      x_prec = myRPN.t_prec;
   DEBUG_YRPN_M  yLOG_char    ("x_prec"    , x_prec);
   /*---(check stack)--------------------*/
   --rce;
   rc = yRPN_stack_peek ();
   DEBUG_YRPN_M  yLOG_value   ("peek_rc"   , rc);
   while (rc >= 0  && myRPN.p_type != '(' && myRPN.p_prec < x_prec) {
      DEBUG_YRPN_M  yLOG_char    ("p_type"    , myRPN.p_type);
      DEBUG_YRPN_M  yLOG_char    ("p_prec"    , myRPN.p_prec);
      rc = yRPN_stack_pops  ();
      DEBUG_YRPN_M  yLOG_value   ("pops_rc"   , rc);
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
   char        x_match     [LEN_LABEL]  = "--";
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_enter   (__FUNCTION__);
   DEBUG_YRPN_M  yLOG_info    ("t_name"    , myRPN.t_name);
   DEBUG_YRPN_M  yLOG_char    ("t_type"    , myRPN.t_type);
   DEBUG_YRPN_M  yLOG_char    ("t_prec"    , myRPN.t_prec);
   /*---(identify match)-----------------*/
   --rce; if (strcmp (myRPN.t_name, ")" ) == 0)   strlcpy (x_match, "(" , LEN_LABEL);
   else if   (strcmp (myRPN.t_name, "]" ) == 0)   strlcpy (x_match, "[" , LEN_LABEL);
   else if   (strcmp (myRPN.t_name, "}" ) == 0)   strlcpy (x_match, "{" , LEN_LABEL);
   else if   (strcmp (myRPN.t_name, "):") == 0)   strlcpy (x_match, "):", LEN_LABEL);
   else {
      DEBUG_YRPN_M  yLOG_note    ("do not recognize paren type");
      DEBUG_YRPN_M  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YRPN_M  yLOG_info    ("x_match"   , x_match);
   /*---(check for stack)----------------*/
   rc = yRPN_stack_peek ();
   --rce;  if (rc < 0) {
      DEBUG_YRPN_M  yLOG_note    ("stack empty");
      DEBUG_YRPN_M  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(cycle)--------------------------*/
   DEBUG_YRPN_M  yLOG_char    ("p_type"    , myRPN.p_type);
   if (myRPN.p_type != S_TTYPE_CAST) {
      DEBUG_YRPN_M  yLOG_note    ("normal paren/bracket closing");
      DEBUG_YRPN_M  yLOG_info    ("p_name"    , myRPN.p_name);
      DEBUG_YRPN_M  yLOG_char    ("p_type"    , myRPN.p_type);
      while (rc >= 0 && strcmp (myRPN.p_name, x_match) != 0) {
         rc = yRPN_stack_pops ();
         rc = yRPN_stack_peek ();
         DEBUG_YRPN_M  yLOG_info    ("p_name"    , myRPN.p_name);
         DEBUG_YRPN_M  yLOG_char    ("p_type"    , myRPN.p_type);
      }
      /*---(throw away open paren/bracket)--*/
      if (rc >= 0 && strcmp (myRPN.t_name, ")") == 0)   rc = yRPN_stack_toss ();
      if (rc >= 0 && strcmp (myRPN.t_name, "]") == 0)   rc = yRPN_stack_toss ();
   } else {
      DEBUG_YRPN_M  yLOG_note    ("casting parenthesis closing");
      strlcpy (myRPN.t_name, "):", LEN_LABEL);
      DEBUG_YRPN_M  yLOG_info    ("new name"  , myRPN.t_name);
      myRPN.t_type = S_TTYPE_CAST;
      DEBUG_YRPN_M  yLOG_char    ("new type"  , myRPN.t_type);
      yRPN_stack_push   (a_pos);
   }
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yRPN_stack_comma     (int a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   char        x_match     [LEN_LABEL]  = "--";
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_enter   (__FUNCTION__);
   DEBUG_YRPN_M  yLOG_char    ("t_type"    , myRPN.t_type);
   DEBUG_YRPN_M  yLOG_char    ("t_prec"    , myRPN.t_prec);
   /*---(identify match)-----------------*/
   strlcpy (x_match, "(" , LEN_LABEL);
   DEBUG_YRPN_M  yLOG_info    ("x_match"   , x_match);
   /*---(check for stack)----------------*/
   rc = yRPN_stack_peek ();
   --rce;  if (rc < 0) {
      DEBUG_YRPN_M  yLOG_note    ("stack empty");
      DEBUG_YRPN_M  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(cycle)--------------------------*/
   DEBUG_YRPN_M  yLOG_note    ("normal paren/bracket closing");
   DEBUG_YRPN_M  yLOG_info    ("p_name"    , myRPN.p_name);
   DEBUG_YRPN_M  yLOG_char    ("p_type"    , myRPN.p_type);
   while (rc >= 0 && strcmp (myRPN.p_name, x_match) != 0) {
      rc = yRPN_stack_pops ();
      rc = yRPN_stack_peek ();
      DEBUG_YRPN_M  yLOG_info    ("p_name"    , myRPN.p_name);
      DEBUG_YRPN_M  yLOG_char    ("p_type"    , myRPN.p_type);
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
   char        x_div       [LEN_LABEL];
   char        x_token     [LEN_FULL];
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   /*---(shuntd output)------------------*/
   DEBUG_YRPN_M  yLOG_snote   ("write shuntd");
   if (myRPN.n_shuntd == 0)   strlcpy (x_div, ""        , LEN_LABEL);
   else                       strlcpy (x_div, s_divider , LEN_LABEL);
   strlcat (myRPN.shuntd, x_div           , LEN_RECD);
   strlcat (myRPN.shuntd, myRPN.t_name    , LEN_RECD);
   /*> printf ("%s\n", myRPN.shuntd);                                                   <*/
   /*---(normal stack)-------------------*/
   DEBUG_YRPN_M  yLOG_snote   ("write detail");
   if (myRPN.n_shuntd == 0)   strlcpy (x_div, ""        , LEN_LABEL);
   else                       strlcpy (x_div, s_divtech , LEN_LABEL);
   strlcat (myRPN.detail, x_div           , LEN_RECD);
   sprintf (x_token, "%c,%s", myRPN.t_type, myRPN.t_name);
   strlcat (myRPN.detail, x_token         , LEN_RECD);
   /*---(save this token)----------------*/
   DEBUG_YRPN_M  yLOG_snote   ("save in s_");
   strlcpy (myRPN.s_name, myRPN.t_name, LEN_LABEL);
   myRPN.s_type  = myRPN.t_type;
   myRPN.s_prec  = myRPN.t_prec;
   /*---(update counters)----------------*/
   DEBUG_YRPN_M  yLOG_snote   ("counters");
   ++myRPN.n_shuntd;
   myRPN.l_shuntd = strlen (myRPN.shuntd);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return 0;
}

char             /* [------] put current item on normal output ---------------*/
yRPN_stack_normal       (int a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        x_div       [LEN_LABEL];
   char        x_token     [LEN_FULL];
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   /*---(shuntd output)------------------*/
   if (myRPN.n_shuntd == 0)  strlcpy (x_div, ""        , LEN_LABEL);
   else                      strlcpy (x_div, s_divtech , LEN_LABEL);
   /*---(add token)----------------------*/
   DEBUG_YRPN_M  yLOG_snote   ("write normal");
   strlcat (myRPN.normal, x_div      , LEN_RECD);
   sprintf (x_token, "%c,%04d,%s", myRPN.t_type, a_pos, myRPN.t_name);
   strlcat (myRPN.normal, x_token    , LEN_RECD);
   myRPN.l_normal = strlen (myRPN.normal);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return 0;
}

char       /*----: save a token to the tokens output -------------------------*/
yRPN_stack_tokens        (void)
{
   /*---(locals)-----------+-----------+-*/
   char        x_div       [LEN_LABEL];
   char        x_pre       =  ' ';
   char        x_suf       =  ' ';
   char        x_new       [LEN_LABEL] = "";
   /*---(header)-------------------------*/
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(check line done)----------------*/
   if (strcmp (";"     , myRPN.t_name) == 0)      myRPN.line_done = S_LINE_DONE;
   if (strcmp ("{"     , myRPN.t_name) == 0)      myRPN.line_done = S_LINE_DONE;
   /*---(check line types)---------------*/
   if (myRPN.n_tokens == 0 && myRPN.t_type == S_TTYPE_TYPE) {
      if (strcmp ("extern", myRPN.t_name) == 0)   myRPN.line_type = S_LINE_EXTERN;
      else                                      myRPN.line_type = S_LINE_DEF;
   }
   if (myRPN.n_tokens == 0 && myRPN.t_type == S_TTYPE_OPER && zRPN_lang == YRPN_CBANG) {
      if (strcmp ("#"     , myRPN.t_name) == 0) {
         myRPN.line_type = S_LINE_PREPROC;
         myRPN.pproc     = S_PPROC_YES;
      }
   }
   if (myRPN.n_tokens >  0 && myRPN.line_type == S_LINE_DEF) {
      if (strcmp ("("     , myRPN.t_name) == 0)   myRPN.line_type = S_LINE_DEF_FPTR;
      if (strcmp ("="     , myRPN.t_name) == 0)   myRPN.line_type = S_LINE_DEF_VAR;
      if (strcmp (";"     , myRPN.t_name) == 0)   myRPN.line_type = S_LINE_DEF_VAR;
      if (myRPN.t_type == S_TTYPE_FUNC)           myRPN.line_type = S_LINE_DEF_FUN;
      if (myRPN.t_type == S_TTYPE_VARS)           myRPN.line_type = S_LINE_DEF_VAR;
   }
   if (myRPN.n_tokens >  0 && myRPN.line_type == S_LINE_DEF_FUN) {
      if (strcmp ("{"     , myRPN.t_name) == 0)   myRPN.line_type = S_LINE_DEF_FUN;
      if (strcmp (";"     , myRPN.t_name) == 0)   myRPN.line_type = S_LINE_DEF_PRO;
   }
   if (myRPN.n_tokens >  0 && myRPN.line_type == S_LINE_DEF_VAR) {
      if (strcmp ("="     , myRPN.t_name) == 0)   myRPN.line_sect = '=';
   }
   DEBUG_YRPN     yLOG_char    ("line_done" , myRPN.line_done);
   DEBUG_YRPN     yLOG_char    ("line_type" , myRPN.line_type);
   DEBUG_YRPN     yLOG_char    ("line_sect" , myRPN.line_sect);
   /*---(adapt divider)------------------*/
   if (myRPN.n_tokens == 0)    strlcpy (x_div, ""        , LEN_LABEL);
   else                        strlcpy (x_div, s_divider , LEN_LABEL);
   /*> printf ("myRPN.tokens before %2d:%s\n", myRPN.n_tokens, myRPN.tokens);               <*/
   /*---(add token)----------------------*/
   strlcat (myRPN.tokens, x_div        , LEN_RECD);
   strlcat (myRPN.tokens, myRPN.t_name , LEN_RECD);
   /*---(add token)----------------------*/
   strlcat (myRPN.parsed, x_div        , LEN_RECD);
   strlcat (myRPN.parsed, myRPN.t_token, LEN_RECD);
   /*---(pretty mode)--------------------*/
   if (strcmp (myRPN.t_name, "?") != 0) {
      if (strchr ("o(", myRPN.l_type) != NULL)  yRPN_space (myRPN.l_name, NULL  , &x_suf, NULL );
      if (strchr ("o(", myRPN.t_type) != NULL)  yRPN_space (myRPN.t_name, &x_pre, NULL  , x_new);
      /*> printf ("last %-10s %c %c, curr %-10s %c %c, new %-10s\n", myRPN.l_name, myRPN.l_type, x_suf, myRPN.t_name, myRPN.t_type, x_pre, x_new);   <*/
      if      (x_suf == '-') ;
      else if (x_pre == '-') ;
      else if (myRPN.n_tokens > 0)  strlcat (myRPN.pretty, " ", LEN_RECD);
      if (x_new [0] == 0)  strlcat (myRPN.pretty, myRPN.t_token, LEN_RECD);
      else                 strlcat (myRPN.pretty, x_new        , LEN_RECD);
   }
   /*---(save this token)----------------*/
   strlcpy (myRPN.l_name, myRPN.t_name, LEN_LABEL);
   myRPN.l_type  = myRPN.t_type;
   myRPN.l_prec  = myRPN.t_prec;
   /*---(update counters)----------------*/
   ++(myRPN.n_tokens);
   /*> printf ("myRPN.tokens after  %2d:%s\n", myRPN.n_tokens, myRPN.tokens);               <*/
   /*---(complete)-----------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
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
   char        x_temp      [LEN_LABEL];
   /*---(stack)--------------------------*/
   if        (strcmp (a_question, "stack_depth"   )  == 0) {
      snprintf (unit_answer, LEN_RECD, "stack depth      : %d"    , s_nstack);
   } else if (strcmp (a_question, "stack_list"    )  == 0) {
      snprintf (unit_answer, LEN_RECD, "stack details    :");
      for (i = 0; i < s_nstack; ++i) {
         sprintf (x_temp     , " %c,%c,%s", s_stack [i].type, s_stack [i].prec, s_stack [i].name);
         strlcat (unit_answer, x_temp       , LEN_RECD);
      }
   } else if (strcmp (a_question, "stack_terse"   )  == 0) {
      snprintf (unit_answer, LEN_RECD, "stack terse      :");
      for (i = 0; i < s_nstack && i < 6; ++i) {
         sprintf (x_temp     , " %c%c", s_stack [i].type, s_stack [i].prec);
         strlcat (unit_answer, x_temp       , LEN_RECD);
      }
   }
   /*---(line type)----------------------*/
   else if   (strcmp (a_question, "line_type"     )  == 0) {
      strlcpy (unit_answer, "source decision  : ", LEN_RECD);
      switch (myRPN.line_done) {
      case S_LINE_OPEN :
         strlcat (unit_answer, "OPEN ", LEN_RECD);
         break;
      case S_LINE_DONE :
         strlcat (unit_answer, "DONE ", LEN_RECD);
         break;
      default          :
         strlcat (unit_answer, "???? ", LEN_RECD);
         break;
      }
      switch (myRPN.line_type) {
      case S_LINE_EXTERN   :
         strlcat (unit_answer, "external variable definition", LEN_RECD);
         break;
      case S_LINE_DEF      :
         strlcat (unit_answer, "unknown type of definition", LEN_RECD);
         break;
      case S_LINE_DEF_VAR  :
         strlcat (unit_answer, "variable definition", LEN_RECD);
         break;
      case S_LINE_DEF_PRO  :
         strlcat (unit_answer, "function prototype", LEN_RECD);
         break;
      case S_LINE_DEF_FUN  :
         strlcat (unit_answer, "function definition", LEN_RECD);
         break;
      case S_LINE_DEF_FPTR :
         strlcat (unit_answer, "function pointer definition", LEN_RECD);
         break;
      case S_LINE_NORMAL   :
         strlcat (unit_answer, "normal statement", LEN_RECD);
         break;
      default          :
         strlcat (unit_answer, "unknown", LEN_RECD);
         break;
      }
   }
   /*---(UNKNOWN)------------------------*/
   else {
      snprintf(unit_answer, LEN_RECD, "UNKNOWN          : question is not understood");
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}





/*============================----end-of-source---============================*/
