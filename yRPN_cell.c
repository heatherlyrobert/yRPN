/*============================----beg-of-source---============================*/

#include  "yRPN.h"
#include  "yRPN_priv.h"


static int  v_tab       = 0;
static int  v_col       = 0;
static int  v_row       = 0;
static int  v_abs       = 0;

static char s_addr      [20];
static int  s_pos       = 0;
static int  s_tab       = 0;
static int  s_col       = 0;
static int  s_row       = 0;
static int  s_abs       = 0;
static int  s_max       = 0;


extern char    (*g_validator )   (char *a_label, char **a_new, int z);
extern char    (*g_adjuster  )   (char *a_label, char **a_new, char a_scope, int x, int y, int z);        /* pass deproot->owner, get back label of thing    */

char
yRPN_addr_config        (char *a_chars, void *a_validator, void *a_adjuster)
{
}

/*====================------------------------------------====================*/
/*===----                        helper functions                      ----===*/
/*====================------------------------------------====================*/
static void        o___HELPERS_________________o (void) {;}


char         /*--> prepare for cell analysis -------------[-leaf---[--------]-*/
yRPN_cell_init       (char *a_label, short *a_pos, short *a_tab, short *a_col, short *a_row, char *a_abs, char *a_max)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   int         x_len       =    0;
   int         i           =    0;
   /*---(defense)------------------------*/
   --rce;  if (a_label == NULL)    return rce;
   --rce;  if (a_pos   == NULL)    return rce;
   --rce;  if (a_tab   == NULL)    return rce;
   --rce;  if (a_col   == NULL)    return rce;
   --rce;  if (a_row   == NULL)    return rce;
   --rce;  if (a_abs   == NULL)    return rce;
   --rce;  if (a_max   == NULL)    return rce;
   /*---(initialize)---------------------*/
   strlcpy (s_addr, "", S_LEN_LABEL);
   *a_pos = s_pos = 0;
   *a_tab = s_tab =-1;
   *a_col = s_col =-1;
   *a_row = s_row =-1;
   *a_abs = s_abs = 0;
   *a_max = s_max = 0;
   /*---(check label)--------------------*/
   x_len = strlen (a_label);
   --rce;  if (x_len  <   2)       return rce;     /* a1                      */
   --rce;  if (x_len  >  10)       return rce;     /* $0$ab$1234              */
   --rce;  for (i = 0; i < x_len; ++i) {
      if (strchr (v_address, a_label [i]) == 0)   return rce;
   }
   strlcpy (s_addr, a_label, S_LEN_LABEL);
   *a_max = s_max = x_len;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> interpret tab in cell address ---------[-leaf---[--------]-*/
yRPN_cell_tab        (char *a_label, short *a_pos, short *a_tab, char *a_abs, char  a_max, short a_ctab)
{  /*---(design notes)--------------------------------------------------------*/
   /* tabs can only be 0-9.  i am keeping this simple with rational limits    */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   int         i           =    0;
   /*---(defense)------------------------*/
   --rce;  if (a_label == NULL)    return rce;
   --rce;  if (a_pos   == NULL)    return rce;
   --rce;  if (a_tab   == NULL)    return rce;
   --rce;  if (a_abs   == NULL)    return rce;
   --rce;  if (a_max   == NULL)    return rce;
   --rce;  if (*a_pos  <  0)       return rce;
   --rce;  if (*a_pos  >= a_max)   return rce;
   /*> printf ("   t :     : l=%-15.15s, t=%4d,       ,       , a=%2d, p=%2d, m=%2d\n", a_label, *a_tab, *a_abs, *a_pos, a_max);   <*/
   /*---(absolute marker)----------------*/
   if        (a_label [*a_pos] == S_CHAR_WORM ) {
      *a_abs  = 7;
      ++*a_pos;
   } else if (a_label [*a_pos] == S_CHAR_GREED) {
      *a_abs  = 4;
      ++*a_pos;
   }
   /*---(parse tab reference)------------*/
   if (strchr (v_number, a_label [*a_pos]) != NULL) {
      *a_tab  = a_label [*a_pos] - S_CHAR_ZERO;
      ++*a_pos;
   } else if (strchr (v_upper, a_label [*a_pos]) != NULL) {
      *a_tab  = a_label [*a_pos] - 'A' + 10;
      ++*a_pos;
   }
   /*---(catch problem)------------------*/
   if (*a_tab <  0)  {
      *a_tab = a_ctab;  /* blank tab means current tab  */
      if (*a_abs == 4) {
         *a_abs = 0;    /* this was really the col marker */
         *a_pos = 0;
      }
   }
   /*---(save into debugging vars)-------*/
   s_pos  = *a_pos;
   s_tab  = *a_tab;
   s_abs  = *a_abs;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> interpret col in cell address ---------[-leaf---[--------]-*/
yRPN_cell_col        (char *a_label, short *a_pos, short *a_col, char *a_abs, char  a_max)
{  /*---(design notes)--------------------------------------------------------*/
   /* cols can be 1-2 lower case chars -- rational limits.                    */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   int         i           =    0;
   /*---(defense)------------------------*/
   --rce;  if (a_label == NULL)    return rce;
   --rce;  if (a_pos   == NULL)    return rce;
   --rce;  if (a_col   == NULL)    return rce;
   --rce;  if (a_abs   == NULL)    return rce;
   --rce;  if (a_max   == NULL)    return rce;
   --rce;  if (*a_pos  <  0)       return rce;
   --rce;  if (*a_pos  >= a_max)   return rce;
   /*---(absolute marker)----------------*/
   if (a_label [*a_pos] == S_CHAR_GREED) {
      if (*a_abs < 7)  *a_abs += 2;
      ++*a_pos;
   }
   /*---(parse col reference)------------*/
   for (i = 0; i < 2; ++i) {
      if (a_label [*a_pos] == '\0')                        break;
      if (strchr (v_lower, a_label [*a_pos]) == NULL)      break;
      if (*a_col >= 0)  *a_col  = (*a_col + 1) * 26;
      else              *a_col  = 0;
      *a_col += a_label [*a_pos] - 'a';
      ++*a_pos;
   }
   /*---(catch problem)------------------*/
   --rce;  if (*a_col <  0)        return rce;
   /*---(save into debugging vars)-------*/
   s_pos  = *a_pos;
   s_col  = *a_col;
   s_abs  = *a_abs;
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> interpret row in cell address ---------[-leaf---[--------]-*/
yRPN_cell_row        (char *a_label, short *a_pos, short *a_row, char *a_abs, char  a_max)
{  /*---(design notes)--------------------------------------------------------*/
   /* rows can be 1-4 digits or 0-9998 rows -- rational limits.               */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   int         i           =    0;
   /*---(header)-------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   /*---(defense)------------------------*/
   --rce;  if (a_label == NULL)    return rce;
   --rce;  if (a_pos   == NULL)    return rce;
   --rce;  if (a_row   == NULL)    return rce;
   --rce;  if (a_abs   == NULL)    return rce;
   --rce;  if (a_max   == NULL)    return rce;
   --rce;  if (*a_pos  <  0)       return rce;
   --rce;  if (*a_pos  >= a_max)   return rce;
   /*---(absolute marker)----------------*/
   if (a_label [*a_pos] == S_CHAR_GREED) {
      if (*a_abs < 7)  ++*a_abs;
      ++*a_pos;
   }
   /*---(parse col reference)------------*/
   for (i = 0; i < 5; ++i) {
      if (a_label [*a_pos] == '\0')                        break;
      if (strchr (v_number, a_label [*a_pos]) == NULL)     break;
      if (*a_row >= 0)   *a_row *= 10;
      else               *a_row  = 0;
      *a_row += a_label [*a_pos] - S_CHAR_ZERO;
      ++*a_pos;
   }
   --*a_row;
   /*---(catch problem)------------------*/
   --rce;  if (*a_row < 0)         return rce;
   --rce;  if (*a_row > 9999)      return rce;
   --rce;  if (*a_pos < a_max)     return rce;
   --rce;  if (*a_pos > a_max)     return rce;
   /*---(save into debugging vars)-------*/
   s_pos  = *a_pos;
   s_row  = *a_row;
   s_abs  = *a_abs;
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> turn components into a nice address ---[-leaf---[--------]-*/
yRPN_cell_pretty     (short a_tab, short a_col, short a_row, char a_abs, char *a_pretty)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;
   char        x_addr      [S_LEN_LABEL];
   char        x_temp      [S_LEN_LABEL];
   /*---(start empty)--------------------*/
   strlcpy (x_addr, "", S_LEN_LABEL);
   strlcpy (a_pretty, x_addr, S_LEN_LABEL);
   strlcpy (s_addr  , x_addr, S_LEN_LABEL);
   s_max = strlen (x_addr);
   /*---(defense)------------------------*/
   --rce;  if (a_tab    <  0   )         return rce;
   --rce;  if (a_tab    >  35  )         return rce;
   --rce;  if (a_col    <  0   )         return rce;
   --rce;  if (a_col    >  701 )         return rce;
   --rce;  if (a_row    <  0   )         return rce;
   --rce;  if (a_row    >  9998)         return rce;
   --rce;  if (a_abs    <  0   )         return rce;
   --rce;  if (a_abs    >  7   )         return rce;
   --rce;  if (a_pretty == NULL)         return rce;
   /*---(create pretty address)----------*/
   if (a_abs == 7) {
      strlcat (x_addr, "@", S_LEN_LABEL);
      a_abs -= 7;
   }
   /*---(tab)---------------*/
   if (a_abs >= 4) {
      strlcat (x_addr, "$", S_LEN_LABEL);
      a_abs -= 4;
   }
   if (a_tab <= 9)  sprintf (x_temp, "%d", a_tab);
   else             sprintf (x_temp, "%c", a_tab - 10 + 'A');
   strlcat (x_addr, x_temp, S_LEN_LABEL);
   /*---(col)---------------*/
   if (a_abs >= 2) {
      strlcat (x_addr, "$", S_LEN_LABEL);
      a_abs -= 2;
   }
   if (a_col > 25) {
      sprintf (x_temp, "%c", (a_col / 26) + 'a' - 1);
      strlcat (x_addr, x_temp, S_LEN_LABEL);
   }
   sprintf (x_temp, "%c", (a_col % 26) + 'a');
   strlcat (x_addr, x_temp, S_LEN_LABEL);
   /*---(col)---------------*/
   if (a_abs >= 1) {
      strlcat (x_addr, "$", S_LEN_LABEL);
   }
   sprintf (x_temp, "%d", a_row + 1);
   strlcat (x_addr, x_temp, S_LEN_LABEL);
   /*---(save result)--------------------*/
   strlcpy (a_pretty, x_addr, S_LEN_LABEL);
   strlcpy (s_addr  , x_addr, S_LEN_LABEL);
   s_max = strlen (x_addr);
   /*---(complete)-----------------------*/
   return 0;
}

char         /*--> validate and clean cell addresses -----[--------[--------]-*/
yRPN_cell         (char *a_label, short a_ctab)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =    0;
   short       x_pos       =    0;
   short       x_tab       =    0;
   short       x_col       =    0;
   short       x_row       =    0;
   char        x_abs       =    0;
   char        x_max       =    0;
   /*---(process)------------------------*/
   /*> printf ("rc < : %3d : l=%-15.15s\n", rc, a_label);                             <*/
   if (rc >= 0)  rc = yRPN_cell_init   (a_label, &x_pos, &x_tab, &x_col, &x_row, &x_abs, &x_max);
   /*> printf ("rc i : %3d : l=%-15.15s, t=%4d, c=%4d, r=%4d, a=%2d, p=%2d, m=%2d\n", rc, a_label, x_tab, x_col, x_row, x_abs, x_pos, x_max);   <*/
   if (rc >= 0)  rc = yRPN_cell_tab    (a_label, &x_pos, &x_tab, &x_abs, x_max, a_ctab);
   /*> printf ("rc t : %3d : l=%-15.15s, t=%4d, c=%4d, r=%4d, a=%2d, p=%2d, m=%2d\n", rc, a_label, x_tab, x_col, x_row, x_abs, x_pos, x_max);   <*/
   if (rc >= 0)  rc = yRPN_cell_col    (a_label, &x_pos, &x_col, &x_abs, x_max);
   /*> printf ("rc c : %3d : l=%-15.15s, t=%4d, c=%4d, r=%4d, a=%2d, p=%2d, m=%2d\n", rc, a_label, x_tab, x_col, x_row, x_abs, x_pos, x_max);   <*/
   if (rc >= 0)  rc = yRPN_cell_row    (a_label, &x_pos, &x_row, &x_abs, x_max);
   /*> printf ("rc r : %3d : l=%-15.15s, t=%4d, c=%4d, r=%4d, a=%2d, p=%2d, m=%2d\n", rc, a_label, x_tab, x_col, x_row, x_abs, x_pos, x_max);   <*/
   if (rc >= 0)  rc = yRPN_cell_pretty (x_tab, x_col, x_row, x_abs, a_label);
   /*> printf ("rc p : %3d : l=%-15.15s, t=%4d, c=%4d, r=%4d, a=%2d, p=%2d, m=%2d\n", rc, a_label, x_tab, x_col, x_row, x_abs, x_pos, x_max);   <*/
   /*---(worst case)---------------------*/
   if (rc <  0)       yRPN_cell_init   (a_label, &x_pos, &x_tab, &x_col, &x_row, &x_abs, &x_max);
   /*> printf ("rc > : %3d : l=%-15.15s, t=%4d, c=%4d, r=%4d, a=%2d, p=%2d, m=%2d\n", rc, a_label, x_tab, x_col, x_row, x_abs, x_pos, x_max);   <*/
   /*---(complete)-----------------------*/
   return rc;
}



/*====================------------------------------------====================*/
/*===----                     spreadsheet specific                     ----===*/
/*====================------------------------------------====================*/
static void        o___GYGES___________________o (void) {;}

int          /*--> chec for cell addresses ---------------[--------[--------]-*/
yRPN__addresses      (int  a_pos, short a_ctab)
{  /*---(design notes)--------------------------------------------------------*/
   /* addresses are only lowercase alphanumerics plus $ and @.                */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   char        rc          =    0;     /* generic return code                 */
   int         x_pos       =    0;     /* updated position in input           */
   char        x_addr      [S_LEN_LABEL];
   /*---(header)------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yRPN__token_error ();
   --rce;  if (zRPN_lang == S_LANG_CBANG) {
      DEBUG_YRPN    yLOG_note    ("skip in c lang mode");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YRPN    yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos <  0) {
      DEBUG_YRPN    yLOG_note    ("start can not be negative");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN    yLOG_note    ("accumulate characters");
   myRPN.t_type   = S_TTYPE_ADDR;
   x_pos        = a_pos;  /* starting point */
   while (yRPN__token_add (&x_pos) == 0);
   /*---(validate the address)-------------*/
   /*> printf ("%s\n", myRPN.t_name);                                                   <*/
   strlcpy (x_addr, myRPN.t_name, S_LEN_LABEL);
   rc = yRPN_cell (x_addr, a_ctab);
   /*---(handle misses)------------------*/
   --rce;  if (rc < 0) {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("address not valid");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(handle)-------------------------*/
   strlcpy (myRPN.t_name, x_addr, S_LEN_LABEL);
   myRPN.t_len = strlen (myRPN.t_name);
   yRPN_stack_tokens ();
   yRPN_stack_shuntd ();
   yRPN_stack_normal (a_pos);
   myRPN.left_oper  = S_OPER_CLEAR;
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return x_pos;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

char*        /*--> answer unit test grey-box questions ---[-leaf---[--------]-*/
yRPN_cell_unit       (char *a_question, int a_item)
{
   /*---(spreadsheet)--------------------*/
   if      (strcmp (a_question, "address"   )     == 0) {
      if (s_max == 0)  snprintf (unit_answer, S_LEN_OUTPUT, "cell address     : (%2d)"                             , s_max);
      else             snprintf (unit_answer, S_LEN_OUTPUT, "cell address     : (%2d) %s"                          , s_max, s_addr);
   }
   else if (strcmp (a_question, "break-out" )     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "cell break-out   : tab=%4d, col=%4d, row=%4d, abs=%1d", s_tab, s_col, s_row, s_abs);
   }
   else if (strcmp (a_question, "position"  )     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "cell position    : %d"                                , s_pos);
   }
   /*---(UNKNOWN)------------------------*/
   else {
      snprintf(unit_answer, S_LEN_OUTPUT, "UNKNOWN          : question is not understood");
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}



/*============================----end-of-source---============================*/
