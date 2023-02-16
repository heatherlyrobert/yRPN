/*============================----beg-of-source---============================*/

#include  "yRPN.h"
#include  "yRPN_priv.h"



char     *v_enders    = ";{";
/*> char     *v_operator  = "|&=!<>+/%+-.?:^~#—–“‘”’∆«»Œ";                            <*/
/*> char     *v_preproc   = "#";                                                      <*/
char     *v_preproc   = "";


/*====================------------------------------------====================*/
/*===----                        overall support                       ----===*/
/*====================------------------------------------====================*/
static void        o___OVERALL_________________o (void) {;}

char
yrpn_syms__classify     (short a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        x_ch        =    0;     /* current character                   */
   int         x_pos       =    0;     /* updated position in input           */
   char        x_type      =  YRPN_VARS   ;
   /*---(skip whitespace)----------------*/
   x_pos = a_pos;
   while (x_pos <=  myRPN.l_working) {
      x_ch = myRPN.working [x_pos];
      if (x_ch == '\0')                  break;
      if (strchr ("≤ ∑", x_ch) == NULL)  break;
      ++x_pos;
   }
   /*---(classify)-----------------------*/
   if (x_ch == '(')  x_type = YRPN_FUNC   ;
   else              x_type = YRPN_VARS   ;
   /*---(complete)-----------------------*/
   return x_type;
}

char
yrpn_syms_one           (short l, char f, char p, char c)
{
   /*---(all)----------------------------*/
   if (c == 0)                                                       return 0;
   /*---(specific)-----------------------*/
   DEBUG_YRPN_M  yLOG_snote   ("SYMBOL");
   if (l == 0) {
      if (strchr ("ï" YSTR_VARL, c) == NULL)                         return 0;
   }
   /*---(locals)-------------------------*/
   else if (f == 'Á') {
      if (l == 1) {
         if (strchr ("Ø>!?@" YSTR_LOWER YSTR_NUMBER, c) == NULL)     return 0;
      } else if (l == 2 && p == '@') {
         if (strchr (YSTR_LOWER YSTR_NUMBER YSTR_GREEK, c) == NULL)  return 0;
      } else                                                         return 0;
   }
   /*---(greek)--------------------------*/
   else if (f != 0 && strchr (YSTR_GREEK, f) != NULL) {
      if (myRPN.math == 'y') {
         if (l == 1  && strchr (YSTR_SUBS , c) == NULL)              return 0;
         if (l >  1)                                                 return 0;
      } else {
         if (l == 1) {
            if      (strchr (YSTR_LOWER YSTR_SUBS , c) == NULL)      return 0;
         }
         else                                                        return 0;
      }
   }
   /*---(normal)-------------------------*/
   else {
      if (myRPN.math == 'y') {
         if (l == 1 && strchr (YSTR_SUBS , c) == NULL)               return 0;
         if (l >  1)                                                 return 0;
      } else {
         if (l >= 25)                                                return 0;
         if (strchr ("ï" YSTR_SUBS , p) != NULL)                     return 0;
         if (strchr ("_" YSTR_ALNUM YSTR_SUBS, c)   == NULL)         return 0;
      }
   }
   /*---(complete)-----------------------*/
   return 1;
}


/*====================------------------------------------====================*/
/*===----                       symbol handling                        ----===*/
/*====================------------------------------------====================*/
static void        o___SYMBOLS_________________o (void) {;}

/*> int          /+--> check for constants -------------------[--------[--------]-+/         <* 
 *> yRPN__constants      (int  a_pos)                                                        <* 
 *> {  /+---(design notes)--------------------------------------------------------+/         <* 
 *>    /+ constants only contain alphnanumerics plus the underscore.              +/         <* 
 *>    /+---(locals)-----------+-----------+-+/                                              <* 
 *>    char        rce         =  -10;     /+ return code for errors              +/         <* 
 *>    int         x_pos       =    0;     /+ updated position in input           +/         <* 
 *>    int         i           =    0;     /+ iterator for keywords               +/         <* 
 *>    int         x_found     =   -1;     /+ index of keyword                    +/         <* 
 *>    /+---(header)------------------------+/                                               <* 
 *>    DEBUG_YRPN     yLOG_enter   (__FUNCTION__);                                           <* 
 *>    /+---(defenses)-----------------------+/                                              <* 
 *>    yrpn_token_error  ();                                                                 <* 
 *>    DEBUG_YRPN     yLOG_value   ("a_pos"     , a_pos);                                    <* 
 *>    --rce;  if (a_pos <  0) {                                                             <* 
 *>       DEBUG_YRPN     yLOG_note    ("start can not be negative");                         <* 
 *>       DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);                                   <* 
 *>       return rce;                                                                        <* 
 *>    }                                                                                     <* 
 *>    /+---(accumulate characters)------------+/                                            <* 
 *>    DEBUG_YRPN     yLOG_note    ("accumulate characters");                                <* 
 *>    myRPN.t_type   = YRPN_CONST   ;                                                       <* 
 *>    x_pos        = a_pos;  /+ starting point +/                                           <* 
 *>    while (yrpn_token_accum (&x_pos) == 0);                                                 <* 
 *>    DEBUG_YRPN     yLOG_info    ("myRPN.t_name", myRPN.t_name);                           <* 
 *>    /+---(try to match constants)-----------+/                                            <* 
 *>    DEBUG_YRPN     yLOG_note    ("search constants");                                     <* 
 *>    for (i = 0; i < MAX_CONST; ++i) {                                                     <* 
 *>       if  (s_consts [i].name [0] == '\0')                   break;                       <* 
 *>       if  (s_consts [i].name [0] != myRPN.t_name [0])         continue;                  <* 
 *>       if  (strcmp (s_consts [i].name, myRPN.t_name ) != 0)    continue;                  <* 
 *>       x_found = i;                                                                       <* 
 *>       DEBUG_YRPN     yLOG_value   ("x_found"   , x_found);                               <* 
 *>       break;                                                                             <* 
 *>    }                                                                                     <* 
 *>    DEBUG_YRPN     yLOG_info    ("token name", myRPN.t_name);                             <* 
 *>    /+---(handle misses)--------------------+/                                            <* 
 *>    --rce;  if (x_found < 0) {                                                            <* 
 *>       yrpn_token_error  ();                                                              <* 
 *>       DEBUG_YRPN     yLOG_note    ("type not found");                                    <* 
 *>       DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);                                   <* 
 *>       return rce;                                                                        <* 
 *>    }                                                                                     <* 
 *>    /+---(save)-----------------------------+/                                            <* 
 *>    DEBUG_YRPN     yLOG_note    ("put constant directly to output");                      <* 
 *>    strlcpy (myRPN.t_token, myRPN.t_name, LEN_FULL);                                      <* 
 *>    yrpn_output_tokens_OLD  ();         /+ strait to tokens list                          +/   <* 
 *>    yrpn_output_rpn   (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);                  <*
 *>    myRPN.left_oper  = S_OPER_CLEAR;                                                      <* 
 *>    myRPN.combined   = S_NO;                                                              <* 
 *>    /+---(complete)-------------------------+/                                            <* 
 *>    DEBUG_YRPN     yLOG_exit    (__FUNCTION__);                                           <* 
 *>    return x_pos;                                                                         <* 
 *> }                                                                                        <*/

int          /*--> check for symbol names ----------------[--------[--------]-*/
yrpn_syms_funcvar       (int   a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /* func/vars only contain alphnanumerics plus the underscore.              */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   char        rc          =    0;     /* generic return code                 */
   int         x_pos       =    0;     /* updated position in input           */
   int         i           =    0;     /* iterator for keywords               */
   int         x_found     =   -1;     /* index of keyword                    */
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
   /*---(accumulate characters)------------*/
   DEBUG_YRPN     yLOG_note    ("accumulate characters");
   myRPN.t_type   = YRPN_VARS   ;
   x_pos        = a_pos;  /* starting point */
   while (yrpn_token_accum (&x_pos) == 0);
   DEBUG_YRPN     yLOG_info    ("myRPN.t_name", myRPN.t_name);
   /*---(check if long enough)-------------*/
   DEBUG_YRPN     yLOG_value   ("myRPN.t_len" , myRPN.t_len);
   --rce;  if (myRPN.t_len <  1) {
      yrpn_token_error  ();
      DEBUG_YRPN     yLOG_note    ("name too short");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check if ready)-------------------*/
   --rce;  if (!yrpn_oper_combining ()) {
      DEBUG_YRPN     yLOG_note    ("does not follow a combining operator");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check func vs vars)---------------*/
   rc = yRPN_stack_peek_OLD ();
   if (myRPN.s_type == YRPN_FPTR   )  myRPN.t_type  = YRPN_FUNC   ;
   else                               myRPN.t_type  = yrpn_syms__classify (x_pos);
   /*---(push functions)-------------------*/
   if (myRPN.t_type == YRPN_FUNC   ) {
      DEBUG_YRPN     yLOG_note    ("put function on stack");
      myRPN.t_prec = S_PREC_FUNC;
      strlcpy (myRPN.t_token, myRPN.t_name, LEN_FULL);
      yrpn_output_infix (myRPN.t_type, myRPN.t_prec, myRPN.t_name, myRPN.t_token, a_pos);
      yrpn_stack_push   (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
   }
   /*---(save variables)-------------------*/
   else {
      DEBUG_YRPN     yLOG_note    ("put variable directly to output");
      if (myRPN.l_type == YRPN_OPER   ) {
         if (strcmp (myRPN.l_name, "." ) == 0)  myRPN.t_type = YRPN_MEMB   ;
         if (strcmp (myRPN.l_name, "->") == 0)  myRPN.t_type = YRPN_MEMB   ;
      }
      if (myRPN.t_name [0] == 'Á')              myRPN.t_type = YRPN_LOCAL   ;
      strlcpy (myRPN.t_token, myRPN.t_name, LEN_FULL);
      /*---(mathy stuff)-----------------*/
      yrpn_oper_mathy (a_pos);
      /*> if (myRPN.math == 'y') {                                                               <* 
       *>    DEBUG_YRPN     yLOG_note    ("mathy multiplier check");                             <* 
       *>    rc = yrpn_output_peek (&x_type, NULL, x_name, NULL, NULL);                          <* 
       *>    DEBUG_YRPN     yLOG_char    ("x_type"    , x_type);                                 <* 
       *>    DEBUG_YRPN     yLOG_info    ("x_name"    , x_name);                                 <* 
       *>    l = strlen (x_name);                                                                <* 
       *>    if (strchr (YRPN_MATHERS, x_type) != NULL) {                                        <* 
       *>       yrpn_oper_mathy (a_pos);                                                         <* 
       *>    } else if (x_type == YRPN_OPER && strchr ("∆«»… ŒÕÃ", x_name [0]) != NULL) {        <* 
       *>       yrpn_oper_mathy (a_pos);                                                         <* 
       *>    } else if (x_type == YRPN_GROUP && l == 1 && strchr (")π", x_name [0]) != NULL) {   <* 
       *>       yrpn_oper_mathy (a_pos);                                                         <* 
       *>    } else {                                                                            <* 
       *>       DEBUG_YRPN     yLOG_note    ("already has an operator");                         <* 
       *>    }                                                                                   <* 
       *> }                                                                                      <*/
      /*---(shared)----------------------*/
      yrpn_output_infix (myRPN.t_type, myRPN.t_prec, myRPN.t_name, myRPN.t_token, a_pos);
      yrpn_output_rpn   (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
      /*---(done)------------------------*/
   }
   myRPN.left_oper  = S_OPER_CLEAR;
   myRPN.combined   = S_NO;
   /*---(complete)-------------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return x_pos;
}



/*====================------------------------------------====================*/
/*===----                          group handling                      ----===*/
/*====================------------------------------------====================*/
static void        o___GROUPING________________o (void) {;}

/*> int                                                                                <* 
 *> yRPN__brack_open     (int a_pos)                                                   <* 
 *> {  /+---(design notes)--------------------------------------------------------+/   <* 
 *>    /+ push a close bracket and open paren to the stack                        +/   <* 
 *>    /+---(handle bracket)-----------------+/                                        <* 
 *>    yrpn_output_tokens_OLD      ();                                                      <* 
 *>    yrpn_output_debug_OLD     (a_pos);                                                  <* 
 *>    /+---(put reverse on stack)-----------+/                                        <* 
 *>    strcpy (myRPN.t_name, "]");                                                     <* 
 *>    myRPN.t_type     = YRPN_OPER   ;                                                <* 
 *>    yrpn_stack_push(a_pos);                                                         <* 
 *>    /+---(pretend open paren)-------------+/                                        <* 
 *>    strcpy (myRPN.t_name, "(");                                                     <* 
 *>    myRPN.t_type     = S_TTYPE_GROUP;                                               <* 
 *>    yrpn_oper_prec ();                                                             <* 
 *>    yrpn_output_tokens_OLD      ();                                                      <* 
 *>    yrpn_stack_push       (a_pos);                                                  <* 
 *>    myRPN.left_oper  = S_OPER_LEFT;                                                 <* 
 *>    /+---(complete)-----------------------+/                                        <* 
 *>    return 0;                                                                       <* 
 *> }                                                                                  <*/


/*> int                                                                                      <* 
 *> yRPN__paren_comma    (int a_pos)                                                         <* 
 *> {                                                                                        <* 
 *>    char rc = 0;                                                                          <* 
 *>    yrpn_output_tokens_OLD ();                                                                 <* 
 *>    rc = yRPN_stack_peek_OLD ();                                                               <* 
 *>    while (rc >= 0  &&  myRPN.p_prec != 'd' + 16) {                                       <* 
 *>       yrpn_stack_pop  ();                                                                <* 
 *>       rc = yRPN_stack_peek_OLD ();                                                            <* 
 *>    }                                                                                     <* 
 *>    if (rc < 0) {                                                                         <* 
 *>       /+> zRPN_DEBUG  printf ("      FATAL :: nothing more on stack\n");           <+/   <* 
 *>       /+> return rc;                                                               <+/   <* 
 *>    }                                                                                     <* 
 *>    if (myRPN.lang != YRPN_GYGES) {                                                        <* 
 *>       myRPN.t_type = YRPN_OPER   ;                                                       <* 
 *>       yrpn_output_shuntd_OLD ();                                                              <* 
 *>       yrpn_output_debug_OLD (a_pos);                                                         <* 
 *>       myRPN.left_oper  = S_OPER_LEFT;                                                    <* 
 *>    }                                                                                     <* 
 *>    return 0;                                                                             <* 
 *> }                                                                                        <*/




/*============================----end-of-source---============================*/
