/*============================----beg-of-source---============================*/

#include  "yRPN.h"
#include  "yRPN_priv.h"



char     *v_alphanum  = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
char     *v_alpha     = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";
char     *v_upnum     = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
char     *v_lower     = "abcdefghijklmnopqrstuvwxyz";
char     *v_number    = "0123456789";
char     *v_float     = "0123456789.";
char     *v_hex       = "0123456789abcdefABCDEF";
char     *v_octal     = "o01234567";
char     *v_binary    = "01";
char     *v_group     = "(),[]";   
char     *v_operator  = "|&=!<>*/%+-.?:^~#";
/*> char     *v_preproc   = "#";                                                      <*/
char     *v_preproc   = "";
char     *v_address   = "@$abcdefghijklmnopqrstuvwxyz0123456789";

char      zRPN_olddebug = 'n';

char      zRPN_lang   = S_LANG_C;

char      zRPN_divider [5] = " ";
char      zRPN_divtech [5] = " ";

char    zRPN_ERRORS [100][50] = {
   "input not long enough",
   "starting quote not found",
   "ending quote not found",
   "string literal not long enough",
   "does not start with an alphabetic",
   "does not start with a number",
   "does not start with an operator",
   "zero length token",
   "operator too long",
   "does not start with a grouping character",
   "stack is prematurely empty",
   "can not handle addresses in c-mode",
   "not an address character",
   "badly formatted address",
   "number has multiple decimals",
   "unknown or undefined constant",
};

tRPN      rpn;



/*===[[ OPERATORS ]]==========================================================*/
typedef   struct cOPER tOPER;
struct    cOPER {
   char        name        [5];        /* operator                            */
   char        real;                   /* real/used, internal, or off         */
   char        prec;                   /* percedence                          */
   char        dir;                    /* direction of evaluation             */
   char        arity;                  /* how many arguments it takes         */
};
#define   MAX_OPER    200
tOPER     s_opers [MAX_OPER] = {
   /*-r----prec------name-----dir-arity-*/
   /*---(spreadsheet)------------*/
   { ".."    , 'r', 'd' +  0,  S_LEFT ,   2 },    /* cell range                        */
   /*---(preprocessor)-----------*/
   { "#"     , '-', 'd' +  0,  S_LEFT ,   2 },    /* prefix                            */
   { "##"    , '-', 'd' +  0,  S_LEFT ,   2 },    /* stringification                   */
   /*---(unary/suffix)-----------*/
   { ":+"    , 'I', 'd' +  1,  S_LEFT ,   1 },    /* suffix increment                  */
   { ":-"    , 'I', 'd' +  1,  S_LEFT ,   1 },    /* suffix decrement                  */
   /*---(element of)-------------*/
   { "["     , 'r', 'd' +  1,  S_LEFT ,   1 },    /* array subscripting                */
   { "]"     , 'r', 'd' +  1,  S_LEFT ,   1 },    /* array subscripting                */
   { "."     , 'r', 'd' +  1,  S_LEFT ,   2 },    /* element selection by reference    */
   { "->"    , 'r', 'd' +  1,  S_LEFT ,   2 },    /* element selection thru pointer    */
   /*---(unary/prefix)-----------*/
   { "++"    , 'r', 'd' +  2,  S_RIGHT,   1 },    /* prefix increment                  */
   { "--"    , 'r', 'd' +  2,  S_RIGHT,   1 },    /* prefix decrement                  */
   { "+:"    , 'I', 'd' +  2,  S_RIGHT,   1 },    /* unary plus                        */
   { "-:"    , 'I', 'd' +  2,  S_RIGHT,   1 },    /* unary minus                       */
   { "!"     , 'r', 'd' +  2,  S_RIGHT,   1 },    /* logical NOT                       */
   { "~"     , 'r', 'd' +  2,  S_RIGHT,   1 },    /* bitwise NOT                       */
   { "*:"    , 'I', 'd' +  2,  S_RIGHT,   1 },    /* indirection/dereference           */
   { "&:"    , 'I', 'd' +  2,  S_RIGHT,   1 },    /* address-of                        */
   /*---(multiplicative)---------*/
   { "*"     , 'r', 'd' +  3,  S_LEFT ,   2 },    /* multiplication                    */
   { "/"     , 'r', 'd' +  3,  S_LEFT ,   2 },    /* division                          */
   { "%"     , 'r', 'd' +  3,  S_LEFT ,   2 },    /* modulus                           */
   /*---(additive)---------------*/
   { "+"     , 'r', 'd' +  4,  S_LEFT ,   2 },    /* addition                          */
   { "-"     , 'r', 'd' +  4,  S_LEFT ,   2 },    /* substraction                      */
   { "#"     , 'r', 'd' +  4,  S_LEFT ,   2 },    /* string concatination              */
   { "##"    , 'r', 'd' +  4,  S_LEFT ,   2 },    /* string concatination              */
   /*---(shift)------------------*/
   { "<<"    , 'r', 'd' +  5,  S_LEFT ,   2 },    /* bitwise shift left                */
   { ">>"    , 'r', 'd' +  5,  S_LEFT ,   2 },    /* bitwise shift right               */
   /*---(relational)-------------*/
   { "<"     , 'r', 'd' +  6,  S_LEFT ,   2 },    /* relational lesser                 */
   { "<="    , 'r', 'd' +  6,  S_LEFT ,   2 },    /* relational less or equal          */
   { ">"     , 'r', 'd' +  6,  S_LEFT ,   2 },    /* relational greater                */
   { ">="    , 'r', 'd' +  6,  S_LEFT ,   2 },    /* relational more or equal          */
   { "#<"    , 'r', 'd' +  6,  S_LEFT ,   2 },    /* relational string lesser          */
   { "#>"    , 'r', 'd' +  6,  S_LEFT ,   2 },    /* relational string greater         */
   /*---(equality)---------------*/
   { "=="    , 'r', 'd' +  7,  S_LEFT ,   2 },    /* relational equality               */
   { "!="    , 'r', 'd' +  7,  S_LEFT ,   2 },    /* relational inequality             */
   { "#="    , 'r', 'd' +  7,  S_LEFT ,   2 },    /* relational string equality        */
   { "#!"    , 'r', 'd' +  7,  S_LEFT ,   2 },    /* relational string inequality      */
   /*---(bitwise)----------------*/
   { "&"     , 'r', 'd' +  8,  S_LEFT ,   2 },    /* bitwise AND                       */
   { "^"     , 'r', 'd' +  9,  S_LEFT ,   2 },    /* bitwise XOR                       */
   { "|"     , 'r', 'd' + 10,  S_LEFT ,   2 },    /* bitwise OR                        */
   /*---(logical)----------------*/
   { "&&"    , 'r', 'd' + 11,  S_LEFT ,   2 },    /* logical AND                       */
   { "||"    , 'r', 'd' + 12,  S_LEFT ,   2 },    /* logical OR                        */
   /*---(conditional)------------*/
   { "?"     , 'r', 'd' + 13,  S_RIGHT,   2 },    /* trinary conditional               */
   { ":"     , 'r', 'd' + 13,  S_RIGHT,   2 },    /* trinary conditional               */
   /*---(assignment)-------------*/
   { "="     , 'r', 'd' + 14,  S_RIGHT,   2 },    /* direct assignment                 */
   { "+="    , 'r', 'd' + 14,  S_RIGHT,   2 },
   { "-="    , 'r', 'd' + 14,  S_RIGHT,   2 },
   { "*="    , 'r', 'd' + 14,  S_RIGHT,   2 },
   { "/="    , 'r', 'd' + 14,  S_RIGHT,   2 },
   { "%="    , 'r', 'd' + 14,  S_RIGHT,   2 },
   { "<<="   , 'r', 'd' + 14,  S_RIGHT,   2 },
   { ">>="   , 'r', 'd' + 14,  S_RIGHT,   2 },
   { "&="    , 'r', 'd' + 14,  S_RIGHT,   2 },
   { "^="    , 'r', 'd' + 14,  S_RIGHT,   2 },
   { "|="    , 'r', 'd' + 14,  S_RIGHT,   2 },
   /*---(comma)------------------*/
   { ","     , 'r', 'd' + 15,  S_LEFT ,   2 },    /* comma                             */
   /*---(parenthesis)------------*/
   { "("     , 'r', 'd' + 16,  S_LEFT ,   1 },
   { ")"     , 'r', 'd' + 16,  S_LEFT ,   1 },
   /*---(semicolon)--------------*/
   { ";"     , 'r', 'd' + 17,  S_LEFT ,   1 },
   /*-------------(braces)-------*/
   { "{"     , 'r', 'd' + 18,  S_LEFT ,   1 },
   { "}"     , 'r', 'd' + 18,  S_LEFT ,   1 },
   /*---(end)--------------------*/
   { ""      , '-', '-'     ,  '-'    ,   0 },
};



/*===[[ HIGH-VALUE CONSTANTS ]]===============================================*/
#define   MAX_CONST   200
typedef   struct cCONST tCONST;
struct    cCONST {
   char      name  [30];
   double    value;
};
tCONST    s_consts [MAX_CONST] = {
   /*---(spreadsheet)------------*/
   { "TRUE"                   , 1.0                },
   { "FALSE"                  , 0.0                },
   { "NULL"                   , 0.0                },
   { "PI"                     , 3.1415927          },
   { "DEG2RAD"                , 0.0                },
   { "RAD2DEG"                , 0.0                },
   { ""                       , 0.0                },
};



/*===[[ STANDARD C TYPES ]]===================================================*/
#define   MAX_TYPES   200
typedef   struct cTYPES tTYPES;
struct    cTYPES {
   char      name  [30];
   char      usage;
};
tTYPES    s_types [MAX_TYPES] = {
   /*---(beg)--------------------*/
   { "beg-of-types"           , '-'                },
   /*---(base types)-------------*/
   { "void"                   , 'b'                },
   { "char"                   , 'b'                },
   { "short"                  , 'b'                },
   { "int"                    , 'b'                },
   { "long"                   , 'b'                },
   { "octint"                 , 'b'                },
   { "mini"                   , 'b'                },
   { "half"                   , 'b'                },
   { "float"                  , 'b'                },
   { "double"                 , 'b'                },
   { "quadra"                 , 'b'                },
   /*---(modifiers)--------------*/
   { "signed"                 , 'm'                },
   { "unsigned"               , 'm'                },
   /*---(qualifiers)-------------*/
   { "const"                  , 'm'                },
   { "volitle"                , 'm'                },
   /*---(storage class)----------*/
   { "auto"                   , 'm'                },
   { "static"                 , 'm'                },
   { "register"               , 'm'                },
   { "extern"                 , 'm'                },
   /*---(done)-------------------*/
   { ""                       , '-'                },
};



/*===[[ STANDARD C KEYWORDS ]]================================================*/
#define   MAX_KEYWORDS     200
typedef   struct cKEYWORDS tKEYWORDS;
struct    cKEYWORDS {
   char      name  [30];
   char      usage;
};
tKEYWORDS  s_keywords [MAX_KEYWORDS] = {
   /*---(beg)--------------------*/
   { "beg-of-keywords"        , '-' },
   /*---(preprocessor)-----------*/
   { "include"                , 'p' },
   { "define"                 , 'p' },
   { "unfef"                  , 'p' },
   { "ifdef"                  , 'p' },
   { "ifndef"                 , 'p' },
   { "elif"                   , 'p' },
   { "endif"                  , 'p' },
   { "line"                   , 'p' },
   { "error"                  , 'p' },
   { "pragma"                 , 'p' },
   /*---(storage)----------------*/
   { "struct"                 , 's' },
   { "union"                  , 's' },
   { "typedef"                , 's' },
   { "enum"                   , 's' },
   { "sizeof"                 , 's' },
   /*---(control)----------------*/
   { "break"                  , 'c' },
   { "case"                   , 'c' },
   { "continue"               , 'c' },
   { "default"                , 'c' },
   { "do"                     , 'c' },
   { "else"                   , 'c' },
   { "for"                    , 'c' },
   { "goto"                   , 'c' },
   { "if"                     , 'c' },
   { "return"                 , 'c' },
   { "switch"                 , 'c' },
   { "while"                  , 'c' },
   /*---(reserving)--------------*/
   { "asm"                    , 'r' },
   { "typeof"                 , 'r' },
   /*---(end)--------------------*/
   { ""                       , '-' },
};


int         v_tab       = 0;
int         v_col       = 0;
int         v_row       = 0;
int         v_abs       = 0;


/*====================------------------------------------====================*/
/*===----                           utility                            ----===*/
/*====================------------------------------------====================*/
static void        o___UTILITY_________________o (void) {;}

char*        /*--> return library version information ----[-leaf---[--------]-*/
yRPN_version       (void)
{
   char    t [20] = "";
#if    __TINYC__ > 0
   strncpy (t, "[tcc built]", 15);
#elif  __GNUC__  > 0
   strncpy (t, "[gnu gcc  ]", 15);
#else
   strncpy (t, "[unknown  ]", 15);
#endif
   snprintf (rpn.about, 100, "%s   %s : %s", t, zRPN_VER_NUM, zRPN_VER_TXT);
   return rpn.about;
}

char         /*--> process urgents/debugging -------------[--------[--------]-*/
yRPN_debug         (char *a_urgent)
{
   /*---(defense)------------------------*/
   if (a_urgent == NULL)  return -1;
   /*---(prepare)------------------------*/
   zRPN_debug.tops    = 'n';
   zRPN_debug.stack   = 'n';
   zRPN_debug.cell    = 'n';
   zRPN_debug.oper    = 'n';
   zRPN_debug.keys    = 'n';
   /*---(parse)--------------------------*/
   if (strcmp ("@@yRPN"       , a_urgent) == 0)  zRPN_debug.tops  = 'y';
   if (strcmp ("@@yRPN_oper"  , a_urgent) == 0)  zRPN_debug.tops  = zRPN_debug.oper  = 'y';
   DEBUG_TOPS  yLOG_char    ("zRPN_tops" , zRPN_debug.tops);
   DEBUG_TOPS  yLOG_char    ("zRPN_oper" , zRPN_debug.oper);
   /*---(complete)-----------------------*/
   return 0;
}







char       /* ---- : identify the symbol precedence --------------------------*/
yRPN__precedence   (void)
{
   int       i         = 0;
   for (i = 0; i < MAX_OPER; ++i) {
      if  (strcmp (s_opers[i].name, "end"     ) == 0)  break;
      if  (strcmp (s_opers[i].name, rpn.t_name) != 0)  continue;
      rpn.t_prec  = s_opers[i].prec;
      rpn.t_dir   = s_opers[i].dir;
      rpn.t_arity = s_opers[i].arity;
      return  0;
   }
   /*---(complete)----------------*/
   rpn.t_prec  = S_PREC_FAIL;
   return -1;
}

char       /* ---- : identify the symbol arity -------------------------------*/
yRPN_arity         (char *a_op)
{
   int       i         = 0;
   for (i = 0; i < MAX_OPER; ++i) {
      if  (strcmp (s_opers[i].name, "end"     ) == 0)  break;
      if  (strcmp (s_opers[i].name, a_op      ) != 0)  continue;
      return  s_opers[i].arity;
   }
   /*---(complete)----------------*/
   return -1;
}



/*====================------------------------------------====================*/
/*===----                        normal output                         ----===*/
/*====================------------------------------------====================*/
static void        o___NORMAL__________o () { return; }

char             /* [------] put current item on normal output ---------------*/
yRPN__normal       (int a_pos)
{
   char      x_token [zRPN_MAX_LEN];
   /*> if (rpn.t_type == S_TTYPE_GROUP)  return 0;                                                <*/
   /*> if (rpn.t_type == S_TTYPE_OPER && strcmp(rpn.t_name, ",") == 0)  return 0;                 <*/
   /*> printf ("found a comma, skipping  %c, %c, %s\n", zRPN_lang, rpn.t_type, rpn.t_name);    <* 
    *> if (zRPN_lang == S_LANG_GYGES && rpn.t_type == S_TTYPE_OPER && strcmp(rpn.t_name, ",") == 0)  return 0;   <* 
    *> printf ("just didn't skip\n");                                                       <*/
   sprintf (x_token, "%c,%04d,%s%s", rpn.t_type, a_pos, rpn.t_name, zRPN_divtech);
   strncat (rpn.normal, x_token, zRPN_MAX_LEN);
   ++rpn.l_normal;
   zRPN_DEBUG  printf("      RPN__normal     :: (---) %s\n", x_token);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       stack management                       ----===*/
/*====================------------------------------------====================*/
static void        o___STACK___________o () { return; }

char             /* [------] push current token to stack ---------------------*/
yRPN__push         (void)
{
   snprintf (rpn.stack[rpn.n_stack], zRPN_MAX_LEN, "%c,%c,%s", rpn.t_type, rpn.t_prec, rpn.t_name);
   ++rpn.n_stack;
   zRPN_DEBUG  printf("      RPN__push       :: (%3d) %s\n", rpn.n_stack, rpn.stack [rpn.n_stack - 1]);
   return 0;
}

char             /* [------] look at item on top ot stack --------------------*/
yRPN__peek         (void)
{
   /*---(handle empty stack)-------------*/
   if (rpn.n_stack <= 0) {
      rpn.p_prec  = zRPN_END;
      strncpy (rpn.t_token, YRPN_TOKEN_NULL, 15);
      zRPN_DEBUG  printf("      RPN__peek       :: empty stack\n");
      return zRPN_ERR_EMPTY_STACK;
   }
   /*---(normal stack)-------------------*/
   else {
      rpn.p_type  = rpn.stack[rpn.n_stack - 1][0];
      rpn.p_prec  = rpn.stack[rpn.n_stack - 1][2];
      strncpy (rpn.t_token, rpn.stack[rpn.n_stack - 1] + 4, zRPN_MAX_LEN);
      zRPN_DEBUG  printf("      RPN__peek       :: (%3d) %s\n", rpn.n_stack, rpn.stack [rpn.n_stack - 1]);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char             /* [------] pop and toss the top of the stack ---------------*/
yRPN__toss         (void)
{
   /*---(handle empty stack)-------------*/
   if (rpn.n_stack <= 0) {
      zRPN_DEBUG  printf("      RPN__toss       :: FATAL empty stack\n");
      return zRPN_ERR_EMPTY_STACK;
   }
   /*---(normal stack)-------------------*/
   else {
      zRPN_DEBUG  printf("      RPN__toss       :: (%3d) %s\n", rpn.n_stack, rpn.stack [rpn.n_stack - 1]);
      --rpn.n_stack;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char             /* [------] pop and save the top of the stack ---------------*/
yRPN__pops         (void)
{
   char      xprec;
   char      x_token[S_LEN_TOKEN];
   /*---(handle empty stack)-------------*/
   if (rpn.n_stack <= 0) {
      zRPN_DEBUG  printf("      RPN__pops       :: FATAL empty stack\n");
      return zRPN_ERR_EMPTY_STACK;
   }
   /*---(normal stack)-------------------*/
   else {
      zRPN_DEBUG  printf("      RPN__pops       :: (%3d) %s\n", rpn.n_stack, rpn.stack [rpn.n_stack - 1]);
      --rpn.n_stack;
      ++rpn.n_shuntd;
      sprintf (x_token, "%c,%s%s", rpn.stack[rpn.n_stack][0], rpn.stack[rpn.n_stack] + 4, zRPN_divtech);
      strncat (rpn.detail, x_token, S_LEN_TOKEN);
      sprintf (x_token, "%s%s", rpn.stack[rpn.n_stack] + 4, zRPN_divider);
      strncat (rpn.shuntd, x_token, S_LEN_TOKEN);
      xprec = rpn.stack[rpn.n_stack][2];
   }
   return 0;
}

char             /* [------] save current item to final output ---------------*/
yRPN__save         (void)
{
   char      x_token [S_LEN_TOKEN];
   ++rpn.n_shuntd;
   sprintf (x_token, "%c,%s%s", rpn.t_type, rpn.t_name, zRPN_divtech);
   strncat (rpn.detail, x_token, S_LEN_TOKEN);
   sprintf (x_token, "%s%s", rpn.t_name, zRPN_divider);
   strncat (rpn.shuntd, x_token, S_LEN_TOKEN);
   zRPN_DEBUG  printf("      RPN__save       :: (---) %c,%c,%s\n", rpn.t_type, rpn.t_prec, rpn.t_name);
   return 0;
}

char       /*----: save a token to the tokens output -------------------------*/
yRPN__token        (void)
{
   char          x_div     [S_LEN_LABEL];
   strncat (rpn.tokens, rpn.t_name   , S_LEN_TOKEN);
   strncat (rpn.tokens, zRPN_divider, S_LEN_TOKEN);
   zRPN_DEBUG  printf("      RPN__token      :: (---) %s\n", rpn.t_name);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       token helpers                          ----===*/
/*====================------------------------------------====================*/
static void        o___HELPERS_________________o (void) {;}

char         /*--> set token to error --------------------[--------[--------]-*/
yRPN__token_error    (void)
{
   strncpy (rpn.t_name, YRPN_TOKEN_NULL, S_LEN_TOKEN);
   rpn.t_len    = 0;
   rpn.t_type   = S_TTYPE_ERROR;
   rpn.t_prec   = S_PREC_NONE;
   return 0;
}

char         /*--> add a char to current token -----------[--------[--------]-*/
yRPN__token_add      (int *a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   char        x_ch        =    0;     /* current character                   */
   char        x_esc       =  '-';     /* is the previous char an escape      */
   char        x_bad       =  '-';
   char       *x_norm      = "\"";
   char       *x_ppre      = "\"<";
   char       *x_psuf      = "\">";
   /*---(header)------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   DEBUG_YRPN_M  yLOG_schar   (rpn.t_type);
   DEBUG_YRPN_M  yLOG_sint    (*a_pos);
   DEBUG_YRPN_M  yLOG_sint    (rpn.l_working);
   /*---(defense)-----------------------*/
   --rce;  if (*a_pos >= rpn.l_working) {
      DEBUG_YRPN_M  yLOG_snote   ("past max");
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(get character)-----------------*/
   x_ch    = rpn.working [*a_pos];
   DEBUG_YRPN_M  yLOG_schar   (x_ch);
   if (rpn.t_len > 0 && rpn.t_name [rpn.t_len - 1] == '\\')  x_esc = 'y';
   DEBUG_YRPN_M  yLOG_schar   (x_esc);
   /*---(check character)---------------*/
   --rce;  switch (rpn.t_type) {
   case S_TTYPE_KEYW   : case S_TTYPE_TYPE   :
      DEBUG_YRPN_M  yLOG_snote   ("v_lower");
      if (strchr (v_lower , x_ch) == 0)                         x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_CONST  :
      DEBUG_YRPN_M  yLOG_snote   ("v_upnum");
      if (rpn.t_len == 0 && (x_ch < 'A' || x_ch > 'Z'))         x_bad = 'y';
      if (strchr (v_upnum , x_ch) == 0)                         x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_INT    :
      DEBUG_YRPN_M  yLOG_snote   ("v_number");
      if (strchr (v_number, x_ch) == 0)                         x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_FLOAT  :
      DEBUG_YRPN_M  yLOG_snote   ("v_float");
      if (strchr (v_float , x_ch) == 0)                         x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_BIN    :
      DEBUG_YRPN_M  yLOG_snote   ("v_binary");
      if (rpn.t_len != 1 && strchr (v_binary, x_ch) == 0)       x_bad = 'y';
      if (rpn.t_len == 1 && (x_ch != 'b' && x_ch != 'B'))       x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_OCT    :
      DEBUG_YRPN_M  yLOG_snote   ("v_octal");
      if (rpn.t_len != 1 && strchr (v_octal + 1, x_ch) == 0)    x_bad = 'y';
      if (rpn.t_len == 1 && strchr (v_octal    , x_ch) == 0)    x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_HEX    :
      DEBUG_YRPN_M  yLOG_snote   ("v_hex");
      if (rpn.t_len != 1 && strchr (v_hex   , x_ch) == 0)       x_bad = 'y';
      if (rpn.t_len == 1 && (x_ch != 'x' && x_ch != 'X'))       x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_VARS   : case S_TTYPE_FUNC   :
      DEBUG_YRPN_M  yLOG_snote   ("v_alphanum");
      if (rpn.t_len == 0 && strchr (v_alpha, x_ch) == 0)        x_bad = 'y';
      if (strchr (v_alphanum, x_ch) == 0)                       x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_STR    :
      /*---(check pre-processor)---------*/
      if (rpn.pproc == S_PPROC_INCL) {
         DEBUG_YRPN_M  yLOG_snote   ("include str");
         if (rpn.t_len == 0) {
            if (strchr (x_ppre, x_ch) == 0)                     x_bad = 'y';
         } else {
            if (x_esc != 'y' && strchr (x_psuf, x_ch) != 0)     x_bad = '#';
         }
      }
      /*---(check normal)----------------*/
      else {
         DEBUG_YRPN_M  yLOG_snote   ("normal str");
         if (rpn.t_len == 0) {
            if (strchr (x_norm, x_ch) == 0)                     x_bad = 'y';
         } else {
            if (x_esc != 'y' && strchr (x_norm, x_ch) != 0)     x_bad = '#';
         }
      }
      /*---(done)------------------------*/
      break;
   case S_TTYPE_CHAR   :
      if (rpn.t_len == 0 && x_ch != '\'')                       x_bad = 'y';
      if (x_esc != 'y' && rpn.t_len == 2 && x_ch == '\'')       x_bad = '#';
      if (x_esc == 'y' && rpn.t_len == 3 && x_ch == '\'')       x_bad = '#';
      if (rpn.t_len >= 3)                                       x_bad = 'y';
      break;
      /*---(done)------------------------*/
   case S_TTYPE_OPER   :
      if (rpn.t_len >= 2)                                       x_bad = 'y';
      if (strchr (v_operator , x_ch) == 0)                      x_bad = 'y';
      break;
      /*---(done)------------------------*/
   default             :
      DEBUG_YRPN_M  yLOG_snote   ("illegal type");
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
      /*---(done)------------------------*/
   }
   /*---(stop if bad)-------------------*/
   --rce;  if (x_bad == 'y') {
      DEBUG_YRPN_M  yLOG_snote   ("illegal char");
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(add to token name)-------------*/
   rpn.t_name [rpn.t_len]    = x_ch;
   rpn.t_name [++rpn.t_len]  = '\0';
   DEBUG_YRPN_M  yLOG_snote   (rpn.t_name);
   DEBUG_YRPN_M  yLOG_sint    (rpn.t_len);
   ++(*a_pos);
   DEBUG_YRPN_M  yLOG_sint    (*a_pos);
   /*---(stop if string ended)----------*/
   --rce;  if (x_bad == '#') {
      DEBUG_YRPN_M  yLOG_snote   ("string end");
      DEBUG_YRPN_M  yLOG_sexitr  (__FUNCTION__, rce);
      return rce;
   }
   /*---(prepare for next char)-------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return 0;
}

char         /*--> check what comes next -----------------[--------[--------]-*/
yRPN__token_paren    (int a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        x_ch        =    0;     /* current character                   */
   int         x_pos       =    0;     /* updated position in input           */
   char        x_type      =  S_TTYPE_VARS;
   /*---(skip whitespace)----------------*/
   x_pos = a_pos;
   while (x_pos <  rpn.l_working) {
      x_ch = rpn.working [x_pos];
      if (x_ch != ' ')  break;
      ++x_pos;
   }
   /*---(classify)-----------------------*/
   if (x_ch == '(')  x_type = S_TTYPE_FUNC;
   else              x_type = S_TTYPE_VARS;
   /*---(complete)-----------------------*/
   return x_type;
}

char         /*--> check what comes next -----------------[--------[--------]-*/
yRPN__token_nums     (int a_pos)
{
   /*---(locals)-----------+-----------+-*/
   char        x_ch        =    0;     /* current character                   */
   int         x_pos       =    0;     /* updated position in input           */
   char        x_type      =  S_TTYPE_INT;
   /*---(header)------------------------*/
   DEBUG_YRPN_M  yLOG_senter  (__FUNCTION__);
   DEBUG_YRPN_M  yLOG_sint    (a_pos);
   /*---(check for non-number)-----------*/
   x_pos = a_pos;
   while (x_pos <  rpn.l_working) {
      x_ch = rpn.working [x_pos];
      DEBUG_YRPN_M  yLOG_schar   (x_ch);
      if (strchr (v_number, x_ch) == 0) break;
      ++x_pos;
   }
   DEBUG_YRPN_M  yLOG_sint    (x_pos);
   /*---(classify)-----------------------*/
   if (rpn.working [a_pos] == '0' && x_pos == a_pos + 1) {
      DEBUG_YRPN_M  yLOG_snote   ("special");
      switch (x_ch) {
      case 'x' : case 'X' :
         DEBUG_YRPN_M  yLOG_snote   ("hex");
         x_type = S_TTYPE_HEX;
         break;
      case 'b' : case 'B' :
         DEBUG_YRPN_M  yLOG_snote   ("bin");
         x_type = S_TTYPE_BIN;
         break;
      case 'o' :
         DEBUG_YRPN_M  yLOG_snote   ("oct");
         x_type = S_TTYPE_OCT;
         break;
      case '.' :
         DEBUG_YRPN_M  yLOG_snote   ("float");
         x_type = S_TTYPE_FLOAT;
         break;
      default  :
         DEBUG_YRPN_M  yLOG_snote   ("int1");
         x_type = S_TTYPE_INT;
         break;
      }
   } else if (rpn.working [a_pos] == '0' && x_pos > a_pos + 1) {
      DEBUG_YRPN_M  yLOG_snote   ("oct2");
      x_type = S_TTYPE_OCT;
   } else if (x_ch == '.') {
      DEBUG_YRPN_M  yLOG_snote   ("float2");
      x_type = S_TTYPE_FLOAT;
   } else {
      DEBUG_YRPN_M  yLOG_snote   ("int2");
      x_type = S_TTYPE_INT;
   }
   DEBUG_YRPN_M  yLOG_schar   (x_type);
   /*---(complete)-----------------------*/
   DEBUG_YRPN_M  yLOG_sexit   (__FUNCTION__);
   return x_type;
}

char         /*--> save token to output ------------------[--------[--------]-*/
yRPN__token_save     (char a_pos)
{
   yRPN__token  ();         /* strait to tokens list                          */
   yRPN__save   ();         /* strait to shunted and output lists (no stack)  */
   yRPN__normal (a_pos);    /* strait to normal list                          */
   return 0;
}

char         /*--> push token on stack -------------------[--------[--------]-*/
yRPN__token_push     (char a_pos)
{
   yRPN__token  ();         /* strait to tokens list                          */
   yRPN__push   ();         /* move token to stack                            */
   yRPN__normal (a_pos);    /* strait to normal list                          */
   return 0;
}


/*====================------------------------------------====================*/
/*===----                       c language specific                    ----===*/
/*====================------------------------------------====================*/
static void        o___C_LANG__________________o (void) {;}

int          /*--> check for keyword ---------------------[--------[--------]-*/
yRPN__keywords       (int  a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /* keywords are only lowercase alphanumerics.                              */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   int         x_pos       =    0;     /* updated position in input           */
   int         i           =    0;     /* iterator for keywords               */
   int         x_found     =   -1;     /* index of keyword                    */
   /*---(header)------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yRPN__token_error ();
   --rce;  if (zRPN_lang == S_LANG_GYGES) {
      DEBUG_YRPN    yLOG_note    ("skip in gyges mode");
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
   rpn.t_type   = S_TTYPE_KEYW;
   x_pos        = a_pos;  /* starting point */
   while (yRPN__token_add (&x_pos) == 0);
   /*---(try to match keyword)-------------*/
   DEBUG_YRPN    yLOG_note    ("search keywords");
   for (i = 0; i < MAX_KEYWORDS; ++i) {
      if  (s_keywords [i].name [0] == '\0')                break;
      if  (s_keywords [i].name [0] != rpn.t_name [0])      continue;
      if  (strcmp (s_keywords [i].name, rpn.t_name ) != 0) continue;
      x_found = i;
      DEBUG_YRPN    yLOG_value   ("x_found"   , x_found);
      break;
   }
   DEBUG_YRPN    yLOG_info    ("rpn.t_name", rpn.t_name);
   /*---(handle misses)--------------------*/
   --rce;  if (x_found < 0) {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("keyword not found");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(mark includes)--------------------*/
   DEBUG_YRPN    yLOG_note    ("check for preprocessor keywords");
   if (rpn.pproc == 'y') {
      if (strcmp (rpn.t_name, "include") == 0)  rpn.pproc = S_PPROC_INCL;
      else                                      rpn.pproc = S_PPROC_OTHER;
   }
   /*---(save)-----------------------------*/
   DEBUG_YRPN    yLOG_note    ("put keyword directly to output");
   yRPN__token_save    (a_pos);
   rpn.left_oper = S_OPER_CLEAR;
   /*---(complete)-------------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return x_pos;
}

int          /*--> check for variable types --------------[--------[--------]-*/
yRPN__types          (int  a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /* types are only lowercase alphanumerics.                                 */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   int         x_pos       =    0;     /* updated position in input           */
   int         i           =    0;     /* iterator for keywords               */
   int         x_found     =   -1;     /* index of keyword                    */
   /*---(header)------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yRPN__token_error ();
   --rce;  if (zRPN_lang == S_LANG_GYGES) {
      DEBUG_YRPN    yLOG_note    ("skip in gyges mode");
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
   rpn.t_type   = S_TTYPE_TYPE;
   x_pos        = a_pos;  /* starting point */
   while (yRPN__token_add (&x_pos) == 0);
   DEBUG_YRPN    yLOG_info    ("rpn.t_name", rpn.t_name);
   /*---(try to match types)---------------*/
   DEBUG_YRPN    yLOG_note    ("search types");
   for (i = 0; i < MAX_TYPES; ++i) {
      if  (s_types [i].name [0] == '\0')                   break;
      if  (s_types [i].name [0] != rpn.t_name [0])         continue;
      if  (strcmp (s_types [i].name, rpn.t_name ) != 0)    continue;
      x_found = i;
      DEBUG_YRPN    yLOG_value   ("x_found"   , x_found);
      break;
   }
   DEBUG_YRPN    yLOG_info    ("token name", rpn.t_name);
   /*---(handle misses)--------------------*/
   --rce;  if (x_found < 0) {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("type not found");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)-----------------------------*/
   DEBUG_YRPN    yLOG_note    ("put c type directly to output");
   yRPN__token_save    (a_pos);
   rpn.left_oper  = S_OPER_CLEAR;
   /*---(complete)-------------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return x_pos;
}



/*====================------------------------------------====================*/
/*===----                       literal handling                       ----===*/
/*====================------------------------------------====================*/
static void        o___LITERALS________________o (void) {;}

int          /*--> check for string literals -------------[--------[--------]-*/
yRPN__strings        (int  a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /*  begin and end with double quotes, can escape quotes inside             */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   int         x_pos       =    0;     /* updated position in input           */
   int         i           =    0;     /* iterator for keywords               */
   int         x_found     =   -1;     /* index of keyword                    */
   int         x_last      =    0;
   /*---(header)------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yRPN__token_error ();
   DEBUG_YRPN    yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos <  0) {
      DEBUG_YRPN    yLOG_note    ("start can not be negative");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN    yLOG_note    ("accumulate characters");
   rpn.t_type   = S_TTYPE_STR;
   x_pos        = a_pos;  /* starting point */
   while (yRPN__token_add (&x_pos) == 0);
   DEBUG_YRPN    yLOG_info    ("rpn.t_name", rpn.t_name);
   /*---(check if long enough)-------------*/
   DEBUG_YRPN    yLOG_value   ("rpn.t_len" , rpn.t_len);
   --rce;  if (rpn.t_len <  2) {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("string too short");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check matching quotes)------------*/
   x_last = rpn.t_len - 1;
   --rce;  if (rpn.t_name [0] == '\"' && rpn.t_name [x_last] != '\"') {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("string does not end properly");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (rpn.t_name [0] == '<' && rpn.t_name [x_last] != '>') {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("include does not end properly");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)-----------------------------*/
   DEBUG_YRPN    yLOG_note    ("put string literal directly to output");
   yRPN__token_save    (a_pos);
   rpn.left_oper  = S_OPER_CLEAR;
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return x_pos;
}

int          /*--> check for character literals ----------[--------[--------]-*/
yRPN__chars          (int  a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /*  begin and end with single quotes, only one character inside.           */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   int         x_pos       =    0;     /* updated position in input           */
   int         i           =    0;     /* iterator for keywords               */
   int         x_found     =   -1;     /* index of keyword                    */
   int         x_last      =    0;
   /*---(header)------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yRPN__token_error ();
   DEBUG_YRPN    yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos <  0) {
      DEBUG_YRPN    yLOG_note    ("start can not be negative");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN    yLOG_note    ("accumulate characters");
   rpn.t_type   = S_TTYPE_CHAR;
   x_pos        = a_pos;  /* starting point */
   while (yRPN__token_add (&x_pos) == 0);
   DEBUG_YRPN    yLOG_info    ("rpn.t_name", rpn.t_name);
   /*---(check if long enough)-------------*/
   DEBUG_YRPN    yLOG_value   ("rpn.t_len" , rpn.t_len);
   --rce;  if (rpn.t_len <  3) {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("string too short");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (rpn.t_len >  4) {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("string long short");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check start and stop)-------------*/
   x_last = rpn.t_len - 1;
   --rce;  if (rpn.t_name [0] != rpn.t_name [x_last]) {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("char literal does not end properly");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (rpn.t_len == 4 && rpn.t_name [1] != '\\') {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("long char literal not escaped");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)-----------------------------*/
   DEBUG_YRPN    yLOG_note    ("put char literal directly to output");
   yRPN__token_save    (a_pos);
   rpn.left_oper  = S_OPER_CLEAR;
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return x_pos;
}

int          /*--> check for normal type numbers ---------[--------[--------]-*/
yRPN__numbers        (int  a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /*  begin and end with single quotes, only one character inside.           */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   int         rc          =    0;
   int         x_pos       =    0;     /* updated position in input           */
   int         x_last      =    0;
   char        x_bad       =  '-';
   /*---(header)------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yRPN__token_error ();
   DEBUG_YRPN    yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos <  0) {
      DEBUG_YRPN    yLOG_note    ("start can not be negative");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (strchr (v_number, rpn.working [a_pos]) == 0) {
      DEBUG_YRPN    yLOG_note    ("must start with a number");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN    yLOG_note    ("accumulate characters");
   x_pos        = a_pos;  /* starting point */
   rpn.t_type   = yRPN__token_nums (x_pos);
   DEBUG_YRPN    yLOG_char    ("rpn.t_type", rpn.t_type);
   while (yRPN__token_add (&x_pos) == 0);
   DEBUG_YRPN    yLOG_info    ("rpn.t_name", rpn.t_name);
   /*---(check if long enough)-------------*/
   DEBUG_YRPN    yLOG_value   ("rpn.t_len" , rpn.t_len);
   switch (rpn.t_type) {
   case S_TTYPE_INT  :
      if (rpn.t_len < 1)  x_bad = 'y';
      break;
   case S_TTYPE_OCT  :
      if (rpn.t_len < 2)  x_bad = 'y';
      break;
   default           :
      if (rpn.t_len < 3)  x_bad = 'y';
      break;
   }
   --rce;  if (x_bad == 'y') {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("number too short");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check float decimals)-------------*/
   --rce;  if (rpn.t_type == S_TTYPE_FLOAT) {
      rc = strldcnt (rpn.t_name, '.', S_LEN_TOKEN);
      --rce;  if (rc != 1) {
         yRPN__token_error ();
         DEBUG_YRPN    yLOG_note    ("too many decimals");
         DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      --rce;  if (rpn.t_name [rpn.t_len - 1] == '.') {
         yRPN__token_error ();
         DEBUG_YRPN    yLOG_note    ("can not end with decimal");
         DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(save)-----------------------------*/
   DEBUG_YRPN    yLOG_note    ("put har literal directly to output");
   yRPN__token_save    (a_pos);
   rpn.left_oper  = S_OPER_CLEAR;
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return x_pos;
}



/*====================------------------------------------====================*/
/*===----                       symbol handling                        ----===*/
/*====================------------------------------------====================*/
static void        o___SYMBOLS_________________o (void) {;}

int          /*--> check for constants -------------------[--------[--------]-*/
yRPN__constants      (int  a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /* constants only contain alphnanumerics plus the underscore.              */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   int         x_pos       =    0;     /* updated position in input           */
   int         i           =    0;     /* iterator for keywords               */
   int         x_found     =   -1;     /* index of keyword                    */
   /*---(header)------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yRPN__token_error ();
   DEBUG_YRPN    yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos <  0) {
      DEBUG_YRPN    yLOG_note    ("start can not be negative");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN    yLOG_note    ("accumulate characters");
   rpn.t_type   = S_TTYPE_CONST;
   x_pos        = a_pos;  /* starting point */
   while (yRPN__token_add (&x_pos) == 0);
   DEBUG_YRPN    yLOG_info    ("rpn.t_name", rpn.t_name);
   /*---(try to match constants)-----------*/
   DEBUG_YRPN    yLOG_note    ("search constants");
   for (i = 0; i < MAX_CONST; ++i) {
      if  (s_consts [i].name [0] == '\0')                   break;
      if  (s_consts [i].name [0] != rpn.t_name [0])         continue;
      if  (strcmp (s_consts [i].name, rpn.t_name ) != 0)    continue;
      x_found = i;
      DEBUG_YRPN    yLOG_value   ("x_found"   , x_found);
      break;
   }
   DEBUG_YRPN    yLOG_info    ("token name", rpn.t_name);
   /*---(handle misses)--------------------*/
   --rce;  if (x_found < 0) {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("type not found");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)-----------------------------*/
   DEBUG_YRPN    yLOG_note    ("put constant directly to output");
   yRPN__token_save    (a_pos);
   rpn.left_oper  = S_OPER_CLEAR;
   /*---(complete)-------------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return x_pos;
}

int          /*--> check for symbol names ----------------[--------[--------]-*/
yRPN__funcvar      (int   a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /* func/vars only contain alphnanumerics plus the underscore.              */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   int         x_pos       =    0;     /* updated position in input           */
   int         i           =    0;     /* iterator for keywords               */
   int         x_found     =   -1;     /* index of keyword                    */
   /*---(header)------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yRPN__token_error ();
   DEBUG_YRPN    yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos <  0) {
      DEBUG_YRPN    yLOG_note    ("start can not be negative");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN    yLOG_note    ("accumulate characters");
   rpn.t_type   = S_TTYPE_VARS;
   x_pos        = a_pos;  /* starting point */
   while (yRPN__token_add (&x_pos) == 0);
   DEBUG_YRPN    yLOG_info    ("rpn.t_name", rpn.t_name);
   /*---(check if long enough)-------------*/
   DEBUG_YRPN    yLOG_value   ("rpn.t_len" , rpn.t_len);
   --rce;  if (rpn.t_len <  1) {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("name too short");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check func vs vars)---------------*/
   rpn.t_type   = yRPN__token_paren (x_pos);
   /*---(save)-----------------------------*/
   if (rpn.t_type == S_TTYPE_FUNC) {
      DEBUG_YRPN    yLOG_note    ("put function on stack");
      rpn.t_prec = S_PREC_FUNC;
      yRPN__token_push    (a_pos);
   } else {
      DEBUG_YRPN    yLOG_note    ("put variable directly to output");
      yRPN__token_save    (a_pos);
   }
   rpn.left_oper  = S_OPER_CLEAR;
   /*---(complete)-------------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return x_pos;
}



/*====================------------------------------------====================*/
/*===----                          token handling                      ----===*/
/*====================------------------------------------====================*/
static void        o___TOKENS__________________o (void) {;}

int        /* ---- : save off string literals --------------------------------*/
yRPN__text         (int  a_pos)
{
   strcat (rpn.tokens, rpn.working + a_pos);
   strcat (rpn.tokens, " ");
   return strlen (rpn.working);
}

char
yRPN__oper_stack     (int a_pos)
{
   yRPN__peek ();
   DEBUG_OPER  yLOG_complex ("prec"      , "curr=%c, stack=%c", rpn.t_prec, rpn.p_prec);
   zRPN_DEBUG  printf("      precedence %c versus stack top of %c\n", rpn.t_prec, rpn.p_prec);
   if ( (rpn.t_dir == S_LEFT && rpn.t_prec >= rpn.p_prec) ||
         (rpn.t_dir == S_RIGHT && rpn.t_prec >  rpn.p_prec)) {
      while ((rpn.t_dir == S_LEFT && rpn.t_prec >= rpn.p_prec) ||
            (rpn.t_dir == S_RIGHT && rpn.t_prec >  rpn.p_prec)) {
         /*> if (rpn__last != 'z') RPN__pops();                                       <*/
         if (rpn.p_prec == 'z') break;
         yRPN__pops();
         yRPN__peek();
      }
      yRPN__push();
      yRPN__normal (a_pos);
   } else {
      yRPN__push();
      yRPN__normal (a_pos);
   }
}

int          /*--> check for operators -------------------[--------[--------]-*/
yRPN__operators      (int  a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /* operators are symbols and stored in a table.                            */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   int         x_pos       =    0;     /* updated position in input           */
   int         i           =    0;     /* iterator for keywords               */
   int         x_found     =   -1;     /* index of keyword                    */
   /*---(header)------------------------*/
   DEBUG_YRPN    yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yRPN__token_error ();
   DEBUG_YRPN    yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos <  0) {
      DEBUG_YRPN    yLOG_note    ("start can not be negative");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN    yLOG_note    ("accumulate characters");
   rpn.t_type   = S_TTYPE_OPER;
   x_pos        = a_pos;  /* starting point */
   while (yRPN__token_add (&x_pos) == 0);
   DEBUG_YRPN    yLOG_info    ("rpn.t_name", rpn.t_name);
   /*---(try to match operators)-----------*/
   DEBUG_YRPN    yLOG_note    ("search operators");
   for (i = 0; i < MAX_OPER; ++i) {
      if  (s_opers [i].name [0] == '\0')                   break;
      if  (s_opers [i].real     != 'r' )                   continue;
      if  (s_opers [i].name [0] != rpn.t_name [0])         continue;
      if  (strcmp (s_opers [i].name, rpn.t_name ) != 0)    continue;
      x_found = i;
      DEBUG_YRPN    yLOG_value   ("x_found"   , x_found);
      break;
   }
   if (x_found < 0 && rpn.t_len == 2) {
      rpn.t_name [1] = '\0';
      rpn.t_len      = 1;
      --x_pos;
      for (i = 0; i < MAX_OPER; ++i) {
         if  (s_opers [i].name [0] == '\0')                   break;
         if  (s_opers [i].real     != 'r' )                   continue;
         if  (s_opers [i].name [0] != rpn.t_name [0])         continue;
         if  (strcmp (s_opers [i].name, rpn.t_name ) != 0)    continue;
         x_found = i;
         DEBUG_YRPN    yLOG_value   ("x_found"   , x_found);
         break;
      }
   }
   /*---(handle misses)--------------------*/
   --rce;  if (x_found < 0) {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("operator not found");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check for unary)------------------*/
   if (rpn.left_oper  == S_OPER_LEFT) {
      DEBUG_YRPN    yLOG_note    ("check for left operators in right only mode");
      if        (strcmp (rpn.t_name, "+" ) == 0)  { strcpy (rpn.t_name, "+:"); rpn.t_len  = 2;
      } else if (strcmp (rpn.t_name, "-" ) == 0)  { strcpy (rpn.t_name, "-:"); rpn.t_len  = 2;
      } else if (strcmp (rpn.t_name, "*" ) == 0)  { strcpy (rpn.t_name, "*:"); rpn.t_len  = 2;
      } else if (strcmp (rpn.t_name, "&" ) == 0)  { strcpy (rpn.t_name, "&:"); rpn.t_len  = 2;
      }
   } else {
      DEBUG_YRPN    yLOG_note    ("check for right operators in left only mode");
      if        (strcmp (rpn.t_name, "++") == 0)  { strcpy (rpn.t_name, ":+"); rpn.t_len  = 2;
      } else if (strcmp (rpn.t_name, "--") == 0)  { strcpy (rpn.t_name, ":-"); rpn.t_len  = 2;
      }
   }
   /*---(handle it)------------------------*/
   yRPN__precedence ();
   yRPN__peek ();
   DEBUG_OPER  yLOG_complex ("prec"      , "curr=%c, stack=%c", rpn.t_prec, rpn.p_prec);
   zRPN_DEBUG  printf("      precedence %c versus stack top of %c\n", rpn.t_prec, rpn.p_prec);
   if ( (rpn.t_dir == S_LEFT && rpn.t_prec >= rpn.p_prec) ||
         (rpn.t_dir == S_RIGHT && rpn.t_prec >  rpn.p_prec)) {
      while ((rpn.t_dir == S_LEFT && rpn.t_prec >= rpn.p_prec) ||
            (rpn.t_dir == S_RIGHT && rpn.t_prec >  rpn.p_prec)) {
         /*> if (rpn__last != 'z') RPN__pops();                                       <*/
         if (rpn.p_prec == 'z') break;
         yRPN__pops();
         yRPN__peek();
      }
      yRPN__push();
      yRPN__normal (a_pos);
   } else {
      yRPN__push();
      yRPN__normal (a_pos);
   }
   /*---(save)-----------------------------*/
   DEBUG_YRPN    yLOG_note    ("put constant directly to output");
   rpn.left_oper  = S_OPER_LEFT;  /* an oper after an oper must be right-only */
   /*---(complete)-------------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return x_pos;
}



/*====================------------------------------------====================*/
/*===----                          group handling                      ----===*/
/*====================------------------------------------====================*/
static void        o___GROUPING________________o (void) {;}

int
yRPN__paren_open     (int a_pos)
{
   yRPN__token_push (a_pos);
   rpn.left_oper  = S_OPER_LEFT;
   return 0;
}

int
yRPN__brack_open     (int a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /* push a close bracket and open paren to the stack                        */
   /*---(handle bracket)-----------------*/
   yRPN__token      ();
   yRPN__normal     (a_pos);
   /*---(put reverse on stack)-----------*/
   strcpy (rpn.t_name, "]");
   rpn.t_type     = S_TTYPE_OPER;
   yRPN__push();
   /*---(pretend open paren)-------------*/
   strcpy (rpn.t_name, "(");
   rpn.t_type     = S_TTYPE_GROUP;
   yRPN__precedence ();
   yRPN__token      ();
   yRPN__push       ();
   rpn.left_oper  = S_OPER_LEFT;
   /*---(complete)-----------------------*/
   return 0;
}

int
yRPN__paren_close    (int a_pos)
{
   char rc = 0;
   yRPN__token  ();
   yRPN__normal (a_pos);
   rc = yRPN__peek();
   while (rc >= 0  &&  rpn.p_prec != 'd' + 16) {
      yRPN__pops();
      rc = yRPN__peek();
   }
   if (rc < 0) {
      zRPN_DEBUG  printf ("      FATAL :: nothing more on stack\n");
      return rc;
   }
   yRPN__toss();
   rpn.left_oper  = S_OPER_CLEAR;
   return 0;
}

int
yRPN__paren_comma    (int a_pos)
{
   char rc = 0;
   yRPN__token ();
   rc = yRPN__peek();
   while (rc >= 0  &&  rpn.p_prec != 'd' + 16) {
      yRPN__pops();
      rc = yRPN__peek();
   }
   if (rc < 0) {
      /*> zRPN_DEBUG  printf ("      FATAL :: nothing more on stack\n");           <*/
      /*> return rc;                                                               <*/
   }
   if (zRPN_lang != S_LANG_GYGES) {
      rpn.t_type = S_TTYPE_OPER;
      yRPN__save ();
      yRPN__normal (a_pos);
      rpn.left_oper  = S_OPER_LEFT;
   }
   return 0;
}

/*> int          /+--> check for grouping symbols ------------[--------[--------]-+/   <* 
 *> yRPN__grouping       (int  a_pos)                                                  <* 
 *> {  /+---(design notes)--------------------------------------------------------+/   <* 
 *>    /+ grouping symbols are all one-char and specific.                         +/   <* 
 *>    /+---(locals)-----------+-----------+-+/                                        <* 
 *>    char        rce         =  -10;     /+ return code for errors              +/   <* 
 *>    int         x_pos       =    0;     /+ updated position in input           +/   <* 
 *>    int         i           =    0;     /+ iterator for keywords               +/   <* 
 *>    int         x_found     =   -1;     /+ index of keyword                    +/   <* 
 *>    /+---(header)------------------------+/                                         <* 
 *>    DEBUG_YRPN    yLOG_enter   (__FUNCTION__);                                      <* 
 *>    /+---(defenses)-----------------------+/                                        <* 
 *>    yRPN__token_error ();                                                           <* 
 *>    DEBUG_YRPN    yLOG_value   ("a_pos"     , a_pos);                               <* 
 *>    --rce;  if (a_pos <  0) {                                                       <* 
 *>       DEBUG_YRPN    yLOG_note    ("start can not be negative");                    <* 
 *>       DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);                              <* 
 *>       return rce;                                                                  <* 
 *>    }                                                                               <* 
 *>    --rce;  if (strchr (v_group, rpn.working [a_pos]) == 0) {                       <* 
 *>       DEBUG_YRPN    yLOG_note    ("not a valid grouping symbol");                  <* 
 *>       DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);                              <* 
 *>       return rce;                                                                  <* 
 *>    }                                                                               <* 
 *>    /+---(accumulate characters)------------+/                                      <* 
 *>    DEBUG_YRPN    yLOG_note    ("accumulate characters");                           <* 
 *>    rpn.t_name [0] = rpn.working [a_pos];                                           <* 
 *>    rpn.t_name [1] = '\0';                                                          <* 
 *>    rpn.t_len      = 1;                                                             <* 
 *>    rpn.t_type     = S_TTYPE_GROUP;                                                 <* 
 *>    x_pos          = a_pos + 1;                                                     <* 
 *>    DEBUG_YRPN    yLOG_info    ("rpn.t_name", rpn.t_name);                          <* 
 *>    DEBUG_YRPN    yLOG_value   ("rpn.t_len" , rpn.t_len);                           <* 
 *>    yRPN__precedence ();                                                            <* 
 *>                                                                                    <* 
 *>                                                                                    <* 
 *>                                                                                    <* 
 *> }                                                                                  <*/

int        /* ---- : process grouping ----------------------------------------*/
yRPN__grouping     (int  a_pos)
{
   /*---(design notes)-------------------*/
   /*
    *  operators can have a wild number of forms and precedences
    *
    */
   /*---(begin)--------------------------*/
   zRPN_DEBUG  printf("   group------------------\n");
   /*---(locals)---------------------------*/
   int       i         = a_pos;
   int       rc        = 0;
   char      x_match   = ' ';
   char      x_fake    = 'n';
   /*---(prepare)------------------------*/
   strncpy (rpn.t_name, YRPN_TOKEN_NULL, zRPN_MAX_LEN);
   rpn.t_name[0] = rpn.working[i];
   rpn.t_name[1] = '\0';
   rpn.t_type     = S_TTYPE_GROUP;
   rpn.t_prec     = S_PREC_NONE;
   /*---(defenses)-----------------------*/
   if (i              >= rpn.l_working)           return  zRPN_ERR_INPUT_NOT_AVAIL;
   if (strchr(v_group   , rpn.working[i]) == 0)  return  zRPN_ERR_NOT_GROUPING;
   /*---(main loop)------------------------*/
   rpn.t_len  = 1;
   ++i;
   /*---(end)------------------------------*/
   zRPN_DEBUG  printf("      fin (%02d) <<%s>>\n", rpn.t_len , rpn.t_name);
   /*---(handle it)------------------------*/
   yRPN__precedence ();
   zRPN_DEBUG  printf("      prec = %c\n", rpn.t_prec);
   /*---(open bracket)---------------------*/
   if (rpn.t_name[0] == '[') {
      yRPN__token ();
      /*> strcpy (rpn.t_name, "]*");                                                   <*/
      strcpy (rpn.t_name, "]");
      rpn.t_type         = S_TTYPE_OPER;
      yRPN__push();
      strcpy (rpn.t_name, "[");
      yRPN__normal (a_pos);
      x_fake = 'y';
      strcpy (rpn.t_name, "(");
      rpn.t_type         = S_TTYPE_GROUP;
      yRPN__precedence ();
   }
   /*---(open paren)-----------------------*/
   if (rpn.t_name[0] == '(') {
      yRPN__token ();
      yRPN__push();
      if (x_fake == 'n')  yRPN__normal (a_pos);
      rpn.left_oper  = S_LEFT_ONLY;
      return 0;
   }
   /*---(close bracket)-------------------*/
   if (rpn.t_name[0] == ']') {
      yRPN__token ();
      strcpy (rpn.t_name, ")");
      rpn.t_type         = S_TTYPE_GROUP;
      yRPN__precedence ();
      x_fake = 'y';
   }
   /*---(close paren)----------------------*/
   if (rpn.t_name[0] == ')') {
      yRPN__token ();
      if (x_fake == 'n')  yRPN__normal (a_pos);
      rc = yRPN__peek();
      while (rc >= 0  &&  rpn.p_prec != 'd' + 16) {
         yRPN__pops();
         rc = yRPN__peek();
      }
      if (rc < 0) {
         zRPN_DEBUG  printf ("      FATAL :: nothing more on stack\n");
         return rc;
      }
      yRPN__toss();
      rc = yRPN__peek();
      if (rpn.t_token[0] == ']')  yRPN__pops();
      rpn.left_oper  = S_OPER_CLEAR;
   }
   /*---(comma)----------------------------*/
   else if (strncmp(rpn.t_name, ",", 1) == 0) {
      yRPN__token ();
      rc = yRPN__peek();
      while (rc >= 0  &&  rpn.p_prec != 'd' + 16) {
         yRPN__pops();
         rc = yRPN__peek();
      }
      if (rc < 0) {
         /*> zRPN_DEBUG  printf ("      FATAL :: nothing more on stack\n");           <*/
         /*> return rc;                                                               <*/
      }
      if (zRPN_lang != S_LANG_GYGES) {
         rpn.t_type = S_TTYPE_OPER;
         yRPN__save ();
         yRPN__normal (a_pos);
         rpn.left_oper  = S_OPER_LEFT;
      }
   }
   /*---(complete)-------------------------*/
   return i;
}



/*====================------------------------------------====================*/
/*===----                     spreadsheet specific                     ----===*/
/*====================------------------------------------====================*/
static void        o___GYGES___________________o (void) {;}

char        s_addr      [20];

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
   strncpy (rpn.t_name, YRPN_TOKEN_NULL, zRPN_MAX_LEN);
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
   yRPN__token ();
   yRPN__save();
   yRPN__normal (a_pos);
   /*---(end)------------------------------*/
   zRPN_DEBUG  printf("      fin (%02d) <<%s>>\n", rpn.t_len , rpn.t_name);
   rpn.left_oper  = S_OPER_CLEAR;
   /*---(complete)-------------------------*/
   zRPN_DEBUG  printf("exiting yRPN_addresses");
   return i;
}


/*====================------------------------------------====================*/
/*===----                            driver                            ----===*/
/*====================------------------------------------====================*/
static void        o___DRIVER__________________o (void) {;}

int          /*--> prepare variables for rpn conversion --[ ------ [ ------ ]-*/
yRPN__load         (char *a_source)   /* source infix string                          */
{
   DEBUG_YRPN    yLOG_senter  (__FUNCTION__);
   /*---(set the source/working)---------*/
   if (a_source == NULL || a_source [0] == '\0') {
      strncpy (rpn.source  , YRPN_TOKEN_NULL, S_LEN_OUTPUT);
      strncpy (rpn.working , ""             , S_LEN_OUTPUT);
      rpn.l_source  = rpn.l_working = 0;
   } else {
      strncpy (rpn.source  , a_source       , S_LEN_OUTPUT);
      strncpy (rpn.working , a_source       , S_LEN_OUTPUT);
      rpn.l_source  = rpn.l_working = strlen (rpn.source);
   }
   DEBUG_YRPN    yLOG_sint    (rpn.l_source);
   /*---(set the outputs)----------------*/
   DEBUG_YRPN    yLOG_snote   ("clear output");
   strncpy (rpn.shuntd  , ""             , S_LEN_OUTPUT);
   strncpy (rpn.detail  , ""             , S_LEN_OUTPUT);
   strncpy (rpn.normal  , ""             , S_LEN_OUTPUT);
   strncpy (rpn.tokens  , ""             , S_LEN_OUTPUT);
   rpn.l_shuntd   = 0;
   rpn.l_normal   = 0;
   rpn.n_shuntd   = 0;
   DEBUG_YRPN    yLOG_sint    (rpn.l_shuntd);
   /*---(set the token vars)-------------*/
   DEBUG_YRPN    yLOG_snote   ("token vars");
   strncpy (rpn.t_name  , YRPN_TOKEN_NULL, S_LEN_OUTPUT);
   rpn.t_type     = S_TTYPE_ERROR;
   rpn.t_len      = 0;
   rpn.t_type     = 'e';
   rpn.t_prec     = '-';
   rpn.t_dir      = S_LEFT;
   rpn.t_arity    = 0;
   rpn.left_oper  = S_OPER_LEFT;
   rpn.pproc      = '-';
   rpn.p_type     = '-';
   rpn.p_prec     = '-';
   /*---(set the stack vars)-------------*/
   DEBUG_YRPN    yLOG_snote   ("stack");
   rpn.n_stack    = 0;
   rpn.depth      = 0;
   /*---(complete)-----------------------*/
   DEBUG_YRPN    yLOG_sexit   (__FUNCTION__);
   return  rpn.l_source;
}

char       /* ---- : set c human readable ------------------------------------*/
yRPN__chuman       (void)
{
   zRPN_lang    = S_LANG_C;
   strcpy (zRPN_divider, " ");
   return 0;
}

char       /* ---- : set spreadsheet human readable --------------------------*/
yRPN__shuman       (int *a_ntoken)
{
   zRPN_lang    = S_LANG_GYGES;
   strcpy (zRPN_divider, " ");
   return 0;
}

char       /* ---- : set c internal ------------------------------------------*/
yRPN_compiler      (void)
{
   zRPN_lang    = S_LANG_C;
   strcpy (zRPN_divider, ", ");
   strcpy (zRPN_divtech, "\x1F");
   return 0;
}

char*        /*--> convert spreadsheet infix to rpn ------[ ------ [ ------ ]-*/
yRPN_spreadsheet   (
      /*----------+-----------+-----------------------------------------------*/
      char       *a_source,   /* source infix string                          */
      int        *a_ntoken)   /* number of rpn tokens in result (return)      */
{
   /*---(locals)-----------+-----------+-*/
   char       *x_rpn       = NULL;          /* return string of rpn notation  */
   /*---(prepare flags)------------------*/
   zRPN_lang    = S_LANG_GYGES;
   strcpy (zRPN_divider, ",");
   /*---(convert)------------------------*/
   x_rpn = yRPN_convert (a_source);
   zRPN_DEBUG  printf("   ready   = <<%s>>\n", x_rpn);
   /*---(interprete results)-------------*/
   if (x_rpn == NULL)  {
      if (a_ntoken != NULL) *a_ntoken = 0;
      return NULL;
   }
   *a_ntoken = rpn.n_shuntd - 1;
   /*---(complete)-----------------------*/
   return x_rpn;
}

char*      /* ---- : retrieve the tokenized format ---------------------------*/
yRPN_stokens       (char *a_source)
{
   char     *x_rpn = NULL;
   zRPN_lang    = S_LANG_GYGES;
   strcpy (zRPN_divider, " ");
   x_rpn = yRPN_convert (a_source);
   if (x_rpn == NULL)   return NULL;
   return rpn.tokens;
}

char*      /* ---- : retrieve the normal format ------------------------------*/
yRPN_normal        (char *a_source, char *a_normal, int *a_ntoken)
{
   char     *x_rpn = NULL;
   zRPN_lang    = S_LANG_C;
   strcpy (zRPN_divider, ", ");
   strcpy (zRPN_divtech, "\x1F");
   x_rpn = yRPN_convert (a_source);
   if (x_rpn == NULL)  {
      if (a_normal != NULL) strcpy (a_normal, "");
      if (a_ntoken != NULL) *a_ntoken = 0;
      return NULL;
   }
   if (a_normal != NULL) strcpy (a_normal, rpn.normal);
   if (a_ntoken != NULL) *a_ntoken = rpn.l_normal;
   return rpn.shuntd;
}

char*      /* ---- : retrieve the detailed format ----------------------------*/
yRPN_detail        (char *a_source, char *a_detail, int *a_ntoken)
{
   char     *x_rpn = NULL;
   zRPN_lang    = S_LANG_C;
   strcpy (zRPN_divider, ", ");
   strcpy (zRPN_divtech, "\x1F");
   x_rpn = yRPN_convert (a_source);
   if (x_rpn == NULL)  {
      if (a_detail != NULL) strcpy (a_detail, "");
      if (a_ntoken != NULL) *a_ntoken = 0;
      return NULL;
   }
   if (a_detail != NULL) strcpy (a_detail, rpn.detail);
   if (a_ntoken != NULL) *a_ntoken = rpn.n_shuntd;
   return rpn.shuntd;
}

char*      /* ---- : retrieve the tokenized format ---------------------------*/
yRPN_techtoken     (char *a_source)
{
   char     *x_rpn = NULL;
   zRPN_lang    = S_LANG_C;
   strcpy (zRPN_divider, "\x1F");
   x_rpn = yRPN_convert (a_source);
   if (x_rpn == NULL)   return NULL;
   return rpn.tokens;
}

char*      /* ---- : retrieve the tokenized format ---------------------------*/
yRPN_tokens        (char *a_source)
{
   char     *x_rpn = NULL;
   zRPN_lang    = S_LANG_C;
   strcpy (zRPN_divider, " ");
   x_rpn = yRPN_convert (a_source);
   if (x_rpn == NULL)   return NULL;
   return rpn.tokens;
}

char       /* ---- : calculate the stack depth -------------------------------*/
yRPN__depth        (void)
{
   /*> yLOG_enter  (__FUNCTION__);                                                    <*/
   /*---(locals)-------*-----------------*/
   char      x_detail  [zRPN_MAX_LEN] = "";
   int       i         = 0;            /* loop iterator -- token              */
   char     *p         = NULL;         /* pointer to current token            */
   char     *s         = NULL;         /* strtok context variable             */
   int       x_curr    = 0;            /* current depth                       */
   int       x_max     = 0;            /* maximum depth                       */
   int       x_arity   = 0;            /* number of operands                  */
   /*---(defense)------------------------*/
   /*> yLOG_note   ("before NULL check");                                             <*/
   if (rpn.detail == NULL)   return -1;
   /*> yLOG_note   ("after NULL check");                                              <*/
   /*---(prepare)------------------------*/
   strcpy (x_detail, rpn.detail);
   printf ("x = %p\n", x_detail);
   /*> yLOG_note   (x_detail);                                                        <*/
   /*> yLOG_note   ("after copy");                                                    <*/
   /*---(run through the tokens)---------*/
   /*> p = strtok_r (x_detail, zRPN_divider, &s);                                     <*/
   p = strtok_r (x_detail, " ", &s);
   printf ("p = %p\n", p);
   /*> int lenny = strlen(p);                                                         <* 
    *> printf ("l = %d\n", lenny);                                                    <*/
   /*> yLOG_note   ("after strtok_r");                                                <*/
   while (p != NULL) {
      if (p[0] == 'o')  x_curr   -= (yRPN_arity (p + 2) - 1);
      else              ++x_curr;
      if (x_curr > x_max)  x_max = x_curr;
      printf ("token = %-10s, x_curr = %2d, x_max = %2d\n", p, x_curr, x_max);
      p = strtok_r (NULL, zRPN_divider, &s);
   }
   /*> yLOG_note   ("after loop");                                                    <*/
   rpn.depth = x_max;
   /*---(complete)-----------------------*/
   /*> yLOG_exit   ();                                                                <*/
   return 0;
}

char       /* ---- : convert normal infix notation to postfix/rpn ------------*/
yRPN__output_done    (void)
{
   /*---(locals)-----------+-----------+-*/
   int         x_len       = 0;
   /*---(drop the trailing space)--------*/
   x_len = strlen (rpn.shuntd) - strlen (zRPN_divider);
   rpn.shuntd [x_len] = '\0';
   x_len = strlen (rpn.detail) - strlen (zRPN_divtech);
   rpn.detail [x_len] = '\0';
   x_len = strlen (rpn.normal) - strlen (zRPN_divtech);
   rpn.normal[x_len] = '\0';
   x_len = strlen (rpn.tokens) - 1;
   rpn.tokens [x_len] = '\0';
   /*---(output)-------------------------*/
   zRPN_DEBUG  printf("   shunted = <<%s>>\n", rpn.shuntd);
   zRPN_DEBUG  printf("   detail  = <<%s>>\n", rpn.detail);
   zRPN_DEBUG  printf("   normal  = <<%s>>\n", rpn.normal);
   zRPN_DEBUG  printf("   tokens  = <<%s>>\n", rpn.tokens);
   zRPN_DEBUG  printf("   depth   = <<%d>>\n", rpn.depth);
   DEBUG_TOPS  yLOG_info    ("shunted"   , rpn.shuntd);
   DEBUG_TOPS  yLOG_info    ("detail"    , rpn.detail);
   DEBUG_TOPS  yLOG_info    ("normal"    , rpn.normal);
   DEBUG_TOPS  yLOG_info    ("tokens"    , rpn.tokens);
   DEBUG_TOPS  yLOG_value   ("depth"     , rpn.depth);
   return 0;
}

char*      /* ---- : convert normal infix notation to postfix/rpn ------------*/
yRPN_convert       (char *a_source)
{
   /*---(design notes)-------------------*/
   /*
    *   convert from normal infix to the more efficient RPN/postfix notation
    *   and at the same time, identify any critical errors
    *      - unbalanced parens
    *      - unknown operator (+-, =+, etc)
    *      - unknown variable/reference name
    *      - unknown function
    *   all presedence should match ansi-c to keep it clear and clean for me
    *
    */
   DEBUG_TOPS  yLOG_enter   (__FUNCTION__);
   zRPN_DEBUG  printf("RPN_convert     :: beg ------------------------------\n");
   /*---(locals)-------------------------*/
   int       x_pos     = 1;
   int       len       = 0;
   int       rc        = 0;
   char      x_ch      = 0;
   /*---(defenses)-----------------------*/
   if (a_source     == NULL)  {
      DEBUG_TOPS  yLOG_exit    (__FUNCTION__);
      return 0;    /* no source to convert         */
   }
   yRPN__load (a_source);
   /*---(main loop)----------------------*/
   zRPN_DEBUG  printf("   ---process-------------\n");
   x_pos = 0;
   rpn.pproc = S_PPROC_NO;
   while (x_pos < rpn.l_working) {
      /*---(prepare)---------------------*/
      x_ch    = rpn.working [x_pos];
      rc      = x_pos;
      /*---(pick handler)----------------*/
      if (rc <= x_pos && x_ch == '\"')                    rc = yRPN__strings    (x_pos);
      if (rc <= x_pos && x_ch == '\'')                    rc = yRPN__chars      (x_pos);
      if (rc <= x_pos && strchr (v_address , x_ch) != 0)  rc = yRPN__addresses  (x_pos);
      if (rc <= x_pos && strchr (v_lower   , x_ch) != 0)  rc = yRPN__keywords   (x_pos);
      if (rc <= x_pos && strchr (v_lower   , x_ch) != 0)  rc = yRPN__types      (x_pos);
      if (rc <= x_pos && strchr (v_alpha   , x_ch) != 0)  rc = yRPN__constants  (x_pos);
      if (rc <= x_pos && strchr (v_alpha   , x_ch) != 0)  rc = yRPN__funcvar    (x_pos);
      if (rc <= x_pos && strchr (v_number  , x_ch) != 0)  rc = yRPN__numbers    (x_pos);
      if (rc <= x_pos && strchr (v_group   , x_ch) != 0)  rc = yRPN__grouping   (x_pos);
      if (rc <= x_pos && strchr (v_operator, x_ch) != 0)  rc = yRPN__operators  (x_pos);
      /*> if (rc <= x_pox_pos && x_ch == '<' && rpn.pproc == S_PPROC_INCL)  rc = yRPN__strings    (x_pos);   <* 
       *> if (rc <= x_pos &&                rpn.pproc == S_PPROC_OTHER) rc = yRPN__text       (i);   <*/
      /*---(unrecognized)----------------*/
      if (rc <= x_pos) {
         if (x_ch == ' ')  zRPN_DEBUG  printf ("   whitespace\n");
         ++x_pos;
         continue;
      }
      /*---(output)----------------------*/
      zRPN_DEBUG  printf("   shuntd  = <<%s>>\n", rpn.shuntd);
      zRPN_DEBUG  printf("   detail  = <<%s>>\n", rpn.detail);
      zRPN_DEBUG  printf("   normal  = <<%s>>\n", rpn.normal);
      zRPN_DEBUG  printf("   tokens  = <<%s>>\n", rpn.tokens);
      /*---(error handling)--------------*/
      if (rc < 0)  break;
      /*---(next)------------------------*/
      x_pos = rc;
   }
   /*---(handle errors)------------------*/
   if (rc < 0) {
      zRPN_DEBUG  printf ("FATAL %4d : %s\n", rc, zRPN_ERRORS [ -rc - 100]);
      rpn.l_shuntd = 0;
      rpn.n_shuntd   = 0;
      strcpy (rpn.shuntd, YRPN_TOKEN_NULL);
      /*> return  strndup (rpn.shuntd, zRPN_MAX_LEN);                                 <*/
      /*> return  rpn.shuntd;                                                         <*/
      DEBUG_TOPS  yLOG_exit    (__FUNCTION__);
      return NULL;
   }
   /*---(clear stack)--------------------*/
   zRPN_DEBUG  printf("   ---clear stack---------\n");
   while (rpn.n_stack > 0) {
      yRPN__peek();
      if (strcmp(rpn.t_token, "(") == 0) {
         zRPN_DEBUG  printf ("FATAL %4d : %s\n", zRPN_ERR_UNBALANCED_PARENS, "unbalanced parentheses\n");
         rpn.l_shuntd = 0;
         rpn.n_shuntd   = 0;
         strcpy (rpn.shuntd, YRPN_TOKEN_NULL);
         /*> return  strndup (rpn.shuntd, zRPN_MAX_LEN);                              <*/
         /*> return  rpn.shuntd;                                                      <*/
         DEBUG_TOPS  yLOG_exit    (__FUNCTION__);
         return NULL;
      }
      yRPN__pops();
   }
   zRPN_DEBUG  printf("      done\n");
   /*---(drop the trailing space)--------*/
   /*> len = strlen(rpn.shuntd) - strlen(zRPN_divider);                                         <* 
    *> rpn.shuntd[len] = '\0';                                                                  <* 
    *> zRPN_DEBUG  printf("      done 2\n");                                                    <* 
    *> len = strlen(rpn.detail) - strlen(zRPN_divtech);                                         <* 
    *> rpn.detail[len] = '\0';                                                                  <* 
    *> len = strlen(rpn.normal) - strlen(zRPN_divtech);                                         <* 
    *> rpn.normal[len] = '\0';                                                                  <* 
    *> zRPN_DEBUG  printf("      done 3\n");                                                    <* 
    *> /+> yRPN__depth();                                                                 <+/   <* 
    *> zRPN_DEBUG  printf("      done 4\n");                                                    <* 
    *> len = strlen(rpn.tokens) - 1;                                                            <* 
    *> rpn.tokens[len] = '\0';                                                                  <* 
    *> zRPN_DEBUG  printf("      done 5\n");                                                    <*/
   /*---(output)-------------------------*/
   yRPN__output_done ();
   /*> zRPN_DEBUG  printf("   shunted = <<%s>>\n", rpn.shuntd);                       <* 
    *> zRPN_DEBUG  printf("   detail  = <<%s>>\n", rpn.detail);                       <* 
    *> zRPN_DEBUG  printf("   normal  = <<%s>>\n", rpn.normal);                       <* 
    *> zRPN_DEBUG  printf("   tokens  = <<%s>>\n", rpn.tokens);                       <* 
    *> zRPN_DEBUG  printf("   depth   = <<%d>>\n", rpn.depth);                        <* 
    *> DEBUG_TOPS  yLOG_info    ("shunted"   , rpn.shuntd);                           <* 
    *> DEBUG_TOPS  yLOG_info    ("detail"    , rpn.detail);                           <* 
    *> DEBUG_TOPS  yLOG_info    ("normal"    , rpn.normal);                           <* 
    *> DEBUG_TOPS  yLOG_info    ("tokens"    , rpn.tokens);                           <* 
    *> DEBUG_TOPS  yLOG_value   ("depth"     , rpn.depth);                            <*/
   /*---(complete)-----------------------*/
   zRPN_DEBUG  printf("RPN_convert     :: end ------------------------------\n");
   DEBUG_TOPS  yLOG_exit    (__FUNCTION__);
   /*> return  strndup (rpn.shuntd, zRPN_MAX_LEN);                                    <*/
   return  rpn.shuntd;
}


/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

char          unit_answer [ S_LEN_OUTPUT ];

char*      /* ---- : answer unit testing gray-box questions ------------------*/
yRPN_accessor      (char *a_question, int a_item)
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;
   char        x_temp      [S_LEN_LABEL];
   /*---(sheet focus)--------------------*/
   if        (strcmp (a_question, "source"    )     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "source string    :%.50s", rpn.source);
   } else if (strcmp (a_question, "token"     )     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "current token    :%.50s", rpn.t_name);
   } else if (strcmp (a_question, "precedence")     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "precedence       : %c"    , rpn.t_prec);
   } else if (strcmp (a_question, "type"      )     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "current type     : %c"    , rpn.t_type);
   } else if (strcmp (a_question, "n_shuntd"     )     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "token n_shuntd      : %d"    , rpn.n_shuntd);
   } else if (strcmp (a_question, "output"    )     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "postfix output   :%.50s", rpn.shuntd);
   } else if (strcmp (a_question, "detail"    )     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "postfix detail   :%.50s", rpn.detail);
   } else if (strcmp (a_question, "tokens"    )     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "tokenization     :%.50s", rpn.tokens);
   } else if (strcmp (a_question, "depth"     )     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "stack depth      : %d"    , rpn.depth);
   } else if (strcmp (a_question, "stack_list")     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "stack details    :");
      for (i = 0; i < rpn.n_stack && i < 6; ++i) {
         strlcat (unit_answer, rpn.stack [i], S_LEN_OUTPUT);
      }
   } else if (strcmp (a_question, "stack_type")     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "stack types      :");
      for (i = 0; i < rpn.n_stack && i < 6; ++i) {
         sprintf (x_temp     , S_LEN_LABEL, " %c", rpn.stack [i][0]);
         strlcat (unit_answer, x_temp       , S_LEN_OUTPUT);
      }
   } else if (strcmp (a_question, "address"   )     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "yRPN address     : tab=%4d, col=%4d, row=%4d, abs=%1d", v_tab, v_col, v_row, v_abs);
   }
   /*---(UNKNOWN)------------------------*/
   else {
      snprintf(unit_answer, S_LEN_OUTPUT, "UNKNOWN          : question is not understood");
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}

char       /*----: set up programgents/debugging -----------------------------*/
yRPN__testquiet     (void)
{
   char       *x_args [2]  = { "yRPN_debug","@@quiet" };
   yURG_logger (2, x_args);
   yURG_urgs   (2, x_args);
   return 0;
}

char       /*----: set up programgents/debugging -----------------------------*/
yRPN__testloud      (void)
{
   char       *x_args [3]  = { "yRPN_debug","@@kitchen","@@YRPN" };
   yURG_logger (3, x_args);
   yURG_urgs   (3, x_args);
   return 0;
}

char       /*----: set up program urgents/debugging --------------------------*/
yRPN__testend       (void)
{
   DEBUG_TOPS   yLOG_end     ();
   return 0;
}



/*============================----end-of-source---============================*/
