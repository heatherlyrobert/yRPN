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
yRPN_cell_tab        (char *a_label, short *a_pos, short *a_tab, char *a_abs, char  a_max)
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
   }
   /*---(catch problem)------------------*/
   if (*a_tab <  0)  {
      *a_tab = 0;  /* blank tab means tab 0 */
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
   --rce;  if (a_tab    >  9   )         return rce;
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
   sprintf (x_temp, "%d", a_tab);
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
yRPN_cell         (char *a_label)
{
   /*---(locals)-----------+-----------+-*/
   char        rc          =  -10;
   short       x_pos       =    0;
   short       x_tab       =    0;
   short       x_col       =    0;
   short       x_row       =    0;
   char        x_abs       =    0;
   char        x_max       =    0;
   /*---(process)------------------------*/
   if (rc >= 0)  rc = yRPN_cell_init   (a_label, &x_pos, &x_tab, &x_col, &x_row, &x_abs, &x_max);
   if (rc >= 0)  rc = yRPN_cell_tab    (a_label, &x_pos, &x_tab, &x_abs, &x_max);
   if (rc >= 0)  rc = yRPN_cell_col    (a_label, &x_pos, &x_col, &x_abs, &x_max);
   if (rc >= 0)  rc = yRPN_cell_row    (a_label, &x_pos, &x_row, &x_abs, &x_max);
   if (rc >= 0)  rc = yRPN_cell_pretty (x_tab, x_col, x_row, x_abs, a_label);
   /*---(worst case)---------------------*/
   if (rc <  0)  rc = yRPN_cell_init   (a_label, &x_pos, &x_tab, &x_col, &x_row, &x_abs, &x_max);
   /*---(complete)-----------------------*/
   return rc;
}



/*====================------------------------------------====================*/
/*===----                     spreadsheet specific                     ----===*/
/*====================------------------------------------====================*/
static void        o___GYGES___________________o (void) {;}

int        /* ---- : save off cell addresses ---------------------------------*/
yRPN__addresses    (int  a_pos)
{
   /*---(design notes)-------------------*/
   /*
    *  cell addresses can take on a number of forms...
    *     a1
    *     0a1
    *     $a1, a$1, $a$1, $0a1
    *     b6:b10
    *
    */
   /*---(locals)---------------------------*/
   int       i         = a_pos;
   int       j         = 0;
   int       rc        = 0;
   int       len       = 0;
   /*---(prepare)------------------------*/
   zRPN_DEBUG  printf("entering yRPN_addresses");
   strlcpy (rpn.t_name, YRPN_TOKEN_NULL, S_LEN_TOKEN);
   rpn.t_type = S_TTYPE_ADDR;
   rpn.t_prec = S_PREC_NONE;
   /*---(defenses)-----------------------*/
   zRPN_DEBUG  printf("entering defenses");
   if (zRPN_lang       != S_LANG_GYGES)         return  zRPN_ERR_NOT_SPREADSHEET;
   if (i               >= rpn.l_working)         return  zRPN_ERR_INPUT_NOT_AVAIL;
   if (strchr(v_address, rpn.working[i]) == 0)  return  zRPN_ERR_NOT_ADDRESS_CHAR;
   /*---(main loop)------------------------*/
   zRPN_DEBUG  printf("passed defenses");
   zRPN_DEBUG  printf("   address----------------\n");
   rpn.t_len  = 0;
   while (i < rpn.l_working) {
      /*---(test for right characters)-----*/
      if (strchr(v_address , rpn.working[i]) == 0)  break;
      /*---(normal number)-----------------*/
      rpn.t_name[j]     = rpn.working[i];
      rpn.t_name[j + 1] = '\0';
      ++rpn.t_len ;
      /*---(output)----------------------*/
      zRPN_DEBUG  printf("      %03d (%02d) <<%s>>\n", j, rpn.t_len , rpn.t_name);
      /*---(prepare for next char)-------*/
      ++i;
      ++j;
   }
   /*---(check size)-----------------------*/
   len = strlen(rpn.t_name);
   if (len <= 0) return -1;
   /*---(handle it)------------------------*/
   rc = yRPN_cell (rpn.t_name);
   zRPN_DEBUG  printf("\n");
   if (rc >= 0)  {
      strcpy  (rpn.t_name, s_addr);
   } else if (rc < 0)  {
      strcpy  (rpn.t_name, YRPN_TOKEN_NULL);
      rpn.t_len  = 0;
      return  zRPN_ERR_BAD_ADDRESS;
   }
   yRPN_stack_tokens ();
   yRPN_stack_shuntd();
   yRPN_stack_normal (a_pos);
   /*---(end)------------------------------*/
   zRPN_DEBUG  printf("      fin (%02d) <<%s>>\n", rpn.t_len , rpn.t_name);
   rpn.left_oper  = S_OPER_CLEAR;
   /*---(complete)-------------------------*/
   zRPN_DEBUG  printf("exiting yRPN_addresses");
   return i;
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
