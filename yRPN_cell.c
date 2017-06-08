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
yRPN_cell_init      (char *a_label, short *a_pos, short *a_tab, short *a_col, short *a_row, char *a_abs, char *a_max)
{
   /*---(locals)-----------------------------*/
   char        rce     =  -10;
   int         x_len   =  -10;
   int         i       =    0;
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

char       /* ---- : interpret cell address ----------------------------------*/
yRPN_cell_tab       (char *a_label, int *a_pos, int *a_tab, char *a_abs, char  a_max)
{
   /*---(locals)-----------------------------*/
   char        rce     =  -10;
   int         i       =    0;
   /*---(defense)------------------------*/
   --rce;  if (*a_pos     <  0)                            return rce;
   --rce;  if (*a_pos     >= a_max)                        return rce;
   /*---(absolute marker)----------------*/
   if (a_label [*a_pos] == S_CHAR_WORM ) {
      *a_abs += 7;
      ++*a_pos;
   }
   if (a_label [*a_pos] == S_CHAR_GREED) {
      *a_abs += 4;
      ++*a_pos;
   }
   /*---(parse tab reference)------------*/
   --rce;
   for (i = 0; i < 2; ++i) {
      if (a_label [*a_pos] == '\0')                        return rce;
      if (strchr("0123456789", a_label [*a_pos]) == 0)     break;
      if (*a_tab >= 0)  *a_tab *= 10;
      *a_tab += a_label [*a_pos] - S_CHAR_ZERO;
      ++*a_pos;
   }
   /*---(save into debugging vars)-------*/
   v_abs  = *a_abs;
   v_tab  = *a_tab;
   /*---(complete)-----------------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                     spreadsheet specific                     ----===*/
/*====================------------------------------------====================*/
static void        o___GYGES___________________o (void) {;}

char       /* ---- : interpret cell address ----------------------------------*/
yRPN__cells       (char *a_label, int *a_tab, int *a_col, int *a_row, char *a_abs)
{
   /*---(locals)-----------------------------*/
   int         x_len   = 0;
   int         i       = 0;
   char        col1    = ' ';
   char        col2    = ' ';
   int         s_tab   = 0;
   int         x_tab   = 0;
   int         s_col   = 0;
   int         x_col   = 0;
   int         s_row   = 0;
   int         x_row   = 0;
   int         e_row   = 0;
   char        x_abs   = 0;
   char        rce         = -10;                /* return code for errors    */
   char        x_temp  [50]  = "";
   /*---(prepare)------------------------*/
   v_tab = v_col = v_row = v_abs = -1;
   strcpy (s_addr, "");
   /*---(check sizes)--------------------*/
   x_len = strlen (a_label);
   --rce;  if (x_len  <   2)           return rce;
   --rce;  if (x_len  >  12)           return rce;     /* a1 to $14$ab$12345  */
   /*---(look for initial markers)-------*/
   if (a_label[s_tab] == '@') {                      
      x_abs  = 7;
      ++s_tab;
   }
   if (a_label[s_tab] == '$') {                      
      x_abs += 4;
      ++s_tab;
   }
   /*---(parse tab reference)------------*/
   s_col = s_tab;
   for (i = s_tab; i < s_tab + 2; ++i) {
      if (strchr("0123456789", a_label[i]) == 0) break;
      if (i >  s_tab)  x_tab *= 10;
      x_tab += a_label[i] - '0';
      ++s_col;
   }
   if (s_col == s_tab && x_abs == 4)  x_abs = 2;
   if (a_tab != NULL)  *a_tab = x_tab;
   v_tab = x_tab;
   /*---(look for absolute column)-------*/
   if (a_label [s_col] == '$') {
      x_abs += 2;
      ++s_col;
   }
   /*---(parse col characters)-----------*/
   s_row = s_col;
   for (i = s_col; i < s_col + 2; ++i) {
      if (strchr ("abcdefghijklmnopqrstuvwxyz", a_label[i]) == 0)   break;
      if (i >  s_col)  x_col *= 26;
      x_col += a_label[i] - 'a' + 1;
      ++s_row;
   }
   --rce;  if (s_row == s_col)         return rce;
   --x_col;
   if (a_col != NULL)  *a_col = x_col;
   v_col = x_col;
   /*---(look for absolute row)----------*/
   if (a_label [s_row] == '$') {
      x_abs += 1;
      ++s_row;
   }
   if (x_abs > 7    )   x_abs = 7;
   if (a_abs != NULL)  *a_abs = x_abs;
   v_abs = x_abs;
   /*---(parse row characters)-----------*/
   e_row = s_row;
   for (i = s_row; i < x_len; ++i) {
      if (strchr ("0123456789", a_label[i]) == 0)   break;
      if (i >  s_row)  x_row *= 10;
      x_row += a_label[i] - '0';
      ++e_row;
   }
   --rce;  if (x_row > 9999)    return rce;
   --rce;  if (s_row == e_row)  return rce;
   --rce;  if (e_row != x_len)  return rce;
   --x_row;
   if (a_row != NULL)  *a_row = x_row;
   v_row = x_row;
   /*---(create pretty address)------------*/
   if (x_abs == 7) {
      strcat (s_addr, "@");
      x_abs -= 7;
   }
   /*---(tab)---------------*/
   if (x_abs >= 4) {
      strcat (s_addr, "$");
      x_abs -= 4;
   }
   sprintf (x_temp, "%d", v_tab);
   strcat  (s_addr, x_temp);
   /*---(col)---------------*/
   if (x_abs >= 2) {
      strcat (s_addr, "$");
      x_abs -= 2;
   }
   if (v_col > 26) {
      sprintf (x_temp, "%c", (v_col / 26) + 'a' - 1);
      strcat  (s_addr, x_temp);
   }
   sprintf (x_temp, "%c", (v_col % 26) + 'a');
   strcat  (s_addr, x_temp);
   /*---(col)---------------*/
   if (x_abs >= 1) {
      strcat (s_addr, "$");
   }
   sprintf (x_temp, "%d", v_row + 1);
   strcat  (s_addr, x_temp);
   /*---(complete)-------------------------*/
   return 0;
}

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
   rc = yRPN__cells (rpn.t_name, NULL, NULL, NULL, NULL);
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
