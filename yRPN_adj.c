/*============================----beg-of-source---============================*/
#include  "yRPN.h"
#include  "yRPN_priv.h"




/*---[[ globally shared ]]--------------------------------*/
char     *g_alpha     = "$abcdefghijklmnopqrstuvwxyz";
char     *g_rcnum     = "-0123456789";
char     *g_rcops     = "|&=!<>*/%+-():;";




/*---[[ private variables ]]------------------------------*/
static char   s_work   [S_LEN_OUTPUT];
static char   s_final  [S_LEN_OUTPUT];

static int    s_txpos  = 0;    /* target col                                  */
static int    s_typos  = 0;    /* target row                                  */
static int    s_tzpos  = 0;    /* target tab                                  */

static int    s_axpos  = 0;    /* adjust col                                  */
static int    s_aypos  = 0;    /* adjust row                                  */
static int    s_azpos  = 0;    /* adjust tab                                  */



/*====================------------------------------------====================*/
/*===----                     formula modification                     ----===*/
/*====================------------------------------------====================*/
void  o___MODIFY__________o () { return; }

char         /*-> shared argument validiation --------[ ------ [fe.G67.55#.92]*/ /*-[01.0000.018.!]-*/ /*-[--.---.---.--]-*/
yRPN__adj_check       (char **a_source, char a_scope, char *a_target)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;                /* return code for errors    */
   char        rc          =    0;                  /* generic return code       */
   /*---(begin)--------------------------*/
   DEBUG_RPN    yLOG_enter   (__FUNCTION__);
   /*---(rpn)----------------------------*/
   DEBUG_RPN    yLOG_point   ("a_source"  , a_source);
   --rce;  if (a_source  == NULL)  {
      DEBUG_RPN    yLOG_note    ("aborted, a_source is NULL, no point");
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_RPN    yLOG_point   ("*a_source" , *a_source);
   --rce;  if (*a_source == NULL)  {
      DEBUG_RPN    yLOG_note    ("aborted, *a_source is NULL, no content");
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_RPN    yLOG_info    ("*a_source" , *a_source);
   /*---(scope)--------------------------*/
   DEBUG_RPN    yLOG_char    ("a_scope"   , a_scope);
   DEBUG_RPN    yLOG_info    ("valid"     , YRPN_ALL);
   --rce;  if (strchr (YRPN_ALL, a_scope) == 0)   {
      DEBUG_RPN    yLOG_note    ("aborted, a_scope not legal");
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(target)-------------------------*/
   DEBUG_RPN    yLOG_point   ("a_target"  , a_target);
   s_txpos = s_typos = s_tzpos = -1;
   --rce;  if (a_target  != NULL)  {
      DEBUG_RPN    yLOG_info    ("a_target"  , a_target);
      rc = str2gyges (a_target, &s_txpos, &s_typos, &s_txpos, NULL, 0);
      DEBUG_SORT    yLOG_value   ("rc"        , rc);
      if (rc < 0)  {
         DEBUG_SORT    yLOG_note    ("could not parse, EXITING");
         DEBUG_SORT    yLOG_exit    (__FUNCTION__);
         return rce;
      }
   }
   DEBUG_RPN    yLOG_complex ("target"    , "x=%4d, y=%4d, z=%4d", s_txpos, s_typos, s_tzpos);
   /*---(save)---------------------------*/
   strlcpy (s_work , *a_source, S_LEN_OUTPUT);
   strlcpy (s_final, "n/a"    , S_LEN_OUTPUT);
   /*---(complete)-----------------------*/
   DEBUG_RPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> change a specific reference --------[ ------ [fe.HA4.498.A3]*/ /*-[02.0000.018.!]-*/ /*-[--.---.---.--]-*/
yRPN__adj_one      (char *a_old, char a_scope, char *a_new)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;               /* generic return code       */
   int         z           =    0;               /* tab of new address        */
   int         x           =    0;               /* col of new address        */
   int         y           =    0;               /* row of new address        */
   char        x_abs       =    0;               /* abs of new address        */
   char        x_addr      [20]        = "";     /* new address               */
   char        x_nabs      =    0;               /* technical abs of address  */
   int         x_bad       =    0;
   /*---(begin)--------------------------*/
   DEBUG_RPN    yLOG_enter   (__FUNCTION__);
   DEBUG_RPN    yLOG_info    ("a_old"     , a_old);
   DEBUG_RPN    yLOG_char    ("a_scope"   , a_scope);
   /*---(prepare)---------------------*/
   strcpy (a_new, a_old);
   /*---(check for do-nothing)--------*/
   --rce;  if (strchr (YRPN_ALL, a_scope) == NULL) {
      DEBUG_RPN    yLOG_note    ("scope indicates nothing to do");
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(parse for address)-----------*/
   rc = str2gyges (a_old, &x, &y, &z, &x_abs, 0);
   DEBUG_RPN    yLOG_value   ("str2gyges" , rc);
   --rce;  if (rc <  0) {
      DEBUG_RPN    yLOG_note    ("not an address, just append");
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   x_nabs = x_abs;
   /*---(check targeted refs)---------*/
   --rce;  if (strchr (YRPN_PROS, a_scope) != NULL) {
      if (s_tzpos != z || s_txpos != x || s_typos != y) {
         DEBUG_RPN    yLOG_note    ("tab, col, or row does not match target, just append");
         DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(adjust to scopes)------------*/
   rc = yVIKEYS_regs_inside (x, y, z);
   --rce;  if (strchr (YRPN_RINSIDE, a_scope) != NULL) {
      if (a_scope == YRPN_RINNER && rc <= 0) {
         DEBUG_RPN    yLOG_note    ("cell label not inner register area");
         DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      if (rc > 0)  x_nabs = 0;
   }
   if (a_scope == YRPN_PALL)    x_nabs = 0;
   /*---(handle addresses)------------*/
   DEBUG_RPN    yLOG_value   ("x_nabs"    , x_nabs);
   switch (x_nabs) {
   case  0  : z += s_azpos; x += s_axpos; y += s_aypos;  break;
   case  1  : z += s_azpos;               y += s_aypos;  break;
   case  2  : z += s_azpos; x += s_axpos;                break;
   case  3  : z += s_azpos;                              break;
   case  4  :               x += s_axpos; y += s_aypos;  break;
   case  5  :                             y += s_aypos;  break;
   case  6  :               x += s_axpos;                break;
   default  : break;
   }
   /*---(form new address)---------*/
   rc = str4gyges   (x, y, z, x_abs, x_addr);
   DEBUG_RPN    yLOG_value   ("str4gyges" , rc);
   if (rc != 0) {
      strcpy (a_new, "#REF");
      DEBUG_RPN    yLOG_note    ("#REF, address cant adjust that far");
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_RPN    yLOG_info    ("new label" , x_addr);
   strcpy (a_new, x_addr);
   /*---(complete)-----------------------*/
   DEBUG_RPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char         /*-> change a specific reference --------[ ------ [fe.J75.197.84]*/ /*-[02.0000.037.!]-*/ /*-[--.---.---.--]-*/
yRPN__adj_main     (char **a_source, char a_scope, int x, int y, int z, char *a_target)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;               /* return code for errors    */
   char        rc          =    0;               /* generic return code       */
   char        x_tokens    [S_LEN_OUTPUT] = "";  /* source made into tokens   */
   char        x_final     [S_LEN_OUTPUT] = "";  /* new version of formula    */
   char       *p           = NULL;               /* strtok field pointer      */
   char       *q           =  " ";               /* strtok delimiters         */
   char       *r           = NULL;               /* strtok context            */
   char        x_new       [S_LEN_OUTPUT] = "";  /* new element               */
   int         x_bad       =    0;
   /*---(begin)--------------------------*/
   DEBUG_RPN    yLOG_enter   (__FUNCTION__);
   /*---(defense)------------------------*/
   rc = yRPN__adj_check (a_source, a_scope, a_target);
   DEBUG_RPN    yLOG_value   ("check"     , rc);
   --rce;  if (rc < 0)  {
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(adjustment)---------------------*/
   DEBUG_RPN    yLOG_note    ("save the adjustment");
   s_axpos = x;
   s_aypos = y;
   s_azpos = z;
   /*---(prepare tokens)-----------------*/
   rc = yRPN_tokens (s_work, &x_tokens, NULL, 2000);
   DEBUG_RPN    yLOG_value   ("tokenize"  , rc);
   --rce;  if (rc < 0)  {
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_RPN    yLOG_info    ("x_tokens"  , x_tokens);
   /*---(parse first token)--------------*/
   p = strtok_r (x_tokens, q, &r);
   DEBUG_RPN    yLOG_point   ("p"         , p);
   if (p == NULL) {
      DEBUG_RPN    yLOG_note    ("aborted, very first parse returned NULL");
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(loop through tokens)------------*/
   while (p != NULL) {
      rc = yRPN__adj_one (p, a_scope, x_new);
      strcat (x_final, x_new);
      strcat (x_final, " ");
      DEBUG_RPN    yLOG_info    ("x_final"   , x_final);
      if (strcmp (x_new, "#REF") == 0)  ++x_bad;
      p = strtok_r (NULL    , q, &r);
   }
   /*---(strip final space)--------------*/
   x_final [strlen (x_final) - 1] = '\0';
   /*---(wrap-up)------------------------*/
   DEBUG_RPN    yLOG_info    ("final"     , x_final);
   strcpy (s_final, x_final);
   if (*a_source != NULL)  strcpy (*a_source, x_final);
   /*---(check for ref troubles)---------*/
   DEBUG_RPN    yLOG_value   ("x_bad"     , x_bad);
   --rce;  if (x_bad > 0) {
      DEBUG_RPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(complete)-----------------------*/
   DEBUG_RPN    yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yRPN_adjust_norm   (char **a_source, int x, int y, int z, int a_max)
{
   return yRPN__adj_main (a_source, YRPN_RREL , x, y, z, NULL);
}

char
yRPN_adjust_scoped (char **a_source, char a_scope, int x, int y, int z, int a_max)
{
   return yRPN__adj_main (a_source, a_scope , x, y, z, NULL);
}

char
yRPN_adjust_reqs   (char **a_source, char a_scope, int x, int y, int z, int a_max)
{
   strcpy (s_final, "n/a");
   if (strchr (YRPN_REQS  , a_scope) == NULL)  return -1;
   return yRPN__adj_main (a_source, a_scope , x, y, z, NULL);
}

char
yRPN_adjust_pros   (char **a_source, char a_scope, int x, int y, int z, char *a_target, int a_max)
{
   strcpy (s_final, "n/a");
   if (strchr (YRPN_PROS, a_scope) == NULL)  return -1;
   return yRPN__adj_main (a_source, a_scope , x, y, z, a_target);
}
