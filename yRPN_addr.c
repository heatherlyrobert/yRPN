/*============================----beg-of-source---============================*/
#include  "yRPN.h"
#include  "yRPN_priv.h"




static char s_ready   = '-';



/*---[[ private variables ]]------------------------------*/
static char   s_work   [LEN_RECD];
static char   s_final  [LEN_RECD];

static int    s_targb  = 0;    /* target buffer                               */
static int    s_targx  = 0;    /* target z-pos                                */
static int    s_targy  = 0;    /* target y-pos                                */
static int    s_targz  = 0;    /* target z-pos                                */

static int    s_adjb   = 0;    /* adjust buffer                               */
static int    s_adjx   = 0;    /* adjust col                                  */
static int    s_adjy   = 0;    /* adjust row                                  */
static int    s_adjz   = 0;    /* adjust row                                  */



/*---(external functions)-------------*/
static char    (*s_breaker )   (char *a_src, int *a_buf, int *a_x, int *a_y, int *a_z, char *a_abs, int a_def, char a_check);
static char    (*s_maker   )   (int a_buf, int a_x, int a_y, int a_z, char a_abs, char *a_out, char a_check);
static char    (*s_prettier)   (char *a_src, int a_def, char *a_out, char a_check);
static char    (*s_adjuster)   (char *a_src, int a_bo, int a_xo, int a_yo, int a_zo, char x_force, char *a_out, char a_check);
static char    (*s_insider )   (int a_b, int a_x, int a_y, int a_z);



/*====================------------------------------------====================*/
/*===----                       program level                          ----===*/
/*====================------------------------------------====================*/
void  o___PROGRAM_________o () { return; }

char
yrpn_addr_init          (void)
{
   /*---(begin)--------------------------*/
   DEBUG_YRPN    yLOG_senter  (__FUNCTION__);
   DEBUG_YRPN    yLOG_schar   (s_ready);
   /*---(clear pointers)-----------------*/
   s_breaker   = NULL;
   DEBUG_YRPN    yLOG_spoint  (s_breaker);
   s_maker     = NULL;
   DEBUG_YRPN    yLOG_spoint  (s_maker);
   s_prettier  = NULL;
   DEBUG_YRPN    yLOG_spoint  (s_prettier);
   s_adjuster  = NULL;
   DEBUG_YRPN    yLOG_spoint  (s_adjuster);
   s_insider   = NULL;
   DEBUG_YRPN    yLOG_spoint  (s_insider);
   /*---(update status)------------------*/
   s_ready     = 'i';
   DEBUG_YRPN    yLOG_schar   (s_ready);
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yRPN_addr_config        (void *a_breaker, void *a_maker, void *a_prettier, void *a_adjuster, void *a_insider)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;                /* return code for errors    */
   char       *x_valid     = "iy";
   /*---(begin)--------------------------*/
   DEBUG_YRPN    yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   DEBUG_YRPN    yLOG_schar   (s_ready);
   DEBUG_YRPN    yLOG_snote   (x_valid);
   --rce;  if (strchr ("iy", s_ready) == NULL) {
      DEBUG_YRPN    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(save pointers)------------------*/
   s_breaker  = a_breaker;
   DEBUG_YRPN    yLOG_spoint  (s_breaker);
   --rce;  if (s_breaker  == NULL) {
      DEBUG_YRPN    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   s_maker    = a_maker;
   DEBUG_YRPN    yLOG_spoint  (s_maker);
   --rce;  if (s_maker    == NULL) {
      DEBUG_YRPN    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   s_prettier = a_prettier;
   DEBUG_YRPN    yLOG_spoint  (s_prettier);
   --rce;  if (s_prettier == NULL) {
      DEBUG_YRPN    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   s_adjuster = a_adjuster;
   DEBUG_YRPN    yLOG_spoint  (s_adjuster);
   --rce;  if (s_adjuster == NULL) {
      DEBUG_YRPN    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   s_insider  = a_insider;
   DEBUG_YRPN    yLOG_spoint  (s_insider);
   --rce;  if (s_insider  == NULL) {
      DEBUG_YRPN    yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(update status)------------------*/
   s_ready     = 'y';
   DEBUG_YRPN    yLOG_schar   (s_ready);
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_sexit   (__FUNCTION__);
   return 0;
}

char
yrpn_addr_one           (short l, char f, char p, char c)
{
   DEBUG_YRPN_M  yLOG_snote   ("ADDRESS");
   if (c == 0)                                        return 0;
   if (strchr (YSTR_ADDR, c) == NULL)                 return 0;
   return 1;
}



/*====================------------------------------------====================*/
/*===----                     formula modification                     ----===*/
/*====================------------------------------------====================*/
void  o___MODIFY__________o () { return; }

char         /*-> shared argument validiation --------[ ------ [fe.G67.55#.92]*/ /*-[01.0000.018.!]-*/ /*-[--.---.---.--]-*/
yrpn_addr__adj_check    (char *a_src, char a_scope, char *a_target)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;                /* return code for errors    */
   char        rc          =    0;                  /* generic return code       */
   /*---(begin)--------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(rpn)----------------------------*/
   DEBUG_YRPN    yLOG_point   ("a_src"   , a_src);
   --rce;  if (a_src  == NULL)  {
      DEBUG_YRPN    yLOG_note    ("aborted, a_src is NULL, no point");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YRPN    yLOG_info    ("a_src"   , a_src);
   /*---(scope)--------------------------*/
   DEBUG_YRPN    yLOG_char    ("a_scope"   , a_scope);
   DEBUG_YRPN    yLOG_info    ("valid"     , YRPN_ALL);
   --rce;  if (strchr (YRPN_ALL, a_scope) == 0)   {
      DEBUG_YRPN    yLOG_note    ("aborted, a_scope not legal");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(target)-------------------------*/
   DEBUG_YRPN    yLOG_point   ("a_target"  , a_target);
   s_targb = s_targx = s_targy = s_targz = -1;
   --rce;  if (a_target  != NULL)  {
      DEBUG_YRPN    yLOG_info    ("a_target"  , a_target);
      rc = s_breaker (a_target, &s_targb, &s_targx, &s_targy, &s_targz, NULL, 0, YSTR_LEGAL);
      DEBUG_YRPN    yLOG_value   ("rc"        , rc);
      if (rc < 0)  {
         DEBUG_YRPN    yLOG_note    ("could not parse, EXITING");
         DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
         return rce;
      }
   }
   DEBUG_YRPN    yLOG_complex ("target"    , "b=%4d, x=%4d, y=%4d, z=%4d", s_targb, s_targx, s_targy, s_targz);
   /*---(save)---------------------------*/
   if (strchr (YSTR_CALC_PRE, a_src [0]) != NULL)  ystrlcpy (s_work, a_src, LEN_RECD);
   else                                            snprintf (s_work, LEN_RECD, "�%s", a_src);
   ystrlcpy (s_final, "n/a"   , LEN_RECD);
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> change a specific reference --------[ ------ [fe.HA4.498.A3]*/ /*-[02.0000.018.!]-*/ /*-[--.---.---.--]-*/
yrpn_addr__adj_one      (char *a_old, char a_scope, char *a_new)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;               /* generic return code       */
   int         b           =    0;               /* tab of new address        */
   int         x           =    0;               /* col of new address        */
   int         y           =    0;               /* row of new address        */
   int         z           =    0;               /* row of new address        */
   char        x_abs       =    0;               /* abs of new address        */
   char        x_addr      [20]        = "";     /* new address               */
   int         x_bad       =    0;
   char        x_force     =  '-';               /* technical abs of address  */
   /*---(begin)--------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   DEBUG_YRPN    yLOG_info    ("a_old"     , a_old);
   DEBUG_YRPN    yLOG_char    ("a_scope"   , a_scope);
   /*---(prepare)------------------------*/
   strcpy (a_new, a_old);
   /*---(shortcut for do-nothing)--------*/
   --rce;  if (strchr (YRPN_NONES, a_scope) != NULL) {
      DEBUG_YRPN    yLOG_note    ("scope indicates nothing to do");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(parse for address)--------------*/
   rc = s_breaker (a_old, &b, &x, &y, &z, &x_abs, 0, YSTR_LEGAL);
   DEBUG_YRPN    yLOG_value   ("s_breaker" , rc);
   --rce;  if (rc <  0) {
      DEBUG_YRPN    yLOG_note    ("not an address, just append");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check targeted refs)------------*/
   --rce;  if (strchr (YRPN_PROS, a_scope) != NULL) {
      if (s_targb != b || s_targx != x || s_targy != y || s_targz != z) {
         DEBUG_YRPN    yLOG_note    ("tab, col, or row does not match target, just append");
         DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(adjust to scopes)---------------*/
   DEBUG_YRPN    yLOG_value   ("x_abs"     , x_abs);
   --rce;  if (x_abs >  0 && a_scope == YRPN_PREL) {
      DEBUG_YRPN    yLOG_note    ("not an address, just append");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (strchr ("AS", a_scope) != 0) {
      DEBUG_YRPN    yLOG_note    ("force a full change");
      x_force = 'y';
   }
   rc = s_insider (b, x, y, z);
   DEBUG_YRPN    yLOG_value   ("insider"   , rc);
   --rce;  if (rc <= 0 && a_scope == YRPN_RINNER) {
      DEBUG_YRPN    yLOG_note    ("cell label not inner register area");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YRPN    yLOG_info    ("rinside"   , YRPN_RINSIDE);
   --rce;  if (rc > 0  && strchr (YRPN_RINSIDE, a_scope) != NULL) {
      DEBUG_YRPN    yLOG_note    ("force a full change");
      x_force = 'y';
   }
   if (a_scope == YRPN_REVERY)  x_force = 'y';
   if (a_scope == YRPN_PALL)    x_force = 'y';
   /*---(handle addresses)---------------*/
   rc = s_adjuster (a_old, s_adjb, s_adjx, s_adjy, s_adjz, x_force, x_addr, YSTR_LEGAL);
   DEBUG_YRPN    yLOG_value   ("s_adjuster", rc);
   if (rc != 0) {
      strcpy (a_new, "#REF");
      DEBUG_YRPN    yLOG_note    ("#REF, address cant adjust that far");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare return)-----------------*/
   DEBUG_YRPN    yLOG_info    ("new label" , x_addr);
   if (a_scope == YRPN_PSPLIT)  sprintf (a_new, "(%s + %s)", a_old, x_addr);
   else                         strcpy  (a_new, x_addr);
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> change a specific reference --------[ ------ [fe.J75.197.84]*/ /*-[02.0000.037.!]-*/ /*-[--.---.---.--]-*/
yrpn_addr__adj_main     (cchar *a_src, cchar a_scope, cchar *a_target, cint b, cint x, cint y, cint z, char *a_out)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        rc          =    0;
   char        x_tokens    [LEN_RECD] = "";      /* source made into tokens   */
   char        x_final     [LEN_RECD] = "";      /* new version of formula    */
   char       *p           = NULL;               /* strtok field pointer      */
   char       *q           =  " ";               /* strtok delimiters         */
   char       *r           = NULL;               /* strtok context            */
   char        x_new       [LEN_RECD] = "";      /* new element               */
   int         x_bad       =    0;
   char        x_pre       =  '=';
   /*---(begin)--------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (s_ready != 'y') {
      DEBUG_YRPN    yLOG_note    ("yRPN address handling functions not configured");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = yrpn_addr__adj_check (a_src, a_scope, a_target);
   DEBUG_YRPN    yLOG_value   ("check"     , rc);
   --rce;  if (rc < 0)  {
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(adjustment)---------------------*/
   DEBUG_YRPN    yLOG_note    ("save the adjustment");
   s_adjb  = b;
   s_adjx  = x;
   s_adjy  = y;
   s_adjz  = z;
   x_pre   = s_work [0];
   /*---(prepare tokens)-----------------*/
   rc = yRPN_gyges (s_work, &x_tokens, NULL, LEN_RECD, 0);
   DEBUG_YRPN    yLOG_value   ("tokenize"  , rc);
   --rce;  if (rc < 0)  {
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*> rc = yRPN_get   (YRPN_TOKENS , &x_tokens, NULL);                               <*/
   rc = yRPN_get   (YRPN_PARSED , &x_tokens, NULL);
   DEBUG_YRPN    yLOG_info    ("x_tokens"  , x_tokens);
   /*---(parse first token)--------------*/
   p = strtok_r (x_tokens, q, &r);
   DEBUG_YRPN    yLOG_point   ("p"         , p);
   if (p == NULL) {
      DEBUG_YRPN    yLOG_note    ("aborted, very first parse returned NULL");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(loop through tokens)------------*/
   while (p != NULL) {
      rc = yrpn_addr__adj_one (p, a_scope, x_new);
      strcat (x_final, x_new);
      strcat (x_final, " ");
      DEBUG_YRPN    yLOG_info    ("x_final"   , x_final);
      if (strcmp (x_new, "#REF") == 0)  ++x_bad;
      p = strtok_r (NULL    , q, &r);
   }
   /*---(strip final space)--------------*/
   x_final [strlen (x_final) - 1] = '\0';
   /*---(wrap-up)------------------------*/
   sprintf (x_tokens, "%c%s", x_pre, x_final);
   rc = yRPN_gyges (x_tokens, &x_final, NULL, LEN_RECD, 0);
   if (x_pre == '�')  rc = yRPN_get   (YRPN_MATHY , &x_final, NULL);
   else               rc = yRPN_get   (YRPN_PRETTY, &x_final, NULL);
   DEBUG_YRPN    yLOG_info    ("x_tokens"  , x_tokens);
   DEBUG_YRPN    yLOG_info    ("final"     , x_final);
   if (strchr (YSTR_CALC_SPC, x_pre) != NULL)   sprintf (s_final, "%c %s", x_pre, x_final);
   else if (x_pre != '�')                       sprintf (s_final, "%c%s" , x_pre, x_final);
   else                                         sprintf (s_final, "%s"   , x_final);
   if (a_out != NULL)  ystrlcpy (a_out, s_final, LEN_RECD);
   DEBUG_YRPN    yLOG_info    ("a_out"     , a_out);
   /*---(check for ref troubles)---------*/
   DEBUG_YRPN    yLOG_value   ("x_bad"     , x_bad);
   --rce;  if (x_bad > 0) {
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yRPN_addr_normal   (cchar *a_src, cint b, cint x, cint y, cint z, cint a_max, char *a_out)
{
   return yrpn_addr__adj_main (a_src, YRPN_RREL, NULL, b, x, y, z, a_out);
}

/*> char                                                                                                        <* 
 *> yRPN_addr_scoped   (cchar *a_src, cchar a_scope, cint b, cint x, cint y, cint z, cint a_max, char *a_out)   <* 
 *> {                                                                                                           <* 
 *>    return yrpn_addr__adj_main (a_src, a_scope, NULL, b, x, y, z, a_out);                                         <* 
 *> }                                                                                                           <*/

char
yRPN_addr_require  (cchar *a_src, cchar a_scope, cint b, cint x, cint y, cint z, cint a_max, char *a_out)
{
   strcpy (s_final, "n/a");
   if (strchr (YRPN_REQS  , a_scope) == NULL)  return -1;
   return yrpn_addr__adj_main (a_src, a_scope, NULL, b, x, y, z, a_out);
}

char
yRPN_addr_provide  (cchar *a_src, cchar a_scope, cchar *a_target, cint b, cint x, cint y, cint z, cint a_max, char *a_out)
{
   strcpy (s_final, "n/a");
   if (strchr (YRPN_PROS , a_scope) == NULL)  return -1;
   return yrpn_addr__adj_main (a_src, a_scope, a_target, b, x, y, z, a_out);
}



/*====================------------------------------------====================*/
/*===----                        token handling                        ----===*/
/*====================------------------------------------====================*/
static void      o___TOKENS__________________o (void) {;};

int          /*--> check for bad addresses ---------------[--------[--------]-*/
yrpn_addr_badref        (int  a_pos)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         x_pos       =    0;     /* updated position in input           */
   int         i           =    0;
   char        x_addr      [LEN_LABEL];
   /*---(header)------------------------*/
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   --rce;  if (s_ready != 'y') {
      DEBUG_YRPN    yLOG_note    ("yRPN address handling functions not configured");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   yrpn_token_error  ();
   --rce;  if (myRPN.lang == YRPN_CBANG) {
      DEBUG_YRPN     yLOG_note    ("skip in c lang mode");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YRPN     yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos <  0) {
      DEBUG_YRPN     yLOG_note    ("start can not be negative");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YRPN     yLOG_value   ("l_working" , myRPN.l_working);
   --rce;  if (a_pos + 3 >= myRPN.l_working) {
      DEBUG_YRPN    yLOG_note    ("not enough room for a bad address");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   ystrlcpy (x_addr, myRPN.working + a_pos, 5);
   DEBUG_YRPN    yLOG_info    ("x_addr"    , x_addr);
   --rce;  if (strcmp (x_addr, "#REF") != 0) {
      DEBUG_YRPN    yLOG_note    ("does not match #REF");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(handle)-------------------------*/
   x_pos = a_pos + 4;
   myRPN.t_type   = YRPN_ADDR   ;
   ystrlcpy (myRPN.t_token, x_addr, LEN_LABEL);
   ystrlcpy (myRPN.t_name , x_addr, LEN_LABEL);
   myRPN.t_len = strlen (myRPN.t_name);
   yrpn_output_infix (myRPN.t_type, myRPN.t_prec, myRPN.t_name, myRPN.t_token, a_pos);
   yrpn_output_rpn   (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
   myRPN.left_oper  = S_OPER_CLEAR;
   /*---(complete)-----------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return x_pos;
}

int          /*--> chec for cell addresses ---------------[--------[--------]-*/
yrpn_addr               (int  a_pos, short a_def)
{  /*---(design notes)--------------------------------------------------------*/
   /* addresses are only lowercase alphanumerics plus $ and @.                */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   char        rc          =    0;     /* generic return code                 */
   int         x_pos       =    0;     /* updated position in input           */
   char        x_addr      [LEN_LABEL];
   char        x_temp      [LEN_LABEL];
   char        x_shunt     [LEN_LABEL];
   char        x_final     [LEN_LABEL];
   int         u, x, y, z;
   /*---(header)------------------------*/
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   --rce;  if (s_ready != 'y') {
      DEBUG_YRPN    yLOG_note    ("yRPN address handling functions not configured");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   yrpn_token_error  ();
   --rce;  if (myRPN.lang == YRPN_CBANG) {
      DEBUG_YRPN     yLOG_note    ("skip in c lang mode");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YRPN     yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos <  0) {
      DEBUG_YRPN     yLOG_note    ("start can not be negative");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN     yLOG_note    ("accumulate characters");
   myRPN.t_type   = YRPN_ADDR   ;
   x_pos        = a_pos;  /* starting point */
   while (yrpn_token_accum (&x_pos) == 0);
   /*---(validate the address)-------------*/
   ystrlcpy (x_addr, myRPN.t_name, LEN_LABEL);
   /*> rc = s_prettier (x_addr, a_def, x_final, YSTR_LEGAL);                          <*/
   rc = s_breaker  (x_addr, &u, &x, &y, &z, NULL, 0, YSTR_CHECK);
   --rce;  if (rc < 0) {
      yrpn_token_error  ();
      DEBUG_YRPN     yLOG_note    ("address not valid");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   rc = s_maker    (u, x, y, z, 0, x_temp , YSTR_CHECK);
   rc = s_prettier (x_temp, a_def, x_shunt, YSTR_CHECK);
   rc = s_prettier (x_addr, a_def, x_final, YSTR_CHECK);
   /*---(handle rpn)---------------------*/
   ystrlcpy (myRPN.t_token, x_shunt, LEN_LABEL);
   ystrlcpy (myRPN.t_name , x_shunt, LEN_LABEL);
   myRPN.t_len = strlen (myRPN.t_name);
   /*> yrpn_output_shuntd_OLD ();                                                     <*/
   yrpn_output_rpn (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
   /*---(handle infix)-------------------*/
   ystrlcpy (myRPN.t_token, x_final, LEN_LABEL);
   ystrlcpy (myRPN.t_name , x_final, LEN_LABEL);
   myRPN.t_len = strlen (myRPN.t_name);
   yrpn_output_infix (myRPN.t_type, myRPN.t_prec, myRPN.t_name, myRPN.t_token, a_pos);
   myRPN.left_oper  = S_OPER_CLEAR;
   /*---(complete)-----------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return x_pos;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNIT_TEST_______________o (void) {;};

char
yrpn_addr__uinsider     (int b, int x, int y, int z)
{
   /*   6i8..6m12   */
   if (b != 6)  return 0;
   if (x <  7)  return 0;
   if (x > 11)  return 0;
   if (y <  7)  return 0;
   if (y > 11)  return 0;
   return 1;
}

char*      /* ---- : answer unit testing gray-box questions ------------------*/
yrpn_addr__unit         (char *a_question, int a_item)
{
   /*---(initialize)---------------------*/
   ystrlcpy (unit_answer, "ADDR status      : unknown request", 100);
   /*---(input)--------------------------*/
   if          (strcmp (a_question, "status"    )     == 0) {
      snprintf (unit_answer, LEN_RECD, "ADDR status      : %c" , s_ready);
   }
   else if     (strcmp (a_question, "adjusted"  )     == 0) {
      snprintf (unit_answer, LEN_RECD, "yRPN adjusted    : %2d:%s:" , strlen (s_final), s_final);
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}
