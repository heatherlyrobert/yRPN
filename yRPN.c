/*============================----beg-of-source---============================*/

#include  "yRPN.h"
#include  "yRPN_priv.h"


char      preproc     = 'n';

char     *v_alphanum  = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
char     *v_alpha     = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";
char     *v_lower     = "abcdefghijklmnopqrstuvwxyz";
char     *v_caps      = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char     *v_number    = "0123456789";
char     *v_hex       = "0123456789abcdefABCDEF";
char     *v_octal     = "01234567";
char     *v_binary    = "01";
char     *v_paren     = "(),[]";   
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

typedef   struct cPREC tPREC;
struct    cPREC {
   char      prec;
   char      token [5];
   char      dir;
   char      arity;                    /* how many arguments it takes         */
};

/*---(design notes)--------------------*/
/*
 *  how do i deal with prefix versus suffix ++ and -- ?
 *
 *  how to deal with unary +/- from normal +/- ?
 *
 *  how to deal with array subscripting ?
 *
 *  how to deal with casting widh (<type>) ?
 *
 *  how to deal with multiplication versus indirection with splat ?
 *
 *  how to deal with address-of and bitwise-and with ampersand ?
 *
 *  how to deal with trinary conditional ?
 *
 */
#define   MAX_PREC    200
tPREC     precedence [MAX_PREC] = {
   /*-prec------token----dir-arity-*/
   /*---(spreadsheet)------------*/
   { 'd' +  0,  ".."    , S_LEFT ,   2 },    /* cell range                        */
   /*---(preprocessor)-----------*/
   /*> { 'd' +  0,  "#"     , S_LEFT ,   2 },    /+ prefix                            +/   <*/
   /*> { 'd' +  0,  "##"    , S_LEFT ,   2 },    /+ stringification                   +/   <*/
   /*---(unary/prefix)-----------*/
   { 'd' +  1,  "+-"    , S_LEFT ,   1 },    /* suffix increment                  */
   { 'd' +  1,  "-+"    , S_LEFT ,   1 },    /* suffix decrement                  */
   /*---(element of)-------------*/
   { 'd' +  1,  "["     , S_LEFT ,   1 },    /* array subscripting                */
   { 'd' +  1,  "]"     , S_LEFT ,   1 },    /* array subscripting                */
   { 'd' +  1,  "."     , S_LEFT ,   2 },    /* element selection by reference    */
   { 'd' +  1,  "->"    , S_LEFT ,   2 },    /* element selection thru pointer    */
   /*---(unary/prefix)-----------*/
   { 'd' +  2,  "++"    , S_RIGHT,   1 },    /* prefix increment                  */
   { 'd' +  2,  "--"    , S_RIGHT,   1 },    /* prefix decrement                  */
   { 'd' +  2,  "+:"    , S_RIGHT,   1 },    /* unary plus                        */
   { 'd' +  2,  "-:"    , S_RIGHT,   1 },    /* unary minus                       */
   { 'd' +  2,  "!"     , S_RIGHT,   1 },    /* logical NOT                       */
   { 'd' +  2,  "~"     , S_RIGHT,   1 },    /* bitwise NOT                       */
   { 'd' +  2,  "*:"    , S_RIGHT,   1 },    /* indirection/dereference           */
   { 'd' +  2,  "&:"    , S_RIGHT,   1 },    /* address-of                        */
   /*---(multiplicative)---------*/
   { 'd' +  3,  "*"     , S_LEFT ,   2 },    /* multiplication                    */
   { 'd' +  3,  "/"     , S_LEFT ,   2 },    /* division                          */
   { 'd' +  3,  "%"     , S_LEFT ,   2 },    /* modulus                           */
   /*---(additive)---------------*/
   { 'd' +  4,  "+"     , S_LEFT ,   2 },    /* addition                          */
   { 'd' +  4,  "-"     , S_LEFT ,   2 },    /* substraction                      */
   { 'd' +  4,  "#"     , S_LEFT ,   2 },    /* string concatination              */
   { 'd' +  4,  "##"    , S_LEFT ,   2 },    /* string concatination              */
   /*---(shift)------------------*/
   { 'd' +  5,  "<<"    , S_LEFT ,   2 },    /* bitwise shift left                */
   { 'd' +  5,  ">>"    , S_LEFT ,   2 },    /* bitwise shift right               */
   /*---(relational)-------------*/
   { 'd' +  6,  "<"     , S_LEFT ,   2 },    /* relational lesser                 */
   { 'd' +  6,  "<="    , S_LEFT ,   2 },    /* relational less or equal          */
   { 'd' +  6,  ">"     , S_LEFT ,   2 },    /* relational greater                */
   { 'd' +  6,  ">="    , S_LEFT ,   2 },    /* relational more or equal          */
   { 'd' +  6,  "#<"    , S_LEFT ,   2 },    /* relational string lesser          */
   { 'd' +  6,  "#>"    , S_LEFT ,   2 },    /* relational string greater         */
   /*---(equality)---------------*/
   { 'd' +  7,  "=="    , S_LEFT ,   2 },    /* relational equality               */
   { 'd' +  7,  "!="    , S_LEFT ,   2 },    /* relational inequality             */
   { 'd' +  7,  "#="    , S_LEFT ,   2 },    /* relational string equality        */
   { 'd' +  7,  "#!"    , S_LEFT ,   2 },    /* relational string inequality      */
   /*---(bitwise)----------------*/
   { 'd' +  8,  "&"     , S_LEFT ,   2 },    /* bitwise AND                       */
   { 'd' +  9,  "^"     , S_LEFT ,   2 },    /* bitwise XOR                       */
   { 'd' + 10,  "|"     , S_LEFT ,   2 },    /* bitwise OR                        */
   /*---(logical)----------------*/
   { 'd' + 11,  "&&"    , S_LEFT ,   2 },    /* logical AND                       */
   { 'd' + 12,  "||"    , S_LEFT ,   2 },    /* logical OR                        */
   /*---(conditional)------------*/
   { 'd' + 13,  "?"     , S_RIGHT,   2 },    /* trinary conditional               */
   { 'd' + 13,  ":"     , S_RIGHT,   2 },    /* trinary conditional               */
   /*---(assignment)-------------*/
   { 'd' + 14,  "="     , S_RIGHT,   2 },    /* direct assignment                 */
   { 'd' + 14,  "+="    , S_RIGHT,   2 },
   { 'd' + 14,  "-="    , S_RIGHT,   2 },
   { 'd' + 14,  "*="    , S_RIGHT,   2 },
   { 'd' + 14,  "/="    , S_RIGHT,   2 },
   { 'd' + 14,  "%="    , S_RIGHT,   2 },
   { 'd' + 14,  "<<="   , S_RIGHT,   2 },
   { 'd' + 14,  ">>="   , S_RIGHT,   2 },
   { 'd' + 14,  "&="    , S_RIGHT,   2 },
   { 'd' + 14,  "^="    , S_RIGHT,   2 },
   { 'd' + 14,  "|="    , S_RIGHT,   2 },
   /*---(comma)------------------*/
   { 'd' + 15,  ","     , S_LEFT ,   2 },    /* comma                             */
   /*---(parenthesis)------------*/
   { 'd' + 16,  "("     , S_LEFT ,   1 },
   { 'd' + 16,  ")"     , S_LEFT ,   1 },
   /*---(semicolon)--------------*/
   { 'd' + 17,  ";"     , S_LEFT ,   1 },
   /*---(braces)-----------------*/
   { 'd' + 18,  "{"     , S_LEFT ,   1 },
   { 'd' + 18,  "}"     , S_LEFT ,   1 },
   /*---(end)--------------------*/
   { '-'     ,  "end"   , '-',   0 },
};

#define     zBEG_PAREN     'd' + 16

#define   MAX_CONST   200
typedef   struct cCONST tCONST;
struct    cCONST {
   char      name  [30];
   double    value;
};
tCONST    constants [MAX_CONST] = {
   /*---(spreadsheet)------------*/
   { "TRUE"                   , 1.0                },
   { "FALSE"                  , 0.0                },
   { "NULL"                   , 0.0                },
   { "PI"                     , 3.1415927          },
   { "end-of-const"           , 0.0                },
};

#define   MAX_TYPES   200
typedef   struct cTYPES tTYPES;
struct    cTYPES {
   char      name  [30];
   char      usage;
};
tTYPES    ctypes [MAX_TYPES] = {
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
   { "end-of-types"           , '-'                },
};

#define   MAX_CKEYWORDS   200
typedef   struct cKEYWORDS tKEYWORDS;
struct    cKEYWORDS {
   char      name  [30];
   char      usage;
};
tKEYWORDS  s_keywords [MAX_CKEYWORDS] = {
   /*---(beg)--------------------*/
   { "beg-of-keywords"        , '-' },
   /*---(preprocessor)-----------*/
   { "include"                , '-' },
   { "define"                 , '-' },
   { "unfef"                  , '-' },
   { "ifdef"                  , '-' },
   { "ifndef"                 , '-' },
   { "elif"                   , '-' },
   { "endif"                  , '-' },
   { "line"                   , '-' },
   { "error"                  , '-' },
   { "pragma"                 , '-' },
   /*---(storage)----------------*/
   { "struct"                 , '-' },
   { "union"                  , '-' },
   { "typedef"                , '-' },
   { "enum"                   , '-' },
   { "sizeof"                 , '-' },
   /*---(control)----------------*/
   { "break"                  , '-' },
   { "case"                   , '-' },
   { "continue"               , '-' },
   { "default"                , '-' },
   { "do"                     , '-' },
   { "else"                   , '-' },
   { "for"                    , '-' },
   { "goto"                   , '-' },
   { "if"                     , '-' },
   { "return"                 , '-' },
   { "switch"                 , '-' },
   { "while"                  , '-' },
   /*---(reserving)--------------*/
   { "asm"                    , '-' },
   { "typeof"                 , '-' },
   /*---(end)--------------------*/
   { "end-of-keywords"        , '-' },
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
   for (i = 0; i < MAX_PREC; ++i) {
      if  (strcmp (precedence[i].token, "end"     ) == 0)  break;
      if  (strcmp (precedence[i].token, rpn.token ) != 0)  continue;
      rpn.prec  = precedence[i].prec;
      rpn.dir   = precedence[i].dir;
      rpn.arity = precedence[i].arity;
      return  0;
   }
   /*---(complete)----------------*/
   rpn.prec  = S_PREC_FAIL;
   return -1;
}

char       /* ---- : identify the symbol arity -------------------------------*/
yRPN_arity         (char *a_op)
{
   int       i         = 0;
   for (i = 0; i < MAX_PREC; ++i) {
      if  (strcmp (precedence[i].token, "end"     ) == 0)  break;
      if  (strcmp (precedence[i].token, a_op      ) != 0)  continue;
      return  precedence[i].arity;
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
   /*> if (rpn.type == S_TTYPE_GROUP)  return 0;                                                <*/
   /*> if (rpn.type == S_TTYPE_OPER && strcmp(rpn.token, ",") == 0)  return 0;                 <*/
   /*> printf ("found a comma, skipping  %c, %c, %s\n", zRPN_lang, rpn.type, rpn.token);    <* 
    *> if (zRPN_lang == S_LANG_GYGES && rpn.type == S_TTYPE_OPER && strcmp(rpn.token, ",") == 0)  return 0;   <* 
    *> printf ("just didn't skip\n");                                                       <*/
   sprintf (x_token, "%c,%04d,%s%s", rpn.type, a_pos, rpn.token, zRPN_divtech);
   strncat (rpn.normal, x_token, zRPN_MAX_LEN);
   ++rpn.nnormal;
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
   snprintf (rpn.stack[rpn.nstack], zRPN_MAX_LEN, "%c,%c,%s", rpn.type, rpn.prec, rpn.token);
   ++rpn.nstack;
   zRPN_DEBUG  printf("      RPN__push       :: (%3d) %s\n", rpn.nstack, rpn.stack [rpn.nstack - 1]);
   return 0;
}

char             /* [------] look at item on top ot stack --------------------*/
yRPN__peek         (void)
{
   /*---(handle empty stack)-------------*/
   if (rpn.nstack <= 0) {
      rpn.t_prec  = zRPN_END;
      strncpy (rpn.t_token, zRPN_NADA, 15);
      zRPN_DEBUG  printf("      RPN__peek       :: empty stack\n");
      return zRPN_ERR_EMPTY_STACK;
   }
   /*---(normal stack)-------------------*/
   else {
      rpn.t_type  = rpn.stack[rpn.nstack - 1][0];
      rpn.t_prec  = rpn.stack[rpn.nstack - 1][2];
      strncpy (rpn.t_token, rpn.stack[rpn.nstack - 1] + 4, zRPN_MAX_LEN);
      zRPN_DEBUG  printf("      RPN__peek       :: (%3d) %s\n", rpn.nstack, rpn.stack [rpn.nstack - 1]);
   }
   /*---(complete)-----------------------*/
   return 0;
}

char             /* [------] pop and toss the top of the stack ---------------*/
yRPN__toss         (void)
{
   /*---(handle empty stack)-------------*/
   if (rpn.nstack <= 0) {
      zRPN_DEBUG  printf("      RPN__toss       :: FATAL empty stack\n");
      return zRPN_ERR_EMPTY_STACK;
   }
   /*---(normal stack)-------------------*/
   else {
      zRPN_DEBUG  printf("      RPN__toss       :: (%3d) %s\n", rpn.nstack, rpn.stack [rpn.nstack - 1]);
      --rpn.nstack;
   }
   /*---(complete)-----------------------*/
   return 0;
}

char             /* [------] pop and save the top of the stack ---------------*/
yRPN__pops         (void)
{
   char      xprec;
   char      x_token[zRPN_MAX_LEN];
   /*---(handle empty stack)-------------*/
   if (rpn.nstack <= 0) {
      zRPN_DEBUG  printf("      RPN__pops       :: FATAL empty stack\n");
      return zRPN_ERR_EMPTY_STACK;
   }
   /*---(normal stack)-------------------*/
   else {
      zRPN_DEBUG  printf("      RPN__pops       :: (%3d) %s\n", rpn.nstack, rpn.stack [rpn.nstack - 1]);
      --rpn.nstack;
      ++rpn.count;
      sprintf (x_token, "%c,%s%s", rpn.stack[rpn.nstack][0], rpn.stack[rpn.nstack] + 4, zRPN_divtech);
      strncat (rpn.detail, x_token, zRPN_MAX_LEN);
      sprintf (x_token, "%s%s", rpn.stack[rpn.nstack] + 4, zRPN_divider);
      strncat (rpn.output, x_token, zRPN_MAX_LEN);
      xprec = rpn.stack[rpn.nstack][2];
   }
   return 0;
}

char             /* [------] save current item to final output ---------------*/
yRPN__save         (void)
{
   char      x_token [zRPN_MAX_LEN];
   ++rpn.count;
   sprintf (x_token, "%c,%s%s", rpn.type, rpn.token, zRPN_divtech);
   strncat (rpn.detail, x_token, zRPN_MAX_LEN);
   sprintf (x_token, "%s%s", rpn.token, zRPN_divider);
   strncat (rpn.output, x_token, zRPN_MAX_LEN);
   zRPN_DEBUG  printf("      RPN__save       :: (---) %c,%c,%s\n", rpn.type, rpn.prec, rpn.token);
   return 0;
}

char       /*----: save a token to the tokens output -------------------------*/
yRPN__token        (void)
{
   strncat (rpn.tokens, rpn.token   , zRPN_MAX_LEN);
   strncat (rpn.tokens, zRPN_divider, zRPN_MAX_LEN);
   zRPN_DEBUG  printf("      RPN__token      :: (---) %s\n", rpn.token);
   return 0;
}



/*====================------------------------------------====================*/
/*===----                       c language specific                    ----===*/
/*====================------------------------------------====================*/
static void        o___C_LANG__________________o (void) {;}

int        /* ---- : save off keywords ---------------------------------------*/
yRPN__keywords     (int  a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /* symbols are only lowercase alphanumerics plus underscore.               */
   /*---(locals)-------*-----------------*/
   char      rce       =  -10;         /* return code for errors              */
   int       x_pos     = a_pos;        /* updated position in input           */
   int       x_len     = 0;            /* keyword/token length                */
   int       i         = 0;            /* iterator for keywords               */
   int       x_key     = -1;           /* index of keyword                    */
   int       rc        = 0;
   /*---(defenses)-----------------------*/
   strncpy (rpn.token, zRPN_NADA, zRPN_MAX_LEN);
   rpn.type   = S_TTYPE_ERROR;
   rpn.ntoken = 0;
   --rce;  if (zRPN_lang == S_LANG_GYGES)                 return rce;
   --rce;  if (a_pos <  0)                                return rce;
   --rce;  if (a_pos >= rpn.nworking)                     return rce;
   --rce;  if (strchr(v_lower, rpn.working[a_pos]) == 0)  return rce;
   /*---(prepare)------------------------*/
   strncpy (rpn.token, zRPN_NADA, zRPN_MAX_LEN);
   rpn.type = S_TTYPE_CKEY;
   rpn.prec = S_PREC_NONE;
   zRPN_DEBUG  printf("   s_keywords--------------\n");
   rpn.ntoken = 0;
   /*---(accumulate characters)------------*/
   while (x_pos < rpn.nworking) {
      /*---(test for right characters)-----*/
      if (strchr(v_lower, rpn.working[x_pos]) == 0)  break;
      /*---(normal name)-------------------*/
      rpn.token[x_len]    = rpn.working[x_pos];
      rpn.token[++x_len]  = '\0';
      /*---(output)----------------------*/
      zRPN_DEBUG  printf("          (%02d) <<%s>>\n", x_len, rpn.token);
      /*---(prepare for next char)-------*/
      ++x_pos;
   }
   rpn.ntoken = x_len;
   /*---(try to match keyword)-------------*/
   for (i = 0; i < MAX_CKEYWORDS; ++i) {
      if  (strcmp (s_keywords [i].name, "end-of-keywords") == 0) break;
      if  (strcmp (s_keywords [i].name, rpn.token ) != 0)        continue;
      x_key = i;
      break;
   }
   /*---(handle misses)--------------------*/
   --rce;  if (x_key < 0) {
      strncpy (rpn.token, zRPN_NADA, zRPN_MAX_LEN);
      rpn.type   = S_TTYPE_ERROR;
      rpn.ntoken = 0;
      return rce;
   }
   /*---(mark includes)--------------------*/
   if (preproc == 'y') {
      if (strcmp (rpn.token, "include") == 0)  preproc = S_PPROC_INCL;
      else                                     preproc = 'o';
   }
   /*---(end)------------------------------*/
   zRPN_DEBUG  printf("      fin (%02d) <<%s>>\n", rpn.ntoken, rpn.token);
   yRPN__token ();
   yRPN__save  ();
   yRPN__normal (a_pos);
   rpn.lops = 'y';
   /*---(complete)-------------------------*/
   return x_pos;
}

int        /* ---- : save off type declarations ------------------------------*/
yRPN__types        (int  a_pos)
{
   /*---(design notes)-------------------*/
   /*
    *  symbols begin with a letter and contain alphanumerics plus underscore.
    *
    */
   /*---(locals)-------*-------------------*/
   char      rce       =  -10;
   int       i         = a_pos;
   int       j         = 0;
   char      post      = 'n';
   int       rc        = 0;
   /*---(defense: spreadsheet)-----------*/
   MODE_GYGES      return a_pos;
   /*---(prepare)------------------------*/
   strncpy (rpn.token, zRPN_NADA, zRPN_MAX_LEN);
   rpn.type = S_TTYPE_TYPE;
   rpn.prec = S_PREC_NONE;
   /*---(defenses)-----------------------*/
   --rce;  if (i >= rpn.nworking)                     return rce;
   --rce;  if (strchr(v_lower, rpn.working[i]) == 0)  return rce;
   /*---(main loop)------------------------*/
   zRPN_DEBUG  printf("   ctypes------------------\n");
   rpn.ntoken = 0;
   while (i < rpn.nworking) {
      /*---(test for right characters)-----*/
      if (rpn.working[i] == '_')                    return a_pos;
      if (strchr(v_lower   , rpn.working[i]) == 0)  break;
      /*---(normal name)-------------------*/
      rpn.token[j]     = rpn.working[i];
      rpn.token[j + 1] = '\0';
      ++rpn.ntoken;
      /*---(output)----------------------*/
      zRPN_DEBUG  printf("      %03d (%02d) <<%s>>\n", j, rpn.ntoken, rpn.token);
      /*---(prepare for next char)-------*/
      ++i;
      ++j;
   }
   for (j = 0; j < MAX_TYPES; ++j) {
      if  (strcmp (ctypes [j].name, "end-of-types") == 0) {
         strncpy (rpn.token, zRPN_NADA, zRPN_MAX_LEN);
         rpn.type = S_TTYPE_ERROR;
         rpn.ntoken = 0;
         return zRPN_ERR_UNKNOWN_TYPE;
      }
      if  (strcmp (ctypes [j].name, rpn.token ) == 0) break;
   }
   /*---(end)------------------------------*/
   zRPN_DEBUG  printf("      fin (%02d) <<%s>>\n", rpn.ntoken, rpn.token);
   yRPN__token ();
   yRPN__save  ();
   yRPN__normal (a_pos);
   rpn.lops = 'y';
   /*---(complete)-------------------------*/
   return i;
}



/*====================------------------------------------====================*/
/*===----                          token handling                      ----===*/
/*====================------------------------------------====================*/
static void        o___TOKENS__________________o (void) {;}

int        /* ---- : save off constants --------------------------------------*/
yRPN__constants    (int  a_pos)
{
   /*---(design notes)-------------------*/
   /*
    *  symbols begin with a letter and contain alphanumerics plus underscore.
    *
    */
   /*---(locals)-------*-------------------*/
   int       i         = a_pos;
   int       j         = 0;
   char      post      = 'n';
   int       rc        = 0;
   /*---(prepare)------------------------*/
   strncpy (rpn.token, zRPN_NADA, zRPN_MAX_LEN);
   rpn.type = S_TTYPE_CONST;
   rpn.prec = S_PREC_NONE;
   /*---(defenses)-----------------------*/
   if (zRPN_lang      != S_LANG_GYGES)        return  zRPN_ERR_NOT_SPREADSHEET;
   if (i              >= rpn.nworking)        return  zRPN_ERR_INPUT_NOT_AVAIL;
   if (strchr(v_alpha, rpn.working[i]) == 0)  return  zRPN_ERR_NOT_START_ALPHA;
   /*---(main loop)------------------------*/
   zRPN_DEBUG  printf("   constant---------------\n");
   rpn.ntoken = 0;
   while (i < rpn.nworking) {
      /*---(test for right characters)-----*/
      if (strchr(v_alphanum, rpn.working[i]) == 0)  break;
      /*---(normal name)-------------------*/
      rpn.token[j]     = rpn.working[i];
      rpn.token[j + 1] = '\0';
      ++rpn.ntoken;
      /*---(output)----------------------*/
      zRPN_DEBUG  printf("      %03d (%02d) <<%s>>\n", j, rpn.ntoken, rpn.token);
      /*---(prepare for next char)-------*/
      ++i;
      ++j;
   }
   for (j = 0; j < MAX_CONST; ++j) {
      if  (strcmp (constants [j].name, "end-of-const") == 0) {
         strncpy (rpn.token, zRPN_NADA, zRPN_MAX_LEN);
         rpn.ntoken = 0;
         return zRPN_ERR_UNKNOWN_CONSTANT;
      }
      if  (strcmp (constants [j].name, rpn.token ) == 0) break;
   }
   /*---(end)------------------------------*/
   zRPN_DEBUG  printf("      fin (%02d) <<%s>>\n", rpn.ntoken, rpn.token);
   yRPN__token ();
   yRPN__save  ();
   yRPN__normal (a_pos);
   rpn.lops = 'n';
   /*---(complete)-------------------------*/
   return i;
}

int        /* ---- : save off character literals -----------------------------*/
yRPN__chars        (int  a_pos)
{
   /*---(design notes)-------------------*/
   /*
    *  character literals begin and end with single quotes.
    *  they can not be nested, but escaped characters can appear inside.
    *
    */
   /*---(locals)-------------------------*/
   int       i         = a_pos;
   int       j         = 0;
   char      prev      = ' ';
   /*---(prepare)------------------------*/
   strncpy (rpn.token, zRPN_NADA, zRPN_MAX_LEN);
   rpn.type = S_TTYPE_CHAR;
   rpn.prec = S_PREC_NONE;
   /*---(defenses)-----------------------*/
   if (i              >= rpn.nworking)    return  zRPN_ERR_INPUT_NOT_AVAIL;
   if (rpn.working[i] != '\''        )    return  zRPN_ERR_NO_STARTING_QUOTE;
   /*---(main loop)----------------------*/
   zRPN_DEBUG  printf("   character--------------\n");
   rpn.ntoken = 0;
   while (i < rpn.nworking) {
      /*---(normal name)-----------------*/
      rpn.token[j]     = rpn.working[i];
      rpn.token[j + 1] = '\0';
      ++rpn.ntoken;
      /*---(output)----------------------*/
      zRPN_DEBUG  printf("      %03d (%02d) <<%s>>\n", j, rpn.ntoken, rpn.token);
      /*---(test for end)----------------*/
      if (rpn.working[i] == '\'' && j > 0 && prev != '\\')  break;
      /*---(prepare for next char)-------*/
      prev  = rpn.working[i];
      ++i;
      ++j;
   }
   /*---(check for long enough)----------*/
   if (i  != a_pos + 2) {
      strncpy (rpn.token, zRPN_NADA, zRPN_MAX_LEN);
      rpn.ntoken = 0;
      return  zRPN_ERR_LITERAL_TOO_SHORT;
   }
   /*---(check for final quote)----------*/
   if (rpn.working[i] != '\'') {
      strncpy (rpn.token, zRPN_NADA, zRPN_MAX_LEN);
      rpn.ntoken = 0;
      return  zRPN_ERR_NO_ENDING_QUOTE;
   }
   /*---(display)------------------------*/
   zRPN_DEBUG  printf("      fin (%02d) <<%s>>\n", rpn.ntoken, rpn.token);
   /*---(finish off)---------------------*/
   yRPN__token ();
   yRPN__save  ();
   yRPN__normal (a_pos);
   rpn.lops = 'n';
   /*---(complete)-----------------------*/
   return ++i;
}

int        /* ---- : save off string literals --------------------------------*/
yRPN__text         (int  a_pos)
{
   strcat (rpn.tokens, rpn.working + a_pos);
   strcat (rpn.tokens, " ");
   return strlen (rpn.working);
}

int        /* ---- : save off string literals --------------------------------*/
yRPN__strings      (int  a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /*  begin and end with double quotes, can escape quotes inside             */
   /*---(locals)-------------------------*/
   int       x_pos     = a_pos;
   int       x_len     = 0;
   int       j         = 0;
   char      prev      = ' ';
   /*---(defenses)-----------------------*/
   if (a_pos >= rpn.nworking)                                   return  zRPN_ERR_INPUT_NOT_AVAIL;
   if (rpn.working[a_pos] != '\"' && rpn.working[a_pos] != '<') return  zRPN_ERR_NO_STARTING_QUOTE;
   /*---(prepare)------------------------*/
   strncpy (rpn.token, zRPN_NADA, zRPN_MAX_LEN);
   rpn.type = S_TTYPE_STR;
   rpn.prec = S_PREC_NONE;
   rpn.ntoken = 0;
   zRPN_DEBUG  printf("   string-----------------\n");
   /*---(accumulate characters)------------*/
   zRPN_DEBUG  printf("      start             (%02d) <<%s>>\n", rpn.ntoken, rpn.token);
   while (x_pos < rpn.nworking) {
      zRPN_DEBUG  printf("      %02d) testing %c", j, rpn.working[x_pos]);
      /*---(normal name)-----------------*/
      rpn.token[x_len]    = rpn.working[x_pos];
      rpn.token[++x_len]  = '\0';
      ++rpn.ntoken;
      /*---(output)----------------------*/
      zRPN_DEBUG  printf("          (%02d) <<%s>>\n", x_len, rpn.token);
      /*---(test for end)----------------*/
      if (rpn.working[x_pos] == '\"' && j > 0 && prev != '\\')  break;
      if (preproc == S_PPROC_INCL && rpn.working[x_pos] == '>')          break;
      /*---(prepare for next char)-------*/
      prev  = rpn.working[x_pos];
      ++x_pos;
      ++j;
   }
   rpn.ntoken = x_pos;
   /*---(check for long enough)----------*/
   if (x_pos  <= a_pos + 1) {
      strncpy (rpn.token, zRPN_NADA, zRPN_MAX_LEN);
      rpn.ntoken = 0;
      return  zRPN_ERR_LITERAL_TOO_SHORT;
   }
   /*---(check for final quote)----------*/
   if (rpn.working[x_pos] != '\"' && rpn.working[x_pos] != '>') {
      strncpy (rpn.token, zRPN_NADA, zRPN_MAX_LEN);
      rpn.ntoken = 0;
      return  zRPN_ERR_NO_ENDING_QUOTE;
   }
   /*---(display)------------------------*/
   zRPN_DEBUG  printf("      fin (%02d) <<%s>>\n", rpn.ntoken, rpn.token);
   /*---(finish off)---------------------*/
   yRPN__token ();
   yRPN__save  ();
   yRPN__normal (a_pos);
   rpn.lops = 'n';
   /*---(complete)-----------------------*/
   return ++x_pos;
}

int        /* ---- : save off symbol names -----------------------------------*/
yRPN__symbols      (int   a_pos)
{
   /*---(design notes)-------------------*/
   /*
    *  symbols begin with a letter and contain alphanumerics plus underscore.
    *
    */
   /*---(begin)--------------------------*/
   zRPN_DEBUG  printf("   symbol-----------------\n");
   /*---(locals)-------*-------------------*/
   int       i         = a_pos;
   int       j         = 0;
   char      post      = 'n';
   int       rc        = 0;
   /*---(prepare)------------------------*/
   strncpy (rpn.token, zRPN_NADA, zRPN_MAX_LEN);
   rpn.type = S_TTYPE_SYM;
   rpn.prec = S_PREC_NONE;
   /*---(defenses)-----------------------*/
   if (i              >= rpn.nworking)        return  zRPN_ERR_INPUT_NOT_AVAIL;
   if (strchr(v_alpha, rpn.working[i]) == 0)  return  zRPN_ERR_NOT_START_ALPHA;
   /*---(main loop)------------------------*/
   rpn.ntoken = 0;
   while (i < rpn.nworking) {
      /*---(clear whitespace)--------------*/
      if (rpn.working[i] == ' ')  {
         post = 'y';
         ++i;
         continue;
      }
      /*---(function marker)---------------*/
      if (rpn.working[i] == '(') {
         rpn.type = S_TTYPE_FUNC;
         rpn.prec = S_PREC_FUNC;
         yRPN__push();
         yRPN__normal (a_pos);
         break;
      }
      /*---(test for right characters)-----*/
      if (post == 'y')                              break;
      if (strchr(v_alphanum, rpn.working[i]) == 0)  break;
      /*---(normal name)-------------------*/
      rpn.token[j]     = rpn.working[i];
      rpn.token[j + 1] = '\0';
      ++rpn.ntoken;
      /*---(output)----------------------*/
      zRPN_DEBUG  printf("      %03d (%02d) <<%s>>\n", j, rpn.ntoken, rpn.token);
      /*---(prepare for next char)-------*/
      ++i;
      ++j;
   }
   /*---(end)------------------------------*/
   zRPN_DEBUG  printf("      fin (%02d) <<%s>>\n", rpn.ntoken, rpn.token);
   if (rpn.type == S_TTYPE_SYM)  {
      yRPN__save();
      yRPN__normal (a_pos);
   }
   yRPN__token ();
   rpn.lops = 'n';
   /*---(complete)-------------------------*/
   return i;
}

int        /* ---- : save off numbers ----------------------------------------*/
yRPN__numbers      (int  a_pos)
{
   /*---(design notes)-------------------*/
   /*
    *  numbers can be integer, float, scientific, octal, hex, or binary
    *
    */
   /*---(begin)--------------------------*/
   DEBUG_OPER  yLOG_enter   (__FUNCTION__);
   zRPN_DEBUG  printf("   number-----------------\n");
   /*---(locals)---------------------------*/
   int       i         = a_pos;
   int       j         = 0;
   int       points    = 0;
   /*---(prepare)------------------------*/
   strncpy (rpn.token, zRPN_NADA, zRPN_MAX_LEN);
   rpn.type   = S_TTYPE_ERROR;
   rpn.prec   = S_PREC_NONE;
   /*---(defenses)-----------------------*/
   if (i              >= rpn.nworking) {
      DEBUG_OPER  yLOG_exit    (__FUNCTION__);
      return  zRPN_ERR_INPUT_NOT_AVAIL;
   }
   if (strchr(v_number, rpn.working[i]) == 0) {
      DEBUG_OPER  yLOG_exit    (__FUNCTION__);
      return  zRPN_ERR_NOT_START_NUMBER;
   }
   if (rpn.working[i] == '.')   {
      DEBUG_OPER  yLOG_exit    (__FUNCTION__);
      return  zRPN_ERR_NOT_START_NUMBER;
   }
   /*---(main loop)------------------------*/
   rpn.type   = 'I';
   rpn.ntoken = 0;
   while (i < rpn.nworking) {
      /*---(test for right characters)-----*/
      if (rpn.working[i] == '.') {
         ++points;
         if (points > 1) {
            strncpy (rpn.token, zRPN_NADA, zRPN_MAX_LEN);
            rpn.type   = 'e';
            DEBUG_OPER  yLOG_exit    (__FUNCTION__);
            return  zRPN_ERR_MULTIPLE_DECIMALS;
         } else if (rpn.type == 'I') {
            rpn.type = 'F';
         } else {
            strncpy (rpn.token, zRPN_NADA, zRPN_MAX_LEN);
            rpn.type   = 'e';
            DEBUG_OPER  yLOG_exit    (__FUNCTION__);
            return  zRPN_ERR_DECIMALS_NOT_ALLOWED;
         }
      } else if (j == 1 && rpn.working[i] == 'x' && rpn.type == 'I') {
         zRPN_DEBUG  printf ("      switching to hex constant mode\n");
         rpn.type = 'X';
      } else if (j == 1 && rpn.working[i] == 'b' && rpn.type == 'I') {
         zRPN_DEBUG  printf ("      switching to binary constant mode\n");
         rpn.type = 'B';
      } else {
         if      (rpn.type == S_TTYPE_INT   && strchr(v_number  , rpn.working[i]) == 0)  break;
         else if (rpn.type == S_TTYPE_FLOAT && strchr(v_number  , rpn.working[i]) == 0)  break;
         else if (rpn.type == S_TTYPE_HEX   && strchr(v_hex     , rpn.working[i]) == 0)  break;
         else if (rpn.type == S_TTYPE_BIN   && strchr(v_binary  , rpn.working[i]) == 0)  break;
         else if (rpn.type == S_TTYPE_OCT   && strchr(v_octal   , rpn.working[i]) == 0)  break;
      }
      /*---(normal number)-----------------*/
      rpn.token[j]     = rpn.working[i];
      rpn.token[j + 1] = '\0';
      ++rpn.ntoken;
      /*---(output)----------------------*/
      zRPN_DEBUG  printf("      %03d (%02d) <<%s>>\n", j, rpn.ntoken, rpn.token);
      /*---(prepare for next char)-------*/
      ++i;
      ++j;
   }
   /*---(check for octal)------------------*/
   if (rpn.type == S_TTYPE_INT && rpn.ntoken > 1 && rpn.token [0] == '0') {
      zRPN_DEBUG  printf ("      recast as an octal onstant\n");
      rpn.type = S_TTYPE_OCT;
   }
   /*---(check for no payload)-------------*/
   if ((strchr (S_TTYPE_BINHEX, rpn.type) != 0) && rpn.ntoken < 3) {
      zRPN_DEBUG  printf ("      special format (0x or 0b) has no payload)\n");
      strncpy (rpn.token, zRPN_NADA, zRPN_MAX_LEN);
      rpn.type   = S_TTYPE_ERROR;
   }
   /*---(end)------------------------------*/
   zRPN_DEBUG  printf("      fin (%02d) <<%s>>\n", rpn.ntoken, rpn.token);
   yRPN__token ();
   yRPN__save  ();
   yRPN__normal (a_pos);
   rpn.lops = 'n';
   /*---(complete)-------------------------*/
   DEBUG_OPER  yLOG_exit    (__FUNCTION__);
   return i;
}

int        /* ---- : process operators ---------------------------------------*/
yRPN__operators    (int  a_pos)
{
   /*---(design notes)-------------------*/
   /*
    *  operators can have a wild number of forms and presidences
    *
    */
   /*---(begin)--------------------------*/
   DEBUG_OPER  yLOG_enter   (__FUNCTION__);
   DEBUG_OPER  yLOG_value   ("a_pos"     , a_pos);
   zRPN_DEBUG  printf("   operator---------------\n");
   /*---(locals)---------------------------*/
   int       i         = a_pos;
   int       j         = 0;
   int       rc        = 0;
   /*---(prepare)------------------------*/
   strncpy (rpn.token, zRPN_NADA, zRPN_MAX_LEN);
   rpn.type = S_TTYPE_OPER;
   rpn.prec = S_PREC_NONE;
   /*---(defenses)-----------------------*/
   DEBUG_OPER  yLOG_value   ("nworking"  , rpn.nworking);
   if (i              >= rpn.nworking) {
      DEBUG_OPER  yLOG_exit    (__FUNCTION__);
      return  zRPN_ERR_INPUT_NOT_AVAIL;
   }
   DEBUG_OPER  yLOG_char    ("first"     , rpn.working[i]);
   if (strchr(v_operator, rpn.working[i]) == 0) {
      DEBUG_OPER  yLOG_exit    (__FUNCTION__);
      return  zRPN_ERR_NOT_START_OPERATOR;
   }
   /*---(main loop)------------------------*/
   rpn.ntoken = 0;
   while (i < rpn.nworking && j < 2) {
      DEBUG_OPER  yLOG_complex ("current"   , "i=%3d, j=%3d, ch=%c", i, j, rpn.working[i]);
      /*---(test for right chars)-------------*/
      if (strchr(v_operator, rpn.working[i]) == 0)  break;
      if (zRPN_lang != S_LANG_GYGES && rpn.count == 0 && rpn.working[i] == '#')   preproc = 'y';
      /*---(add to token)------------------*/
      rpn.token[j]     = rpn.working[i];
      rpn.token[j + 1] = '\0';
      ++rpn.ntoken;
      DEBUG_OPER  yLOG_complex ("token"     , "len=%3d, str=%s", rpn.ntoken, rpn.token);
      /*---(check for too long)---------------*/
      yRPN__precedence ();
      if (j > 0 && rpn.prec == S_PREC_FAIL) {
         rpn.token[j]     = '\0';
         --rpn.ntoken;
         --j;
         --i;
         break;
      }
      /*---(output)----------------------*/
      zRPN_DEBUG  printf("      %03d (%02d) <<%s>>\n", j, rpn.ntoken, rpn.token);
      /*---(prepare for next char)-------*/
      ++i;
      ++j;
   }
   /*---(check size)-----------------------*/
   if (rpn.ntoken <= 0) {
      DEBUG_OPER  yLOG_exit    (__FUNCTION__);
      return  zRPN_ERR_ZERO_LEN_TOKEN;
   }
   if (rpn.ntoken >  3) {
      DEBUG_OPER  yLOG_exit    (__FUNCTION__);
      return  zRPN_ERR_OPERATOR_TOO_LONG;
   }
   /*---(check for unary)------------------*/
   if (rpn.lops == 'y') {
      if        (strcmp (rpn.token, "+") == 0)  { strcpy (rpn.token, "+:"); rpn.ntoken = 2;
      } else if (strcmp (rpn.token, "-") == 0)  { strcpy (rpn.token, "-:"); rpn.ntoken = 2;
      } else if (strcmp (rpn.token, "*") == 0)  { strcpy (rpn.token, "*:"); rpn.ntoken = 2;
      } else if (strcmp (rpn.token, "&") == 0)  { strcpy (rpn.token, "&:"); rpn.ntoken = 2;
      }
   }
   /*---(end)------------------------------*/
   DEBUG_OPER  yLOG_complex ("final"     , "str=%-3.3s, en=%3d, dir=%c, arity=%d, prec=%c", rpn.token, rpn.ntoken, rpn.dir, rpn.arity, rpn.prec);
   zRPN_DEBUG  printf("      fin (%02d) <<%s>>\n", rpn.ntoken, rpn.token);
   yRPN__token ();
   /*---(handle it)------------------------*/
   yRPN__precedence ();
   yRPN__peek ();
   DEBUG_OPER  yLOG_complex ("prec"      , "curr=%c, stack=%c", rpn.prec, rpn.t_prec);
   zRPN_DEBUG  printf("      precedence %c versus stack top of %c\n", rpn.prec, rpn.t_prec);
   if ( (rpn.dir == S_LEFT && rpn.prec >= rpn.t_prec) ||
         (rpn.dir == S_RIGHT && rpn.prec >  rpn.t_prec)) {
      while ((rpn.dir == S_LEFT && rpn.prec >= rpn.t_prec) ||
            (rpn.dir == S_RIGHT && rpn.prec >  rpn.t_prec)) {
         /*> if (rpn__last != 'z') RPN__pops();                                       <*/
         if (rpn.t_prec == 'z') break;
         yRPN__pops();
         yRPN__peek();
      }
      yRPN__push();
      yRPN__normal (a_pos);
   } else {
      yRPN__push();
      yRPN__normal (a_pos);
   }
   /*---(complete)-------------------------*/
   rpn.lops = 'y';
   DEBUG_OPER  yLOG_exit    (__FUNCTION__);
   return i;
}

int        /* ---- : process grouping ----------------------------------------*/
yRPN__grouping     (int  a_pos)
{
   /*---(design notes)-------------------*/
   /*
    *  operators can have a wild number of forms and presidences
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
   strncpy (rpn.token, zRPN_NADA, zRPN_MAX_LEN);
   rpn.token[0] = rpn.working[i];
   rpn.token[1] = '\0';
   rpn.type     = S_TTYPE_GROUP;
   rpn.prec     = S_PREC_NONE;
   /*---(defenses)-----------------------*/
   if (i              >= rpn.nworking)           return  zRPN_ERR_INPUT_NOT_AVAIL;
   if (strchr(v_paren   , rpn.working[i]) == 0)  return  zRPN_ERR_NOT_GROUPING;
   /*---(main loop)------------------------*/
   rpn.ntoken = 1;
   ++i;
   /*---(end)------------------------------*/
   zRPN_DEBUG  printf("      fin (%02d) <<%s>>\n", rpn.ntoken, rpn.token);
   /*---(handle it)------------------------*/
   yRPN__precedence ();
   zRPN_DEBUG  printf("      prec = %c\n", rpn.prec);
   /*---(open bracket)---------------------*/
   if (rpn.token[0] == '[') {
      yRPN__token ();
      /*> strcpy (rpn.token, "]*");                                                   <*/
      strcpy (rpn.token, "]");
      rpn.type         = S_TTYPE_OPER;
      yRPN__push();
      strcpy (rpn.token, "[");
      yRPN__normal (a_pos);
      x_fake = 'y';
      strcpy (rpn.token, "(");
      rpn.type         = S_TTYPE_GROUP;
      yRPN__precedence ();
   }
   /*---(open paren)-----------------------*/
   if (rpn.token[0] == '(') {
      yRPN__token ();
      yRPN__push();
      if (x_fake == 'n')  yRPN__normal (a_pos);
      rpn.lops = 'y';
   }
   /*---(close bracket)-------------------*/
   if (rpn.token[0] == ']') {
      yRPN__token ();
      strcpy (rpn.token, ")");
      rpn.type         = S_TTYPE_GROUP;
      yRPN__precedence ();
      x_fake = 'y';
   }
   /*---(close paren)----------------------*/
   if (rpn.token[0] == ')') {
      yRPN__token ();
      if (x_fake == 'n')  yRPN__normal (a_pos);
      rc = yRPN__peek();
      while (rc >= 0  &&  rpn.t_prec != 'd' + 16) {
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
      rpn.lops = 'n';
   }
   /*---(comma)----------------------------*/
   else if (strncmp(rpn.token, ",", 1) == 0) {
      yRPN__token ();
      rc = yRPN__peek();
      while (rc >= 0  &&  rpn.t_prec != 'd' + 16) {
         yRPN__pops();
         rc = yRPN__peek();
      }
      if (rc < 0) {
         /*> zRPN_DEBUG  printf ("      FATAL :: nothing more on stack\n");           <*/
         /*> return rc;                                                               <*/
      }
      if (zRPN_lang != S_LANG_GYGES) {
         rpn.type = S_TTYPE_OPER;
         yRPN__save ();
         yRPN__normal (a_pos);
         rpn.lops = 'y';
      }
   }
   /*---(complete)-------------------------*/
   return i;
}

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
   strncpy (rpn.token, zRPN_NADA, zRPN_MAX_LEN);
   rpn.type = S_TTYPE_ADDR;
   rpn.prec = S_PREC_NONE;
   /*---(defenses)-----------------------*/
   zRPN_DEBUG  printf("entering defenses");
   if (zRPN_lang       != S_LANG_GYGES)         return  zRPN_ERR_NOT_SPREADSHEET;
   if (i               >= rpn.nworking)         return  zRPN_ERR_INPUT_NOT_AVAIL;
   if (strchr(v_address, rpn.working[i]) == 0)  return  zRPN_ERR_NOT_ADDRESS_CHAR;
   /*---(main loop)------------------------*/
   zRPN_DEBUG  printf("passed defenses");
   zRPN_DEBUG  printf("   address----------------\n");
   rpn.ntoken = 0;
   while (i < rpn.nworking) {
      /*---(test for right characters)-----*/
      if (strchr(v_address , rpn.working[i]) == 0)  break;
      /*---(normal number)-----------------*/
      rpn.token[j]     = rpn.working[i];
      rpn.token[j + 1] = '\0';
      ++rpn.ntoken;
      /*---(output)----------------------*/
      zRPN_DEBUG  printf("      %03d (%02d) <<%s>>\n", j, rpn.ntoken, rpn.token);
      /*---(prepare for next char)-------*/
      ++i;
      ++j;
   }
   /*---(check size)-----------------------*/
   len = strlen(rpn.token);
   if (len <= 0) return -1;
   /*---(handle it)------------------------*/
   rc = yRPN__cells (rpn.token, NULL, NULL, NULL, NULL);
   zRPN_DEBUG  printf("\n");
   if (rc >= 0)  {
      strcpy  (rpn.token, s_addr);
   } else if (rc < 0)  {
      strcpy  (rpn.token, zRPN_NADA);
      rpn.ntoken = 0;
      return  zRPN_ERR_BAD_ADDRESS;
   }
   yRPN__token ();
   yRPN__save();
   yRPN__normal (a_pos);
   /*---(end)------------------------------*/
   zRPN_DEBUG  printf("      fin (%02d) <<%s>>\n", rpn.ntoken, rpn.token);
   rpn.lops = 'n';
   /*---(complete)-------------------------*/
   zRPN_DEBUG  printf("exiting yRPN_addresses");
   return i;
}


/*====================------------------------------------====================*/
/*===----                            driver                            ----===*/
/*====================------------------------------------====================*/
static void        o___DRIVER__________________o (void) {;}

int          /*--> prepare variables for rpn conversion --[ ------ [ ------ ]-*/
yRPN__load         (
      /*----------+-----------+-----------------------------------------------*/
      char       *a_source)   /* source infix string                          */
{
   /*---(locals)-----------+-----------+-*/
   int         i           = 0;            /* loop iterator -- source string      */
   int         pos         = 0;            /* position in new string              */
   char        last        = '-';
   char        dquote      = 'n';
   char        squote      = 'n';
   char        identify    = 'n';
   /*---(output header)------------------*/
   zRPN_DEBUG  printf("   ---prepare-------------\n");
   /*---(save the input)-----------------*/
   strncpy (rpn.source  , a_source , zRPN_MAX_LEN);
   rpn.nsource  = strlen (rpn.source);
   if (rpn.source[0]  == '\0') strncpy (rpn.source  , zRPN_NADA, zRPN_MAX_LEN);
   zRPN_DEBUG  printf("   source  (%02d) <<%s>>\n", rpn.nsource , rpn.source );
   /*---(set the working string)---------*/
   strncpy (rpn.working , rpn.source , zRPN_MAX_LEN);
   rpn.nworking = rpn.nsource;
   zRPN_DEBUG  printf("   working (%02d) <<%s>>\n", rpn.nworking, rpn.working);
   /*---(set the output string)----------*/
   strncpy (rpn.output  , ""       , zRPN_MAX_LEN);
   strncpy (rpn.detail  , ""       , zRPN_MAX_LEN);
   strncpy (rpn.normal  , ""       , zRPN_MAX_LEN);
   strncpy (rpn.tokens  , ""       , zRPN_MAX_LEN);
   rpn.noutput  = 0;
   zRPN_DEBUG  printf("   output  (%02d) <<%s>>\n", rpn.noutput , rpn.output );
   /*---(set the working vars)-----------*/
   rpn.nnormal  = 0;
   rpn.type     = S_TTYPE_ERROR;
   rpn.nstack   = 0;
   rpn.depth    = 0;
   rpn.count    = 0;
   rpn.lops     = 'y';
   rpn.cdepth   = 0;
   rpn.mdepth   = 0;
   /*---(complete)-----------------------*/
   return  rpn.nsource;
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
   *a_ntoken = rpn.count - 1;
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
   if (a_ntoken != NULL) *a_ntoken = rpn.nnormal;
   return rpn.output;
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
   if (a_ntoken != NULL) *a_ntoken = rpn.count;
   return rpn.output;
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
   int       cdepth    = 0;            /* current depth                       */
   int       xdepth    = 0;            /* maximum depth                       */
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
      if (p[0] == 'o')  cdepth -= (yRPN_arity (p + 2) - 1);
      else              ++cdepth;
      if (cdepth > xdepth)  xdepth = cdepth;
      printf ("token = %-10s, cdepth = %2d, xdepth = %2d\n", p, cdepth, xdepth);
      p = strtok_r (NULL, zRPN_divider, &s);
   }
   /*> yLOG_note   ("after loop");                                                    <*/
   rpn.depth = xdepth;
   /*---(complete)-----------------------*/
   /*> yLOG_exit   ();                                                                <*/
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
   int       i         = 1;
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
   i = 0;
   preproc = 'n';
   while (i < rpn.nworking) {
      /*---(pick handler)----------------*/
      rc = i;
      x_ch = rpn.working [i];
      if (rc <= i && x_ch == '\"')                            rc = yRPN__strings    (i);
      if (rc <= i && x_ch == '\'')                            rc = yRPN__chars      (i);
      if (rc <= i && x_ch == '<' && preproc == S_PPROC_INCL)  rc = yRPN__strings    (i);
      if (rc <= i &&                preproc == S_PPROC_OTHER) rc = yRPN__text       (i);
      if (rc <= i && strchr(v_address  , x_ch) != 0)          rc = yRPN__addresses  (i);
      if (rc <= i && strchr(v_lower    , x_ch) != 0)          rc = yRPN__keywords   (i);
      if (rc <= i && strchr(v_lower    , x_ch) != 0)          rc = yRPN__types      (i);
      if (rc <= i && strchr(v_alpha    , x_ch) != 0)          rc = yRPN__constants  (i);
      if (rc <= i && strchr(v_alpha    , x_ch) != 0)          rc = yRPN__symbols    (i);
      if (rc <= i && strchr(v_number   , x_ch) != 0)          rc = yRPN__numbers    (i);
      if (rc <= i && strchr(v_paren    , x_ch) != 0)          rc = yRPN__grouping   (i);
      if (rc <= i && strchr(v_operator , x_ch) != 0)          rc = yRPN__operators  (i);
      /*---(unrecognized)----------------*/
      if (rc <= i) {
         if (x_ch == ' ')  zRPN_DEBUG  printf ("   whitespace\n");
         ++i;
         continue;
      }
      /*---(output)----------------------*/
      zRPN_DEBUG  printf("   shunted = <<%s>>\n", rpn.output);
      zRPN_DEBUG  printf("   detail  = <<%s>>\n", rpn.detail);
      zRPN_DEBUG  printf("   normal  = <<%s>>\n", rpn.normal);
      zRPN_DEBUG  printf("   tokens  = <<%s>>\n", rpn.tokens);
      /*---(error handling)--------------*/
      if (rc < 0)  break;
      /*---(next)------------------------*/
      i = rc;
   }
   /*---(handle errors)------------------*/
   if (rc < 0) {
      zRPN_DEBUG  printf ("FATAL %4d : %s\n", rc, zRPN_ERRORS [ -rc - 100]);
      rpn.noutput = 0;
      rpn.count   = 0;
      strcpy (rpn.output, zRPN_NADA);
      /*> return  strndup (rpn.output, zRPN_MAX_LEN);                                 <*/
      /*> return  rpn.output;                                                         <*/
      DEBUG_TOPS  yLOG_exit    (__FUNCTION__);
      return NULL;
   }
   /*---(clear stack)--------------------*/
   zRPN_DEBUG  printf("   ---clear stack---------\n");
   while (rpn.nstack > 0) {
      yRPN__peek();
      if (strcmp(rpn.t_token, "(") == 0) {
         zRPN_DEBUG  printf ("FATAL %4d : %s\n", zRPN_ERR_UNBALANCED_PARENS, "unbalanced parentheses\n");
         rpn.noutput = 0;
         rpn.count   = 0;
         strcpy (rpn.output, zRPN_NADA);
         /*> return  strndup (rpn.output, zRPN_MAX_LEN);                              <*/
         /*> return  rpn.output;                                                      <*/
         DEBUG_TOPS  yLOG_exit    (__FUNCTION__);
         return NULL;
      }
      yRPN__pops();
   }
   zRPN_DEBUG  printf("      done\n");
   /*---(drop the trailing space)--------*/
   len = strlen(rpn.output) - strlen(zRPN_divider);
   rpn.output[len] = '\0';
   zRPN_DEBUG  printf("      done 2\n");
   len = strlen(rpn.detail) - strlen(zRPN_divtech);
   rpn.detail[len] = '\0';
   len = strlen(rpn.normal) - strlen(zRPN_divtech);
   rpn.normal[len] = '\0';
   zRPN_DEBUG  printf("      done 3\n");
   /*> yRPN__depth();                                                                 <*/
   zRPN_DEBUG  printf("      done 4\n");
   len = strlen(rpn.tokens) - 1;
   rpn.tokens[len] = '\0';
   zRPN_DEBUG  printf("      done 5\n");
   /*---(output)-------------------------*/
   zRPN_DEBUG  printf("   shunted = <<%s>>\n", rpn.output);
   zRPN_DEBUG  printf("   detail  = <<%s>>\n", rpn.detail);
   zRPN_DEBUG  printf("   normal  = <<%s>>\n", rpn.normal);
   zRPN_DEBUG  printf("   tokens  = <<%s>>\n", rpn.tokens);
   zRPN_DEBUG  printf("   depth   = <<%d>>\n", rpn.depth);
   DEBUG_TOPS  yLOG_info    ("shunted"   , rpn.output);
   DEBUG_TOPS  yLOG_info    ("detail"    , rpn.detail);
   DEBUG_TOPS  yLOG_info    ("normal"    , rpn.normal);
   DEBUG_TOPS  yLOG_info    ("tokens"    , rpn.tokens);
   DEBUG_TOPS  yLOG_value   ("depth"     , rpn.depth);
   /*---(complete)-----------------------*/
   zRPN_DEBUG  printf("RPN_convert     :: end ------------------------------\n");
   DEBUG_TOPS  yLOG_exit    (__FUNCTION__);
   /*> return  strndup (rpn.output, zRPN_MAX_LEN);                                    <*/
   return  rpn.output;
}


/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

#define       LEN_TEXT  2000
char          unit_answer [ LEN_TEXT ];

char*      /* ---- : answer unit testing gray-box questions ------------------*/
yRPN_accessor      (char *a_question, int a_item)
{
   /*---(sheet focus)--------------------*/
   if        (strcmp (a_question, "source"    )     == 0) {
      snprintf (unit_answer, LEN_TEXT, "source string    : %.50s", rpn.source);
   } else if (strcmp (a_question, "token"     )     == 0) {
      snprintf (unit_answer, LEN_TEXT, "current token    : %.50s", rpn.token);
   } else if (strcmp (a_question, "precedence")     == 0) {
      snprintf (unit_answer, LEN_TEXT, "precedence       : %c"   , rpn.prec);
   } else if (strcmp (a_question, "type"      )     == 0) {
      snprintf (unit_answer, LEN_TEXT, "current type     : %c"   , rpn.type);
   } else if (strcmp (a_question, "count"     )     == 0) {
      snprintf (unit_answer, LEN_TEXT, "token count      : %d"   , rpn.count);
   } else if (strcmp (a_question, "output"    )     == 0) {
      snprintf (unit_answer, LEN_TEXT, "postfix output   : %.50s", rpn.output);
   } else if (strcmp (a_question, "detail"    )     == 0) {
      snprintf (unit_answer, LEN_TEXT, "postfix detail   : %.50s", rpn.detail);
   } else if (strcmp (a_question, "tokens"    )     == 0) {
      snprintf (unit_answer, LEN_TEXT, "tokenization     : %.50s", rpn.tokens);
   } else if (strcmp (a_question, "depth"     )     == 0) {
      snprintf (unit_answer, LEN_TEXT, "stack depth      : %d"   , rpn.depth);
   } else if (strcmp (a_question, "address"   )     == 0) {
      snprintf (unit_answer, LEN_TEXT, "yRPN address     : tab=%4d, col=%4d, row=%4d, abs=%1d", v_tab, v_col, v_row, v_abs);
   }
   /*---(UNKNOWN)------------------------*/
   else {
      snprintf(unit_answer, LEN_TEXT, "UNKNOWN          : question is not understood");
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}


/*============================----end-of-source---============================*/
