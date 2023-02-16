/*============================----beg-of-source---============================*/
#include  "yRPN.h"
#include  "yRPN_priv.h"



char
yrpn_exact__by_index    (char a_work [LEN_RECD], char a_index, char *r_type, char r_content [LEN_FULL], short *r_pos)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         rc          =    0;
   int         x_len       =    0;
   int         x_pos       =    0;
   int         i           =    0;
   char       *b           =    0;
   short       l           =    0;
   char       *p           = NULL;
   char        n           =    0;
   char        x_content   [LEN_FULL]  = "";
   /*---(header)------------------------*/
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(default)------------------------*/
   if (r_type    != NULL)  *r_type  = '-';
   if (r_content != NULL)  strlcpy (r_content, "", LEN_SHORT);
   if (r_pos     != NULL)  *r_pos   = -1;
   /*---(defenses)-----------------------*/
   DEBUG_YRPN     yLOG_point   ("a_work"    , a_work);
   --rce;  if (a_work    == NULL) {
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YRPN     yLOG_value   ("a_index"   , a_index);
   DEBUG_YRPN     yLOG_point   ("r_type"    , r_type);
   DEBUG_YRPN     yLOG_point   ("r_content" , r_content);
   /*---(prepare)------------------------*/
   x_len = strlen (a_work);
   DEBUG_YRPN     yLOG_value   ("x_len"     , x_len);
   /*---(find breaks)--------------------*/
   for (i = 0; i < x_len; ++i) {
      DEBUG_YRPN     yLOG_complex ("char"      , "%3d, %c", i, a_work [i]);
      if (a_work [i] != 'Œ')  {
         if (i > 0)   continue;
         x_pos = i;
      } else {
         x_pos = i + 1;
         ++n;
      }
      if (n != a_index)  continue;
      b = a_work + x_pos;
      if (a_work [x_pos] != '(')  b += 2;
      DEBUG_YRPN     yLOG_info    ("b"         , b);
      if (b + 3 < l) break;
      p = strchr (b, 'Œ');
      DEBUG_YRPN     yLOG_point   ("p"         , p);
      if (p != NULL) l = p - b;
      else           l = a_work + x_len - b;
      DEBUG_YRPN     yLOG_value   ("l"         , l);
      DEBUG_YRPN     yLOG_char    ("r_type"    , a_work [x_pos]);
      if (r_type    != NULL)  *r_type  = a_work [x_pos];
      snprintf (x_content, LEN_FULL, "%*.*s", l, l, b);
      DEBUG_YRPN     yLOG_info    ("r_content" , x_content);
      if (r_content != NULL)  strlcpy (r_content, x_content, LEN_FULL);
      DEBUG_YRPN     yLOG_value   ("r_pos"     , x_pos);
      if (r_pos     != NULL)  *r_pos   = x_pos;
      DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
      return 0;
   }
   /*---(complete)-----------------------*/
   if (a_index == -1) {
      DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
      return n + 1;
   }
   /*---(complete)-----------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return --rce;
}

char
yrpn_exact__act_prep    (char a_work [LEN_RECD], char a_index, char a_arity, char r_pre [LEN_RECD], char r_suf [LEN_RECD])
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   short       n           =    0;
   short       x_pos       =    0;
   /*---(header)------------------------*/
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(save before)--------------------*/
   n = a_index - a_arity;
   DEBUG_YRPN     yLOG_value   ("n"         , n);
   if (n >= 0) {
      yrpn_exact__by_index (a_work, n, NULL, NULL, &x_pos);
      if (x_pos >= 0)  snprintf (r_pre, LEN_RECD, "%*.*s", x_pos, x_pos, a_work);
   }
   DEBUG_YRPN     yLOG_info    ("r_pre"     , r_pre);
   /*---(save after)---------------------*/
   n = a_index + 1;
   DEBUG_YRPN     yLOG_value   ("n"         , n);
   if (n >= 0) {
      yrpn_exact__by_index (a_work, n, NULL, NULL, &x_pos);
      if (x_pos >= 0)  snprintf (r_suf, LEN_RECD, "%s", a_work + x_pos - 1);
   }
   DEBUG_YRPN     yLOG_info    ("r_suf"     , r_suf);
   /*---(complete)-----------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yrpn_exact__act_get     (char a_work [LEN_RECD], char n, char r_content [LEN_FULL])
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   /*---(header)------------------------*/
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(get one)------------------------*/
   DEBUG_YRPN     yLOG_value   ("n"         , n);
   yrpn_exact__by_index (a_work, n, NULL, r_content, NULL);
   DEBUG_YRPN     yLOG_info    ("r_content" , r_content);
   /*---(complete)-----------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yrpn_exact__act_done    (char a_work [LEN_RECD], char a_pre [LEN_RECD], char a_new [LEN_RECD], char a_suf [LEN_RECD])
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   short       n           =    0;
   short       x_pos       =    0;
   /*---(header)------------------------*/
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(make full)----------------------*/
   snprintf (a_work, LEN_RECD, "%s%s%s", a_pre, a_new, a_suf);
   DEBUG_YRPN     yLOG_info    ("a_work"    , a_work);
   /*---(complete)-----------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yrpn_exact__act_one     (char a_work [LEN_RECD], char a_index, char a_arity, char a_dir)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   short       n           =    0;
   short       x_pos       =    0;
   char        x_pre       [LEN_RECD]  = "";
   char        x_suf       [LEN_RECD]  = "";
   char        x_one       [LEN_FULL]  = "";
   char        x_oper      [LEN_FULL]  = "";
   char        x_new       [LEN_RECD]  = "";
   /*---(header)------------------------*/
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(save ends)----------------------*/
   yrpn_exact__act_prep (a_work, a_index, a_arity, x_pre, x_suf);
   yrpn_exact__act_get  (a_work, a_index - 1, x_one);
   yrpn_exact__act_get  (a_work, a_index    , x_oper);
   /*---(make new)-----------------------*/
   if (a_dir == S_LEFT)  snprintf (x_new, LEN_RECD, "(%s%s)", x_one, x_oper);
   else                  snprintf (x_new, LEN_RECD, "(%s%s)", x_oper, x_one);
   DEBUG_YRPN     yLOG_info    ("x_new"     , x_new);
   /*---(make full)----------------------*/
   yrpn_exact__act_done (a_work, x_pre, x_new, x_suf);
   /*---(complete)-----------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yrpn_exact__act_two     (char a_work [LEN_RECD], char a_index, char a_arity, char a_dir)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   short       n           =    0;
   short       x_pos       =    0;
   char        x_pre       [LEN_RECD]  = "";
   char        x_suf       [LEN_RECD]  = "";
   char        x_one       [LEN_FULL]  = "";
   char        x_two       [LEN_FULL]  = "";
   char        x_oper      [LEN_FULL]  = "";
   char        x_new       [LEN_RECD]  = "";
   /*---(header)------------------------*/
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(save ends)----------------------*/
   yrpn_exact__act_prep (a_work, a_index, a_arity, x_pre, x_suf);
   yrpn_exact__act_get  (a_work, a_index - 2, x_one);
   yrpn_exact__act_get  (a_work, a_index - 1, x_two);
   /*---(get oper)-----------------------*/
   yrpn_exact__act_get  (a_work, a_index    , x_oper);
   if (strcmp (x_oper, "*") == 0)  strcpy (x_oper, "´");
   DEBUG_YRPN     yLOG_info    ("x_oper"    , x_oper);
   /*---(make new)-----------------------*/
   snprintf (x_new, LEN_RECD, "(%s%s%s)", x_one, x_oper, x_two);
   DEBUG_YRPN     yLOG_info    ("x_new"     , x_new);
   /*---(make full)----------------------*/
   yrpn_exact__act_done (a_work, x_pre, x_new, x_suf);
   /*---(complete)-----------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yrpn_exact__act_func    (char a_work [LEN_RECD], char a_index, char a_arity, char a_dir)
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   char        x_pre       [LEN_RECD]  = "";
   char        x_suf       [LEN_RECD]  = "";
   char        x_type      =    0;
   char        c           =    0;
   short       i           =    0;
   char        t           [LEN_FULL]  = "";
   char        x_new       [LEN_RECD]  = "";
   /*---(header)------------------------*/
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(save ends)----------------------*/
   yrpn_exact__act_prep (a_work, a_index, a_arity, x_pre, x_suf);
   /*---(get function)-------------------*/
   yrpn_exact__by_index (a_work, a_index, &x_type, t, NULL);
   DEBUG_YRPN     yLOG_info    ("t (oper)"  , t);
   c = x_type - '0';
   DEBUG_YRPN     yLOG_value   ("c"         , c);
   sprintf (x_new, "(%s(", t);
   /*---(get one)------------------------*/
   for (i = c; i > 0; --i) {
      DEBUG_YRPN     yLOG_value   ("i"         , i);
      yrpn_exact__act_get  (a_work, a_index - i, t);
      DEBUG_YRPN     yLOG_info    ("arg"       , t);
      if (i != c)  strlcat (x_new, ",", LEN_RECD);
      strlcat (x_new, t, LEN_RECD);
   }
   strlcat (x_new, "))", LEN_RECD);
   /*---(make full)----------------------*/
   yrpn_exact__act_done (a_work, x_pre, x_new, x_suf);
   /*---(complete)-----------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yrpn_exact              (char a_before [LEN_RECD])
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         rc          =    0;
   char        w           [LEN_RECD] = "";
   int         l           =    0;
   char        c           =    0;
   char        n           =    0;
   char        x_type      =  '-';
   char        x_content   [LEN_FULL] = "";
   char        x_arity     =    0;
   char        x_dir       =  '-';
   short       x_pos       =  '-';
   /*---(header)------------------------*/
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   DEBUG_YRPN     yLOG_point   ("a_before"  , a_before);
   --rce;  if (a_before == NULL) {
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YRPN     yLOG_info    ("a_before"  , a_before);
   /*---(prepare)------------------------*/
   strlcpy (w, a_before, LEN_RECD);
   l = strlen (w);
   DEBUG_YRPN     yLOG_value   ("l"         , l);
   c = yrpn_exact__by_index (w, -1, NULL, NULL, NULL);
   DEBUG_YRPN     yLOG_value   ("c"         , c);
   /*---(run operators)------------------*/
   for (n = 0; n < c; ++n) {
      /*---(filter)----------------------*/
      yrpn_exact__by_index (w, n, &x_type, x_content, &x_pos);
      if (x_type == YRPN_OPER) {
         /*---(update)----------------------*/
         x_arity = yRPN_arity (x_content, &x_dir);
         switch (x_arity) {
         case  1 :  yrpn_exact__act_one (w, n, 1, x_dir);  break;
         case  2 :  yrpn_exact__act_two (w, n, 2, x_dir);  break;
         default :  continue;                              break;
         }
      }
      /*---(function)--------------------*/
      else if (strchr (YSTR_NUMBER, x_type) != NULL) {
         c = x_type - '0';
         yrpn_exact__act_func (w, n, c, S_LEFT);
      }
      /*---(nothing to do)---------------*/
      else {
         continue;
      }
      /*---(next)------------------------*/
      c = yrpn_exact__by_index (w, -1, NULL, NULL, NULL);
      DEBUG_YRPN     yLOG_value   ("c"         , c);
      if (c < 0) break;
      n = 0;
      /*---(done)------------------------*/
   }
   l = strlen (w);
   snprintf (myRPN.exact, LEN_RECD, "%*.*s", l - 2, l - 2, w + 1);
   /*---(complete)-----------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return 0;
}

