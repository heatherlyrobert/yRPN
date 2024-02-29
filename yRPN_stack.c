/*============================----beg-of-source---============================*/

#include  "yRPN.h"
#include  "yRPN_priv.h"



char s_divider [5] = " ";
char s_divtech [5] = " ";



tSTACK  g_stack     [S_MAX_STACK];
g_nstack    = 0;



/*====================------------------------------------====================*/
/*===----                     program wide                             ----===*/
/*====================------------------------------------====================*/
static void        o___PROGRAM_________o () { return; }

char         /*--> initialize the stack ------------------[--------[--------]-*/
yrpn_stack_init      (void)
{
   /*---(header)-------------------------*/
   DEBUG_YRPN     yLOG_senter  (__FUNCTION__);
   /*---(stack)--------------------------*/
   DEBUG_YRPN     yLOG_snote   ("clear stack");
   g_nstack       = 0;
   DEBUG_YRPN     yLOG_sint    (g_nstack);
   /*---(line variables)-----------------*/
   myRPN.line_done  = S_LINE_OPEN;
   myRPN.line_type  = S_LINE_NORMAL;
   myRPN.line_sect  = '-';
   myRPN.level      = 0;
   /*---(complete)-----------------------*/
   DEBUG_YRPN     yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yrpn_stack_show         (void)
{
   int         i           =    0;
   char        t           [LEN_HUND]  = "";
   /*---(header)-------------------------*/
   DEBUG_YRPN     yLOG_senter  (__FUNCTION__);
   DEBUG_YRPN     yLOG_svalue  ("d", g_nstack);
   /*---(stack)--------------------------*/
   for (i = 0; i < g_nstack; ++i) {
      sprintf (t, "%c,%c,%d,%s", g_stack [i].type, g_stack [i].prec, g_stack [i].pos, g_stack [i].name);
      DEBUG_YRPN     yLOG_snote   (t);
   }
   /*---(complete)-----------------------*/
   DEBUG_YRPN     yLOG_sexit   (__FUNCTION__);
   return 0;

}



/*====================------------------------------------====================*/
/*===----                       stack additions                        ----===*/
/*====================------------------------------------====================*/
static void        o___STACK_ON________o () { return; }

char
yrpn_stack_push         (uchar a_type, uchar a_prec, uchar a_name [LEN_FULL], short a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YRPN_M  yLOG_sint    (g_nstack);
   --rce;  if ((g_nstack + 1) >= S_MAX_STACK) {
      DEBUG_YRPN_M  yLOG_snote   ("full");
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(save entry)---------------------*/
   g_stack [g_nstack].type = a_type;
   g_stack [g_nstack].prec = a_prec;
   ystrlcpy (g_stack [g_nstack].name, a_name, LEN_FULL);
   g_stack [g_nstack].pos  = a_pos;
   DEBUG_YRPN_M  yLOG_snote   (g_stack [g_nstack].name);
   DEBUG_YRPN_M  yLOG_schar   (g_stack [g_nstack].type);
   DEBUG_YRPN_M  yLOG_schar   (g_stack [g_nstack].prec);
   DEBUG_YRPN_M  yLOG_sint    (g_stack [g_nstack].pos );
   /*---(update counters)----------------*/
   ++g_nstack;
   DEBUG_YRPN_M  yLOG_sint    (g_nstack);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   yrpn_stack_show ();
   return 0;
}

char
yrpn_stack_update       (uchar a_type, uchar a_prec, uchar a_name [LEN_FULL])
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YRPN_M  yLOG_sint    (g_nstack);
   --rce;  if (g_nstack <= 0) {
      DEBUG_YRPN_M  yLOG_snote   ("empty, nothing to do");
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YRPN_M  yLOG_snote   ("before");
   DEBUG_YRPN_M  yLOG_snote   (g_stack [g_nstack - 1].name);
   DEBUG_YRPN_M  yLOG_schar   (g_stack [g_nstack - 1].type);
   DEBUG_YRPN_M  yLOG_schar   (g_stack [g_nstack - 1].prec);
   /*---(update except pos)--------------*/
   if (a_type != 0)     g_stack [g_nstack - 1].type = a_type;
   if (a_prec != 0)     g_stack [g_nstack - 1].prec = a_prec;
   if (a_name != NULL)  ystrlcpy (g_stack [g_nstack - 1].name, a_name, LEN_FULL);
   DEBUG_YRPN_M  yLOG_snote   ("after");
   DEBUG_YRPN_M  yLOG_snote   (g_stack [g_nstack - 1].name);
   DEBUG_YRPN_M  yLOG_schar   (g_stack [g_nstack - 1].type);
   DEBUG_YRPN_M  yLOG_schar   (g_stack [g_nstack - 1].prec);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   yrpn_stack_show ();
   return 0;
}



/*====================------------------------------------====================*/
/*===----                        stack removals                        ----===*/
/*====================------------------------------------====================*/
static void        o___STACK_OFF_______o () { return; }

char
yrpn_stack_peek         (uchar *r_type, uchar *r_prec, uchar r_name [LEN_FULL], short *r_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   yrpn_stack_show ();
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   /*---(default params)-----------------*/
   if (r_type != NULL)  *r_type = S_TTYPE_NONE;
   if (r_prec != NULL)  *r_prec = S_TTYPE_NONE;
   if (r_name != NULL)  ystrlcpy (r_name, "", LEN_FULL);
   if (r_pos  != NULL)  *r_pos  = -1;
   /*---(default globals)----------------*/
   myRPN.p_type = S_TTYPE_NONE;
   myRPN.p_prec = YRPN_FUNC   ;
   ystrlcpy (myRPN.p_name, "", LEN_FULL);
   /*---(defense)------------------------*/
   DEBUG_YRPN_M  yLOG_sint    (g_nstack);
   --rce;  if (g_nstack <= 0) {
      DEBUG_YRPN_M  yLOG_snote   ("empty, defaults");
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YRPN_M  yLOG_snote   ("success");
   /*---(save params)--------------------*/
   if (r_type != NULL)  *r_type = g_stack [g_nstack - 1].type;
   if (r_prec != NULL)  *r_prec = g_stack [g_nstack - 1].prec;
   if (r_name != NULL)  ystrlcpy (r_name, g_stack [g_nstack - 1].name, LEN_FULL);
   if (r_pos  != NULL)  *r_pos  = g_stack [g_nstack - 1].pos;
   /*---(save globals)-------------------*/
   myRPN.p_type = g_stack [g_nstack - 1].type;
   myRPN.p_prec = g_stack [g_nstack - 1].prec;
   ystrlcpy (myRPN.p_name, g_stack [g_nstack - 1].name, LEN_FULL);
   /*---(report-out)---------------------*/
   DEBUG_YRPN_M  yLOG_schar   (myRPN.p_type);
   DEBUG_YRPN_M  yLOG_schar   (myRPN.p_prec);
   DEBUG_YRPN_M  yLOG_snote   (myRPN.p_name);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return 0;
}

char yRPN_stack_peek_OLD  (void)  { return yrpn_stack_peek (NULL, NULL, NULL, NULL); }

char
yRPN_stack_peek_DEAD    (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   /*---(default)------------------------*/
   myRPN.p_type = S_TTYPE_NONE;
   myRPN.p_prec = YRPN_FUNC   ;
   ystrlcpy (myRPN.p_name, "", LEN_FULL);
   /*---(defense)------------------------*/
   DEBUG_YRPN_M  yLOG_sint    (g_nstack);
   --rce;  if (g_nstack <= 0) {
      DEBUG_YRPN_M  yLOG_snote   ("empty, defaults");
      DEBUG_YRPN_M  yLOG_schar   (myRPN.p_type);
      DEBUG_YRPN_M  yLOG_schar   (myRPN.p_prec);
      DEBUG_YRPN_M  yLOG_snote   (myRPN.p_name);
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(retreive data)------------------*/
   DEBUG_YRPN_M  yLOG_snote   ("success");
   myRPN.p_type = g_stack [g_nstack - 1].type;
   myRPN.p_prec = g_stack [g_nstack - 1].prec;
   ystrlcpy (myRPN.p_name, g_stack [g_nstack - 1].name, LEN_FULL);
   DEBUG_YRPN_M  yLOG_schar   (myRPN.p_type);
   DEBUG_YRPN_M  yLOG_schar   (myRPN.p_prec);
   DEBUG_YRPN_M  yLOG_snote   (myRPN.p_name);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yrpn_stack_pop      (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        x_div       [LEN_LABEL];
   char        x_token     [LEN_FULL];
   char        c           =    0;
   yrpn_stack_show ();
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YRPN_M  yLOG_value   ("g_stack"   , g_nstack);
   --rce;  if (g_nstack <= 0) {
      DEBUG_YRPN_M  yLOG_note    ("empty");
      DEBUG_YRPN_M  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(adjust stack)-------------------*/
   --g_nstack;
   DEBUG_YRPN_M  yLOG_value   ("g_stack"   , g_nstack);
   /*---(check for casting)--------------*/
   DEBUG_YRPN_M  yLOG_note    (g_stack [g_nstack].name);
   if (strcmp ("(:", g_stack [g_nstack].name) == 0) {
      DEBUG_YRPN_M  yLOG_note    ("skip cast paren");
      DEBUG_YRPN_M  yLOG_exit    (__FUNCTION__);
      return 0;
   }
   if (strcmp ("):", g_stack [g_nstack].name) == 0) {
      DEBUG_YRPN_M  yLOG_note    ("skip cast paren");
      DEBUG_YRPN_M  yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(check for function)-------------*/
   DEBUG_YRPN_M  yLOG_char    ("type"      , g_stack [g_nstack].type);
   if (g_stack [g_nstack].type == YRPN_FUNC) {
      DEBUG_YRPN_M  yLOG_note    ("closing out function");
      /*> c = myRPN.narg [myRPN.level + 1] + '0';                                     <* 
       *> DEBUG_YRPN_M  yLOG_char    ("c"         , c);                               <* 
       *> g_stack [g_nstack].type = c;                                                <*/
   }
   /*---(shunted output)-----------------*/
   yrpn_output_rpn (g_stack [g_nstack].type, g_stack [g_nstack].prec, g_stack [g_nstack].name, g_stack [g_nstack].pos);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*--> pop and toss the top entry ------------[-leaf---[--------]-*/
yrpn_stack__toss   (void)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YRPN_M  yLOG_sint    (g_nstack);
   --rce;  if (g_nstack <= 0) {
      DEBUG_YRPN_M  yLOG_snote   ("empty");
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(update counters)----------------*/
   DEBUG_YRPN_M  yLOG_snote   ("update counters");
   --g_nstack;
   DEBUG_YRPN_M  yLOG_sint    (g_nstack);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   yrpn_stack_show ();
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       specialty moves                        ----===*/
/*====================------------------------------------====================*/
static void        o___SPECIALTY_______o () { return; }

char
yrpn_stack_push_oper    (uchar a_type, uchar a_dir, uchar a_prec, uchar a_name [LEN_FULL], short a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   char        x_prec      = S_PREC_NONE;
   uchar       p_type      = '-';
   uchar       p_prec      = '-';
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_enter   (__FUNCTION__);
   DEBUG_YRPN_M  yLOG_char    ("a_dir"     , a_dir);
   DEBUG_YRPN_M  yLOG_char    ("a_prec"    , a_prec);
   /*---(prepare cutoff)-----------------*/
   if (a_dir == S_LEFT)  x_prec = a_prec + 1;
   else                  x_prec = a_prec;
   DEBUG_YRPN_M  yLOG_char    ("x_prec"    , x_prec);
   /*---(check stack)--------------------*/
   --rce;
   rc = yrpn_stack_peek (&p_type, &p_prec, NULL, NULL);
   DEBUG_YRPN_M  yLOG_value   ("peek_rc"   , rc);
   while (rc >= 0  && p_type != '(' && p_prec < x_prec) {
      DEBUG_YRPN_M  yLOG_char    ("p_type"    , p_type);
      DEBUG_YRPN_M  yLOG_char    ("p_prec"    , p_prec);
      rc = yrpn_stack_pop   ();
      DEBUG_YRPN_M  yLOG_value   ("pops_rc"   , rc);
      rc = yrpn_stack_peek (&p_type, &p_prec, NULL, NULL);
      DEBUG_YRPN_M  yLOG_value   ("peek_rc"   , rc);
   }
   /*---(push operator)------------------*/
   rc = yrpn_stack_push   (a_type, a_prec, a_name, a_pos);
   DEBUG_YRPN_M  yLOG_value   ("push_rc"   , rc);
   --rce;  if (rc < 0) {
      DEBUG_YRPN_M  yLOG_exit    (__FUNCTION__);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yrpn_stack_paren        (uchar a_type, uchar a_prec, uchar a_name [LEN_FULL], short a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   uchar       x_match     [LEN_LABEL] = "--";
   uchar       p_type      = '-';
   uchar       p_name      [LEN_FULL]  = "";
   char        c           =   0;
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_enter   (__FUNCTION__);
   DEBUG_YRPN_M  yLOG_info    ("t_name"    , a_name);
   DEBUG_YRPN_M  yLOG_char    ("t_type"    , a_type);
   DEBUG_YRPN_M  yLOG_char    ("t_prec"    , a_prec);
   /*---(identify match)-----------------*/
   --rce; if (strcmp (a_name, ")" ) == 0)   ystrlcpy (x_match, "(" , LEN_LABEL);
   else if   (strcmp (a_name, "¹" ) == 0)   ystrlcpy (x_match, "¸" , LEN_LABEL);
   else if   (strcmp (a_name, "]" ) == 0)   ystrlcpy (x_match, "[" , LEN_LABEL);
   else if   (strcmp (a_name, "}" ) == 0)   ystrlcpy (x_match, "{" , LEN_LABEL);
   else if   (strcmp (a_name, "):") == 0)   ystrlcpy (x_match, "(:", LEN_LABEL);
   else {
      DEBUG_YRPN_M  yLOG_note    ("do not recognize paren type");
      DEBUG_YRPN_M  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YRPN_M  yLOG_info    ("x_match"   , x_match);
   /*---(check for stack)----------------*/
   rc = yrpn_stack_peek (&p_type, NULL, p_name, NULL);
   --rce;  if (rc < 0) {
      DEBUG_YRPN_M  yLOG_note    ("stack empty");
      DEBUG_YRPN_M  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(cycle)--------------------------*/
   DEBUG_YRPN_M  yLOG_char    ("p_type"    , p_type);
   if (p_type != YRPN_CAST   ) {
      DEBUG_YRPN_M  yLOG_note    ("normal paren/bracket closing");
      DEBUG_YRPN_M  yLOG_info    ("p_name"    , p_name);
      DEBUG_YRPN_M  yLOG_char    ("p_type"    , p_type);
      while (rc >= 0 && strcmp (p_name, x_match) != 0) {
         rc = yrpn_stack_pop  ();
         rc = yrpn_stack_peek (&p_type, NULL, p_name, NULL);
         DEBUG_YRPN_M  yLOG_info    ("p_name"    , p_name);
         DEBUG_YRPN_M  yLOG_char    ("p_type"    , p_type);
         ++c;
      }
      /*---(throw away open paren/bracket)--*/
      if (rc >= 0 && strcmp (a_name, ")") == 0)   rc = yrpn_stack__toss ();
      if (rc >= 0 && strcmp (a_name, "¹") == 0)   rc = yrpn_stack__toss ();
      if (rc >= 0 && strcmp (a_name, "]") == 0)   rc = yrpn_stack__toss ();
   } else {
      DEBUG_YRPN_M  yLOG_note    ("casting parenthesis closing");
      ystrlcpy (a_name, "):", LEN_LABEL);
      DEBUG_YRPN_M  yLOG_info    ("new name"  , a_name);
      a_type = YRPN_CAST   ;
      DEBUG_YRPN_M  yLOG_char    ("new type"  , a_type);
      yrpn_stack_push   (a_type, a_prec, a_name, a_pos);
   }
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yrpn_stack_comma        (uchar a_type, uchar a_prec, uchar a_name [LEN_FULL], short a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         = -10;
   char        rc          =   0;
   char        x_match     [LEN_LABEL]  = "--";
   uchar       p_type      = '-';
   uchar       p_name      [LEN_FULL]  = "";
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_enter   (__FUNCTION__);
   DEBUG_YRPN_M  yLOG_char    ("t_type"    , a_type);
   DEBUG_YRPN_M  yLOG_char    ("t_prec"    , a_prec);
   /*---(identify match)-----------------*/
   ystrlcpy (x_match, "(" , LEN_LABEL);
   DEBUG_YRPN_M  yLOG_info    ("x_match"   , x_match);
   /*---(check for stack)----------------*/
   rc = yrpn_stack_peek (&p_type, NULL, p_name, NULL);
   --rce;  if (rc < 0) {
      DEBUG_YRPN_M  yLOG_note    ("stack empty");
      DEBUG_YRPN_M  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(cycle)--------------------------*/
   DEBUG_YRPN_M  yLOG_note    ("normal paren/bracket closing");
   DEBUG_YRPN_M  yLOG_info    ("p_name"    , p_name);
   DEBUG_YRPN_M  yLOG_char    ("p_type"    , p_type);
   while (rc >= 0 && strcmp (p_name, x_match) != 0) {
      rc = yrpn_stack_pop  ();
      rc = yrpn_stack_peek (&p_type, NULL, p_name, NULL);
      DEBUG_YRPN_M  yLOG_info    ("p_name"    , p_name);
      DEBUG_YRPN_M  yLOG_char    ("p_type"    , p_type);
   }
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_exit    (__FUNCTION__);
   return 0;
}

/*> char                                                                              <* 
 *> yRPN_stack_comma     (int a_pos)                                                  <* 
 *> {                                                                                 <* 
 *>    /+---(locals)-----------+-----------+-+/                                       <* 
 *>    char        rce         = -10;                                                 <* 
 *>    char        rc          =   0;                                                 <* 
 *>    char        x_match     [LEN_LABEL]  = "--";                                   <* 
 *>    /+---(header)-------------------------+/                                       <* 
 *>    DEBUG_YRPN_M  yLOG_enter   (__FUNCTION__);                                     <* 
 *>    DEBUG_YRPN_M  yLOG_char    ("t_type"    , myRPN.t_type);                       <* 
 *>    DEBUG_YRPN_M  yLOG_char    ("t_prec"    , myRPN.t_prec);                       <* 
 *>    /+---(identify match)-----------------+/                                       <* 
 *>    ystrlcpy (x_match, "(" , LEN_LABEL);                                            <* 
 *>    DEBUG_YRPN_M  yLOG_info    ("x_match"   , x_match);                            <* 
 *>    /+---(check for stack)----------------+/                                       <* 
 *>    rc = yRPN_stack_peek_OLD ();                                                   <* 
 *>    --rce;  if (rc < 0) {                                                          <* 
 *>       DEBUG_YRPN_M  yLOG_note    ("stack empty");                                 <* 
 *>       DEBUG_YRPN_M  yLOG_exitr   (__FUNCTION__, rce);                             <* 
 *>       return rce;                                                                 <* 
 *>    }                                                                              <* 
 *>    /+---(cycle)--------------------------+/                                       <* 
 *>    DEBUG_YRPN_M  yLOG_note    ("normal paren/bracket closing");                   <* 
 *>    DEBUG_YRPN_M  yLOG_info    ("p_name"    , myRPN.p_name);                       <* 
 *>    DEBUG_YRPN_M  yLOG_char    ("p_type"    , myRPN.p_type);                       <* 
 *>    while (rc >= 0 && strcmp (myRPN.p_name, x_match) != 0) {                       <* 
 *>       rc = yrpn_stack_pop  ();                                                    <* 
 *>       rc = yRPN_stack_peek_OLD ();                                                <* 
 *>       DEBUG_YRPN_M  yLOG_info    ("p_name"    , myRPN.p_name);                    <* 
 *>       DEBUG_YRPN_M  yLOG_char    ("p_type"    , myRPN.p_type);                    <* 
 *>    }                                                                              <* 
 *>    /+---(complete)-----------------------+/                                       <* 
 *>    DEBUG_YRPN_M  yLOG_exit    (__FUNCTION__);                                     <* 
 *>    return 0;                                                                      <* 
 *> }                                                                                 <*/



/*====================------------------------------------====================*/
/*===----                       moving to output                       ----===*/
/*====================------------------------------------====================*/
static void        o___OUTPUT__________o () { return; }



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
      snprintf (unit_answer, LEN_RECD, "stack depth      : %d"    , g_nstack);
   } else if (strcmp (a_question, "stack_list"    )  == 0) {
      snprintf (unit_answer, LEN_RECD, "stack details    :");
      for (i = 0; i < g_nstack; ++i) {
         sprintf (x_temp     , " %c,%c,%s", g_stack [i].type, g_stack [i].prec, g_stack [i].name);
         ystrlcat (unit_answer, x_temp       , LEN_RECD);
      }
   } else if (strcmp (a_question, "stack_terse"   )  == 0) {
      snprintf (unit_answer, LEN_RECD, "stack terse      :");
      for (i = 0; i < g_nstack && i < 6; ++i) {
         sprintf (x_temp     , " %c%c", g_stack [i].type, g_stack [i].prec);
         ystrlcat (unit_answer, x_temp       , LEN_RECD);
      }
   }
   /*---(line type)----------------------*/
   else if   (strcmp (a_question, "line_type"     )  == 0) {
      ystrlcpy (unit_answer, "source decision  : ", LEN_RECD);
      switch (myRPN.line_done) {
      case S_LINE_OPEN :
         ystrlcat (unit_answer, "OPEN ", LEN_RECD);
         break;
      case S_LINE_DONE :
         ystrlcat (unit_answer, "DONE ", LEN_RECD);
         break;
      default          :
         ystrlcat (unit_answer, "???? ", LEN_RECD);
         break;
      }
      switch (myRPN.line_type) {
      case S_LINE_EXTERN   :
         ystrlcat (unit_answer, "external variable definition", LEN_RECD);
         break;
      case S_LINE_DEF      :
         ystrlcat (unit_answer, "unknown type of definition", LEN_RECD);
         break;
      case S_LINE_DEF_VAR  :
         ystrlcat (unit_answer, "variable definition", LEN_RECD);
         break;
      case S_LINE_DEF_PRO  :
         ystrlcat (unit_answer, "function prototype", LEN_RECD);
         break;
      case S_LINE_DEF_FUN  :
         ystrlcat (unit_answer, "function definition", LEN_RECD);
         break;
      case S_LINE_DEF_FPTR :
         ystrlcat (unit_answer, "function pointer definition", LEN_RECD);
         break;
      case S_LINE_NORMAL   :
         ystrlcat (unit_answer, "normal statement", LEN_RECD);
         break;
      default          :
         ystrlcat (unit_answer, "unknown", LEN_RECD);
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
