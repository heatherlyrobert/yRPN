/*============================----beg-of-source---============================*/
#include  "yRPN.h"
#include  "yRPN_priv.h"

/*
 * test first
 *
 * independent of the rest of yRPN, only requires yrpn_oper_spacing
 * in pretty/mathy function
 *
 */



/*====================------------------------------------====================*/
/*===----                        program level                         ----===*/
/*====================------------------------------------====================*/
static void      o___PROGRAM_________________o (void) {;};

char
yrpn_output_init        (void)
{
   /*---(header)-------------------------*/
   DEBUG_YRPN     yLOG_senter  (__FUNCTION__);
   /*---(line variables)-----------------*/
   myRPN.line_done  = S_LINE_OPEN;
   myRPN.line_type  = S_LINE_NORMAL;
   myRPN.line_sect  = '-';
   myRPN.level      = 0;
   /*---(output)-------------------------*/
   DEBUG_YRPN     yLOG_snote   ("clear output");
   ystrlcpy (myRPN.shunted ,"" , LEN_RECD);
   ystrlcpy (myRPN.detail  ,"" , LEN_RECD);
   ystrlcpy (myRPN.debug   ,"" , LEN_RECD);
   ystrlcpy (myRPN.parsed  ,"" , LEN_RECD);
   ystrlcpy (myRPN.tokens  ,"" , LEN_RECD);
   ystrlcpy (myRPN.pretty  ,"" , LEN_RECD);
   ystrlcpy (myRPN.mathy   ,"" , LEN_RECD);
   ystrlcpy (myRPN.exact   ,"" , LEN_RECD);
   myRPN.n_shunted  = 0;
   myRPN.l_shunted  = 0;
   myRPN.n_tokens   = 0;
   myRPN.l_tokens   = 0;
   DEBUG_YRPN     yLOG_sint    (myRPN.n_shunted);
   /*---(saved vars)---------------------*/
   ystrlcpy (myRPN.l_name  ,"" , LEN_FULL);
   myRPN.l_type     = S_TTYPE_NONE;
   myRPN.l_prec     = S_PREC_NONE;
   ystrlcpy (myRPN.s_name  ,"" , LEN_FULL);
   myRPN.s_type     = S_TTYPE_NONE;
   myRPN.s_prec     = S_PREC_NONE;
   /*---(complete)-----------------------*/
   DEBUG_YRPN     yLOG_sexit   (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     writing to rpn                           ----===*/
/*====================------------------------------------====================*/
static void      o___RPN_____________________o (void) {;};

char             /* [------] put current item on normal output ---------------*/
yrpn_output__shunted    (uchar a_type, uchar a_name [LEN_FULL], short a_pos, uchar a_div [LEN_SHORT])
{
   /*---(locals)-----------+-----+-----+-*/
   char        x_div       [LEN_SHORT];
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   /*---(make divider)-------------------*/
   if (myRPN.n_shunted == 0)  ystrlcpy (x_div, ""   , LEN_SHORT);
   else                       ystrlcpy (x_div, a_div, LEN_SHORT);
   /*---(add token)----------------------*/
   DEBUG_YRPN_M  yLOG_snote   ("write normal");
   ystrlcat (myRPN.shunted, x_div  , LEN_RECD);
   ystrlcat (myRPN.shunted, a_name , LEN_RECD);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return 0;
}

char             /* [------] put current item on normal output ---------------*/
yrpn_output__detail     (uchar a_type, uchar a_name [LEN_FULL], short a_pos, uchar a_div [LEN_SHORT])
{
   /*---(locals)-----------+-----------+-*/
   char        x_div       [LEN_SHORT];
   char        x_token     [LEN_FULL];
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   /*---(add token)----------------------*/
   DEBUG_YRPN_M  yLOG_snote   ("write detail");
   if (myRPN.n_shunted == 0)  ystrlcpy (x_div, ""   , LEN_SHORT);
   else                       sprintf (x_div, a_div, LEN_SHORT);
   ystrlcat (myRPN.detail, x_div           , LEN_RECD);
   sprintf (x_token, "%c,%s", a_type, a_name);
   ystrlcat (myRPN.detail, x_token         , LEN_RECD);
   /*---(add token)----------------------*/
   DEBUG_YRPN_M  yLOG_snote   ("write exact");
   if (myRPN.n_shunted == 0)  ystrlcpy (x_div, ""   , LEN_SHORT);
   else                       sprintf (x_div, "Œ"  , LEN_SHORT);
   ystrlcat (myRPN.exact , x_div           , LEN_RECD);
   ystrlcat (myRPN.exact , x_token         , LEN_RECD);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yrpn_output__debug      (uchar a_type, uchar a_name [LEN_FULL], short a_pos, uchar a_div [LEN_SHORT])
{
   /*---(locals)-----------+-----------+-*/
   char        x_div       [LEN_SHORT];
   char        x_token     [LEN_FULL];
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   /*---(make divider)-------------------*/
   if (myRPN.n_shunted == 0)  ystrlcpy (x_div, ""   , LEN_SHORT);
   else                       sprintf (x_div, a_div, LEN_SHORT);
   /*---(add token)----------------------*/
   DEBUG_YRPN_M  yLOG_snote   ("write debug");
   ystrlcat (myRPN.debug , x_div      , LEN_RECD);
   sprintf (x_token, "%c,%d,%s", a_type, a_pos, a_name);
   ystrlcat (myRPN.debug , x_token    , LEN_RECD);
   myRPN.l_tokens = strlen (myRPN.debug );
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yrpn_output_rpn         (uchar a_type, uchar a_prec, uchar a_name [LEN_FULL], short a_pos)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YRPN_M  yLOG_char    ("a_type"    , a_type);
   --rce;  if (a_type == 0 || strchr (YRPN_TYPES YSTR_NUMBER, a_type) == NULL) {
      DEBUG_YRPN_M  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YRPN_M  yLOG_char    ("a_prec"    , a_prec);
   --rce;  if (a_prec != '-') {
      if (a_prec < 'a' || a_prec > 'z') {
         DEBUG_YRPN_M  yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   DEBUG_YRPN_M  yLOG_point   ("a_name"    , a_name);
   --rce;  if (a_name == NULL) {
      DEBUG_YRPN_M  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YRPN_M  yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos < 0 || a_pos >= LEN_RECD) {
      DEBUG_YRPN_M  yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(write tokents)------------------*/
   DEBUG_YRPN_M  yLOG_info    ("a_name"    , a_name);
   rc = yrpn_output__shunted (a_type, a_name, a_pos, s_divtech);
   DEBUG_YRPN_M  yLOG_value   ("shunted"   , rc);
   rc = yrpn_output__detail  (a_type, a_name, a_pos, s_divtech);
   DEBUG_YRPN_M  yLOG_value   ("detail"    , rc);
   rc = yrpn_output__debug   (a_type, a_name, a_pos, s_divtech);
   DEBUG_YRPN_M  yLOG_value   ("debug"     , rc);
   /*---(save this token)----------------*/
   DEBUG_YRPN_M  yLOG_snote   ("save in s_");
   ystrlcpy (myRPN.s_name, a_name, LEN_LABEL);
   myRPN.s_type  = a_type;
   myRPN.s_prec  = a_prec;
   /*---(statistics)---------------------*/
   ++(myRPN.n_shunted);
   DEBUG_YRPN_M  yLOG_value   ("n"         , myRPN.n_shunted);
   myRPN.l_shunted = strlen (myRPN.shunted);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_exit    (__FUNCTION__);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     writing to tokens                        ----===*/
/*====================------------------------------------====================*/
static void      o___TOKEN___________________o (void) {;};

char
yrpn_output__tokens     (uchar a_type, uchar a_name [LEN_FULL], uchar a_token [LEN_FULL], short a_pos, uchar a_div [LEN_SHORT])
{
   /*---(locals)-----------+-----------+-*/
   char        x_div       [LEN_SHORT] = "";
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   /*---(make divider)-------------------*/
   if (myRPN.n_tokens  == 0)  ystrlcpy (x_div, ""   , LEN_SHORT);
   else                       ystrlcpy (x_div, a_div, LEN_SHORT);
   /*---(add token)----------------------*/
   DEBUG_YRPN_M  yLOG_snote   ("write tokens");
   ystrlcat (myRPN.tokens, x_div      , LEN_RECD);
   ystrlcat (myRPN.tokens, a_name     , LEN_RECD);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yrpn_output__parsed     (uchar a_type, uchar a_name [LEN_FULL], uchar a_token [LEN_FULL], short a_pos, uchar a_div [LEN_SHORT])
{
   /*---(locals)-----------+-----------+-*/
   char        x_div       [LEN_SHORT] = "";
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   /*---(make divider)-------------------*/
   if (myRPN.n_tokens  == 0)  ystrlcpy (x_div, ""   , LEN_SHORT);
   else                       ystrlcpy (x_div, a_div, LEN_SHORT);
   /*---(add token)----------------------*/
   DEBUG_YRPN_M  yLOG_snote   ("write parsed");
   ystrlcat (myRPN.parsed, x_div      , LEN_RECD);
   ystrlcat (myRPN.parsed, a_token    , LEN_RECD);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yrpn_output__pretty     (uchar a_type, uchar a_name [LEN_FULL], uchar a_token [LEN_FULL], short a_pos, uchar a_div [LEN_SHORT])
{
   /*---(locals)-----------+-----------+-*/
   char        x_div       [LEN_SHORT];
   char        x_pre       =  ' ';
   char        x_suf       =  ' ';
   char        x_new       [LEN_LABEL] = "";
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   DEBUG_YRPN_M  yLOG_snote   (a_name);
   /*---(quick-out)----------------------*/
   if (strcmp (myRPN.t_name, "?") == 0) {
      DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
      return 0;
   }
   /*---(make divider)-------------------*/
   if (myRPN.n_tokens  == 0)  ystrlcpy (x_div, ""   , LEN_SHORT);
   else                       ystrlcpy (x_div, a_div, LEN_SHORT);
   /*---(get spacing)--------------------*/
   DEBUG_YRPN_M  yLOG_snote   (myRPN.l_name);
   if (strchr ("o(", myRPN.l_type) != NULL)  yrpn_oper_spacing (myRPN.l_name, NULL  , &x_suf, NULL );
   if (strchr ("o(", a_type      ) != NULL)  yrpn_oper_spacing (a_name      , &x_pre, NULL  , x_new);
   DEBUG_YRPN_M  yLOG_schar   (x_suf);
   DEBUG_YRPN_M  yLOG_schar   (x_pre);
   DEBUG_YRPN_M  yLOG_sint    (myRPN.n_tokens);
   if      (x_suf == '-') ;
   else if (x_pre == '-') ;
   else if (myRPN.n_tokens > 0) {
      ystrlcat (myRPN.pretty, " ", LEN_RECD);
   }
   /*---(add token)----------------------*/
   DEBUG_YRPN_M  yLOG_snote   ("write pretty");
   if (x_new [0] == '\0')  ystrlcat (myRPN.pretty, a_token, LEN_RECD);
   else                    ystrlcat (myRPN.pretty, x_new  , LEN_RECD);
   /*---(save for unit testing)----------*/
   DEBUG_YRPN_M  yLOG_snote   ("save last");
   myRPN.l_type  = a_type;
   ystrlcpy (myRPN.l_name , a_name , LEN_LABEL);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yrpn_output__mathy      (uchar a_type, uchar a_name [LEN_FULL], uchar a_token [LEN_FULL], short a_pos, uchar a_div [LEN_SHORT])
{
   /*---(locals)-----------+-----------+-*/
   char        x_div       [LEN_SHORT];
   char        x_pre       =  ' ';
   char        x_suf       =  ' ';
   char        x_new       [LEN_LABEL] = "";
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   /*---(add token)----------------------*/
   if (myRPN.math == 'y' && strcmp (a_name, "*") == 0) {
      DEBUG_YRPN_M  yLOG_snote   ("leave out multiplies, implied");
      DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
      return 0;
   }
   /*---(get spacing)--------------------*/
   DEBUG_YRPN_M  yLOG_snote   (myRPN.l_name);
   if (strchr ("o(", myRPN.l_type) != NULL)  yrpn_oper_spacing (myRPN.l_name, NULL  , &x_suf, NULL );
   if (strchr ("o(", a_type      ) != NULL)  yrpn_oper_spacing (a_name      , &x_pre, NULL  , x_new);
   /*---(exception)----------------------*/
   if (strcmp (a_name, "*") == 0) {
      DEBUG_YRPN_M  yLOG_snote   ("change multiplies to ´");
      ystrlcpy (x_new, "´", LEN_LABEL);
   }
   /*---(add token)----------------------*/
   DEBUG_YRPN_M  yLOG_snote   ("write mathy");
   if (x_new [0] == '\0')  ystrlcat (myRPN.mathy , a_token, LEN_RECD);
   else                    ystrlcat (myRPN.mathy , x_new  , LEN_RECD);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yrpn_output__linetype   (uchar a_type, uchar a_name [LEN_FULL], uchar a_token [LEN_FULL], short a_pos, uchar a_div [LEN_SHORT])
{
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_enter   (__FUNCTION__);
   /*---(check line done)----------------*/
   if (strcmp (";"     , a_name) == 0)      myRPN.line_done = S_LINE_DONE;
   if (strcmp ("{"     , a_name) == 0)      myRPN.line_done = S_LINE_DONE;
   /*---(check line types)---------------*/
   if (myRPN.n_tokens == 0 && a_type == YRPN_TYPE   ) {
      if (strcmp ("extern", a_name) == 0)   myRPN.line_type = S_LINE_EXTERN;
      else                                  myRPN.line_type = S_LINE_DEF;
   }
   if (myRPN.n_tokens == 0 && a_type == YRPN_OPER && myRPN.lang == YRPN_CBANG) {
      if (strcmp ("#"     , a_name) == 0) {
         myRPN.line_type = S_LINE_PREPROC;
         myRPN.pproc     = S_PPROC_YES;
      }
   }
   if (myRPN.n_tokens >  0 && myRPN.line_type == S_LINE_DEF) {
      if (strcmp ("("     , a_name) == 0)   myRPN.line_type = S_LINE_DEF_FPTR;
      if (strcmp ("="     , a_name) == 0)   myRPN.line_type = S_LINE_DEF_VAR;
      if (strcmp (";"     , a_name) == 0)   myRPN.line_type = S_LINE_DEF_VAR;
      if (a_type == YRPN_FUNC   )           myRPN.line_type = S_LINE_DEF_FUN;
      if (a_type == YRPN_VARS   )           myRPN.line_type = S_LINE_DEF_VAR;
   }
   if (myRPN.n_tokens >  0 && myRPN.line_type == S_LINE_DEF_FUN) {
      if (strcmp ("{"     , a_name) == 0)   myRPN.line_type = S_LINE_DEF_FUN;
      if (strcmp (";"     , a_name) == 0)   myRPN.line_type = S_LINE_DEF_PRO;
   }
   if (myRPN.n_tokens >  0 && myRPN.line_type == S_LINE_DEF_VAR) {
      if (strcmp ("="     , a_name) == 0)   myRPN.line_sect = '=';
   }
   DEBUG_YRPN     yLOG_char    ("line_done" , myRPN.line_done);
   DEBUG_YRPN     yLOG_char    ("line_type" , myRPN.line_type);
   DEBUG_YRPN     yLOG_char    ("line_sect" , myRPN.line_sect);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yrpn_output_infix       (uchar a_type, uchar a_prec, uchar a_name [LEN_FULL], uchar a_token [LEN_FULL], short a_pos)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YRPN_M  yLOG_char    ("a_type"    , a_type);
   --rce;  if (a_type == 0 || strchr (YRPN_TYPES, a_type) == NULL) {
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YRPN_M  yLOG_char    ("a_prec"    , a_prec);
   --rce;  if (a_prec == 0 || strchr (YSTR_LOWER "-", a_prec) == NULL) {
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YRPN_M  yLOG_point   ("a_name"    , a_name);
   --rce;  if (a_name == NULL) {
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YRPN_M  yLOG_info    ("a_name"    , a_name);
   DEBUG_YRPN_M  yLOG_point   ("a_token"   , a_token);
   --rce;  if (a_token == NULL) {
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YRPN_M  yLOG_info    ("a_token"   , a_token);
   DEBUG_YRPN_M  yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos < 0 || a_pos >= LEN_RECD) {
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(determine line type)------------*/
   rc = yrpn_output__linetype (a_type, a_name, a_token, s_divider, a_pos);
   DEBUG_YRPN_M  yLOG_value   ("linetype"  , rc);
   /*---(write tokents)------------------*/
   rc = yrpn_output__tokens   (a_type, a_name, a_token, a_pos, s_divider);
   DEBUG_YRPN_M  yLOG_value   ("tokens"    , rc);
   rc = yrpn_output__parsed   (a_type, a_name, a_token, a_pos, s_divider);
   DEBUG_YRPN_M  yLOG_value   ("parsed"    , rc);
   rc = yrpn_output__pretty   (a_type, a_name, a_token, a_pos, s_divider);
   DEBUG_YRPN_M  yLOG_value   ("pretty"    , rc);
   rc = yrpn_output__mathy    (a_type, a_name, a_token, a_pos, s_divider);
   DEBUG_YRPN_M  yLOG_value   ("mathy"     , rc);
   /*---(statistics)---------------------*/
   ++(myRPN.n_tokens);
   DEBUG_YRPN_M  yLOG_value   ("n"         , myRPN.n_tokens);
   myRPN.l_tokens  = strlen (myRPN.tokens);
   /*---(save this token)----------------*/
   DEBUG_YRPN_M  yLOG_snote   ("save last");
   myRPN.l_type  = a_type;
   myRPN.l_prec  = a_prec;
   ystrlcpy (myRPN.l_name , a_name , LEN_LABEL);
   ystrlcpy (myRPN.l_token, a_token, LEN_LABEL);
   myRPN.l_pos   = a_pos;
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yrpn_output_peek        (uchar *r_type, uchar *r_prec, uchar r_name [LEN_FULL], uchar r_token [LEN_FULL], short *r_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   /*---(default params)-----------------*/
   if (r_type  != NULL)  *r_type = myRPN.l_type;
   if (r_prec  != NULL)  *r_prec = myRPN.l_prec;
   if (r_name  != NULL)  ystrlcpy (r_name , myRPN.l_name , LEN_FULL);
   if (r_token != NULL)  ystrlcpy (r_token, myRPN.l_token, LEN_FULL);
   if (r_pos   != NULL)  *r_pos  = myRPN.l_pos;
   /*---(report-out)---------------------*/
   DEBUG_YRPN_M  yLOG_schar   (myRPN.l_type);
   DEBUG_YRPN_M  yLOG_schar   (myRPN.l_prec);
   DEBUG_YRPN_M  yLOG_snote   (myRPN.l_name);
   DEBUG_YRPN_M  yLOG_snote   (myRPN.l_token);
   DEBUG_YRPN_M  yLOG_sint    (myRPN.l_pos);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return 0;
}




