/*============================----beg-of-source---============================*/

#include  "yRPN.h"
#include  "yRPN_priv.h"



char     *v_alphanum  = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_";
char     *v_alpha     = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_";
char     *v_upper     = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char     *v_upnum     = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
char     *v_lower     = "abcdefghijklmnopqrstuvwxyz";
char     *v_number    = "0123456789";
char     *v_float     = "0123456789.";
char     *v_hex       = "0123456789abcdefABCDEF";
char     *v_octal     = "o01234567";
char     *v_binary    = "01";
char     *v_sequence  = "(),[]";   
char     *v_enders    = ";{";
char     *v_operator  = "|&=!<>*/%+-.?:^~#";
/*> char     *v_preproc   = "#";                                                      <*/
char     *v_preproc   = "";
char     *v_address   = "@$abcdefghijklmnopqrstuvwxyz0123456789";




/*===[[ OPERATORS ]]==========================================================*/
typedef   struct cOPER tOPER;
struct    cOPER {
   char        name        [5];        /* operator                            */
   char        who;                    /* who can use it c, gyges, both       */
   char        real;                   /* real/used, internal, or off         */
   char        offset;                 /* offset for precedence               */
   char        prec;                   /* percedence                          */
   char        dir;                    /* direction of evaluation             */
   char        arity;                  /* how many arguments it takes         */
   char        desc        [LEN_DESC]; /* helpful description vs comment      */
};
#define   MAX_OPER    200
tOPER     s_opers [MAX_OPER] = {
   /*-sym----who--real---prec-----dir---arity---comment-----------------------------*/
   /*---(spreadsheet)------------*/
   { ".."  , 'g', 'r',  0, 'd',  S_LEFT , 2, "cell range"                           },
   /*---(preprocessor)-----------*/
   { "#"   , 'c', 'r',  0, 'd',  S_LEFT , 2, "prefix"                               },
   { "##"  , 'c', '-',  0, 'd',  S_LEFT , 2, "stringification"                      },
   /*---(unary/suffix)-----------*/
   { ":+"  , 'B', 'I',  1, 'e',  S_LEFT , 1, "suffix increment"                     },
   { ":-"  , 'B', 'I',  1, 'e',  S_LEFT , 1, "suffix decrement"                     },
   /*---(element of)-------------*/
   { "["   , 'c', 'r',  1, 'e',  S_LEFT , 1, "array subscripting"                   },
   { "]"   , 'c', 'r',  1, 'e',  S_LEFT , 1, "array subscripting"                   },
   { "."   , 'c', 'r',  1, 'e',  S_LEFT , 2, "element selection by reference"       },
   { "->"  , 'c', 'r',  1, 'e',  S_LEFT , 2, "element selection thru pointer"       },
   /*---(unary/prefix)-----------*/
   { "++"  , 'B', 'r',  2, 'f',  S_RIGHT, 1, "prefix increment"                     },
   { "--"  , 'B', 'r',  2, 'f',  S_RIGHT, 1, "prefix decrement"                     },
   { "+:"  , 'B', 'I',  2, 'f',  S_RIGHT, 1, "unary plus"                           },
   { "-:"  , 'B', 'I',  2, 'f',  S_RIGHT, 1, "unary minus"                          },
   { "!"   , 'B', 'r',  2, 'f',  S_RIGHT, 1, "logical NOT"                          },
   { "~"   , 'B', 'r',  2, 'f',  S_RIGHT, 1, "bitwise NOT"                          },
   { "*:"  , 'B', 'I',  2, 'f',  S_RIGHT, 1, "indirection/dereference"              },
   { "&:"  , 'B', 'I',  2, 'f',  S_RIGHT, 1, "address-of"                           },
   { "(*)" , 'c', 'I',  2, 'f',  S_RIGHT, 1, "casting modifier"                     },
   /*---(multiplicative)---------*/
   { "*"   , 'B', 'r',  3, 'g',  S_LEFT , 2, "multiplication"                       },
   { "/"   , 'B', 'r',  3, 'g',  S_LEFT , 2, "division"                             },
   { "%"   , 'B', 'r',  3, 'g',  S_LEFT , 2, "modulus"                              },
   /*---(additive)---------------*/
   { "+"   , 'B', 'r',  4, 'h',  S_LEFT , 2, "addition"                             },
   { "-"   , 'B', 'r',  4, 'h',  S_LEFT , 2, "substraction"                         },
   { "#"   , 'g', 'r',  4, 'h',  S_LEFT , 2, "string concatination"                 },
   { "##"  , 'g', 'r',  4, 'h',  S_LEFT , 2, "string concatination"                 },
   /*---(shift)------------------*/
   { "<<"  , 'B', 'r',  5, 'i',  S_LEFT , 2, "bitwise shift left"                   },
   { ">>"  , 'B', 'r',  5, 'i',  S_LEFT , 2, "bitwise shift right"                  },
   /*---(relational)-------------*/
   { "<"   , 'B', 'r',  6, 'j',  S_LEFT , 2, "relational lesser"                    },
   { "<="  , 'B', 'r',  6, 'j',  S_LEFT , 2, "relational less or equal"             },
   { ">"   , 'B', 'r',  6, 'j',  S_LEFT , 2, "relational greater"                   },
   { ">="  , 'B', 'r',  6, 'j',  S_LEFT , 2, "relational more or equal"             },
   { "#<"  , 'g', 'r',  6, 'j',  S_LEFT , 2, "relational string lesser"             },
   { "#>"  , 'g', 'r',  6, 'j',  S_LEFT , 2, "relational string greater"            },
   /*---(equality)---------------*/
   { "=="  , 'B', 'r',  7, 'k',  S_LEFT , 2, "relational equality"                  },
   { "!="  , 'B', 'r',  7, 'k',  S_LEFT , 2, "relational inequality"                },
   { "#="  , 'g', 'r',  7, 'k',  S_LEFT , 2, "relational string equality"           },
   { "#!"  , 'g', 'r',  7, 'k',  S_LEFT , 2, "relational string inequality"         },
   /*---(bitwise)----------------*/
   { "&"   , 'c', 'r',  8, 'l',  S_LEFT , 2, "bitwise AND"                          },
   { "^"   , 'c', 'r',  9, 'm',  S_LEFT , 2, "bitwise XOR"                          },
   { "|"   , 'c', 'r', 10, 'n',  S_LEFT , 2, "bitwise OR"                           },
   /*---(logical)----------------*/
   { "&&"  , 'B', 'r', 11, 'o',  S_LEFT , 2, "logical AND"                          },
   { "||"  , 'B', 'r', 12, 'p',  S_LEFT , 2, "logical OR"                           },
   /*---(conditional)------------*/
   { "?"   , 'c', 'r', 13, 'q',  S_RIGHT, 2, "trinary conditional"                  },
   { ":"   , 'c', 'r', 13, 'q',  S_RIGHT, 2, "trinary conditional"                  },
   /*---(assignment)-------------*/
   { "="   , 'B', 'r', 14, 'r',  S_RIGHT, 2, "direct assignment"                    },
   { "+="  , 'c', 'r', 14, 'r',  S_RIGHT, 2, "compound assignment"                  },
   { "-="  , 'c', 'r', 14, 'r',  S_RIGHT, 2, "compound assignment"                  },
   { "*="  , 'c', 'r', 14, 'r',  S_RIGHT, 2, "compound assignment"                  },
   { "/="  , 'c', 'r', 14, 'r',  S_RIGHT, 2, "compound assignment"                  },
   { "%="  , 'c', 'r', 14, 'r',  S_RIGHT, 2, "compound assignment"                  },
   { "<<=" , 'c', 'r', 14, 'r',  S_RIGHT, 2, "compound assignment"                  },
   { ">>=" , 'c', 'r', 14, 'r',  S_RIGHT, 2, "compound assignment"                  },
   { "&="  , 'c', 'r', 14, 'r',  S_RIGHT, 2, "compound assignment"                  },
   { "^="  , 'c', 'r', 14, 'r',  S_RIGHT, 2, "compound assignment"                  },
   { "|="  , 'c', 'r', 14, 'r',  S_RIGHT, 2, "compound assignment"                  },
   /*---(comma)------------------*/
   { ",;"  , 'c', 'r', 15, 'u',  S_LEFT , 1, "sequence mega-separator"              },
   { ","   , 'B', 'r', 15, 's',  S_LEFT , 2, "sequence separator"                   },
   /*---(parenthesis)------------*/
   { "("   , 'B', 'r', 16, 't',  S_LEFT , 1, "sequence openning"                    },
   { ")"   , 'B', 'r', 16, 't',  S_LEFT , 1, "sequence closing"                     },
   { "(>"  , 'c', 'I', 16, 't',  S_LEFT , 1, "function opening"                     },
   { "(:"  , 'c', 'I', 16, 't',  S_LEFT , 1, "casting opening"                      },
   { "):"  , 'c', 'I', 16, 't',  S_LEFT , 1, "casting closing"                      },
   /*---(semicolon)--------------*/
   { ";"   , 'c', 'r', 17, 'u',  S_LEFT , 1, "statement separator"                  },
   /*-------------(braces)-------*/
   { "{"   , 'c', 'r', 18, 'v',  S_LEFT , 1, "function openning"                    },
   { "}"   , 'c', 'r', 18, 'v',  S_LEFT , 1, "function closing"                     },
   /*---(end)--------------------*/
   { ""    , '-', '-',  0, '-',  '-'    , 0, ""                                     },
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
   { "sizeof"                 , 'f' },
   /*---(control)----------------*/
   { "break"                  , 's' },
   { "case"                   , 'f' },
   { "continue"               , 's' },
   { "default"                , 's' },
   { "do"                     , 'f' },
   { "else"                   , 's' },
   { "for"                    , 'f' },
   { "goto"                   , 'f' },
   { "if"                     , 'f' },
   { "return"                 , 'f' },
   { "switch"                 , 'f' },
   { "while"                  , 'f' },
   /*---(reserving)--------------*/
   { "asm"                    , 'r' },
   { "typeof"                 , 'f' },
   /*---(end)--------------------*/
   { ""                       , '-' },
};


int         v_tab       = 0;
int         v_col       = 0;
int         v_row       = 0;
int         v_abs       = 0;





char       /* ---- : identify the symbol precedence --------------------------*/
yRPN__prec   (void)
{
   int       i         = 0;
   for (i = 0; i < MAX_OPER; ++i) {
      if  (strcmp (s_opers[i].name, "end"     ) == 0)  break;
      if  (strcmp (s_opers[i].name, myRPN.t_name) != 0)  continue;
      myRPN.t_prec  = s_opers[i].prec;
      myRPN.t_dir   = s_opers[i].dir;
      myRPN.t_arity = s_opers[i].arity;
      return  0;
   }
   /*---(complete)----------------*/
   myRPN.t_prec  = S_PREC_FAIL;
   return -1;
}

char       /* ---- : identify the symbol precedence --------------------------*/
yRPN__p_prec       (void)
{
   int       i         = 0;
   for (i = 0; i < MAX_OPER; ++i) {
      if  (strcmp (s_opers[i].name, "end"     ) == 0)  break;
      if  (strcmp (s_opers[i].name, myRPN.p_name) != 0)  continue;
      myRPN.p_prec  = s_opers[i].prec;
      return  0;
   }
   /*---(complete)----------------*/
   myRPN.t_prec  = S_PREC_FAIL;
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
   int         x_use       =  '-';
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
   myRPN.t_type   = S_TTYPE_KEYW;
   myRPN.t_prec   = S_PREC_FUNC;
   x_pos        = a_pos;  /* starting point */
   while (yRPN__token_add (&x_pos) == 0);
   /*---(try to match keyword)-------------*/
   DEBUG_YRPN    yLOG_note    ("search keywords");
   for (i = 0; i < MAX_KEYWORDS; ++i) {
      if  (s_keywords [i].name [0] == '\0')                break;
      if  (s_keywords [i].name [0] != myRPN.t_name [0])      continue;
      if  (strcmp (s_keywords [i].name, myRPN.t_name ) != 0) continue;
      x_found = i;
      x_use = s_keywords [i].usage;
      DEBUG_YRPN    yLOG_value   ("x_found"   , x_found);
      DEBUG_YRPN    yLOG_char    ("x_use"     , x_use);
      break;
   }
   DEBUG_YRPN    yLOG_info    ("myRPN.t_name", myRPN.t_name);
   /*---(handle misses)--------------------*/
   --rce;  if (x_found < 0) {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("keyword not found");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(mark includes)--------------------*/
   DEBUG_YRPN    yLOG_note    ("check for preprocessor keywords");
   if (myRPN.pproc == S_PPROC_YES) {
      if (strcmp (myRPN.t_name, "include") == 0)  myRPN.pproc = S_PPROC_INCL;
      else                                      myRPN.pproc = S_PPROC_OTHER;
   }
   /*---(save)-----------------------------*/
   DEBUG_YRPN    yLOG_note    ("keyword is a function");
   yRPN_stack_tokens  ();         /* strait to tokens list                          */
   if (x_use == 'f')  yRPN__token_push    (a_pos);
   else               yRPN__token_save    (a_pos);
   myRPN.left_oper = S_OPER_LEFT;
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
   char        rc          =  -10;     /* generic return code                 */
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
   /*---(accumulate characters)----------*/
   DEBUG_YRPN    yLOG_note    ("accumulate characters");
   myRPN.t_type   = S_TTYPE_TYPE;
   x_pos        = a_pos;  /* starting point */
   while (yRPN__token_add (&x_pos) == 0);
   DEBUG_YRPN    yLOG_info    ("myRPN.t_name", myRPN.t_name);
   /*---(try to match types)-------------*/
   DEBUG_YRPN    yLOG_note    ("search types");
   for (i = 0; i < MAX_TYPES; ++i) {
      if  (s_types [i].name [0] == '\0')                   break;
      if  (s_types [i].name [0] != myRPN.t_name [0])         continue;
      if  (strcmp (s_types [i].name, myRPN.t_name ) != 0)    continue;
      x_found = i;
      DEBUG_YRPN    yLOG_value   ("x_found"   , x_found);
      break;
   }
   DEBUG_YRPN    yLOG_info    ("token name", myRPN.t_name);
   /*---(handle misses)------------------*/
   --rce;  if (x_found < 0) {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("type not found");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(definition vs casting)----------*/
   yRPN_stack_tokens  ();         /* strait to tokens list                          */
   if (myRPN.line_type == S_LINE_NORMAL) {
      DEBUG_YRPN    yLOG_note    ("treating as a casting onto stack");
      rc == yRPN_stack_peek ();
      DEBUG_YRPN    yLOG_char    ("myRPN.p_type", myRPN.p_type);
      switch (myRPN.p_type) {
      case S_TTYPE_FUNC :
         DEBUG_YRPN    yLOG_note    ("function parm type");
         yRPN__token_save    (a_pos);
         myRPN.left_oper  = S_OPER_LEFT;
         break;
      case S_TTYPE_FPTR :
         DEBUG_YRPN    yLOG_note    ("function pointer type");
         yRPN__token_save    (a_pos);
         myRPN.left_oper  = S_OPER_LEFT;
         break;
      case S_TTYPE_TYPE :
         DEBUG_YRPN    yLOG_note    ("normal type");
         yRPN__token_save    (a_pos);
         myRPN.left_oper  = S_OPER_LEFT;
         break;
      case S_TTYPE_CAST :
      default           :
         DEBUG_YRPN    yLOG_note    ("casting type");
         /*---(fix paren)----------------*/
         if (strcmp (myRPN.p_name, "(") == 0) {
            strlcpy (myRPN.p_name, "(:", S_LEN_LABEL);
            myRPN.p_type = S_TTYPE_CAST;
            myRPN.p_prec = S_PREC_FUNC;
            yRPN_stack_update ();
         }
         /*---(update current)-----------*/
         myRPN.t_type = S_TTYPE_CAST;
         myRPN.t_prec = S_PREC_FUNC;
         /*---(push)---------------------*/
         yRPN__token_push    (a_pos);
         myRPN.left_oper  = S_OPER_LEFT;
      }
   }
   /*---(save)---------------------------*/
   else if (myRPN.paren_lvl > 0) {
      DEBUG_YRPN    yLOG_note    ("put c type directly to output");
      myRPN.t_type = S_TTYPE_PTYPE;
      yRPN__token_save    (a_pos);
      myRPN.left_oper  = S_OPER_LEFT;
   }
   /*---(save)---------------------------*/
   else {
      DEBUG_YRPN    yLOG_note    ("put c type directly to output");
      yRPN__token_save    (a_pos);
      myRPN.left_oper  = S_OPER_LEFT;
   }
   /*---(complete)-----------------------*/
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
   myRPN.t_type   = S_TTYPE_STR;
   x_pos        = a_pos;  /* starting point */
   while (yRPN__token_add (&x_pos) == 0);
   DEBUG_YRPN    yLOG_info    ("myRPN.t_name", myRPN.t_name);
   /*---(check if long enough)-------------*/
   DEBUG_YRPN    yLOG_value   ("myRPN.t_len" , myRPN.t_len);
   --rce;  if (myRPN.t_len <  2) {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("string too short");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check matching quotes)------------*/
   x_last = myRPN.t_len - 1;
   --rce;  if (myRPN.t_name [0] == '\"' && myRPN.t_name [x_last] != '\"') {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("string does not end properly");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (myRPN.t_name [0] == '<' && myRPN.t_name [x_last] != '>') {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("include does not end properly");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)-----------------------------*/
   DEBUG_YRPN    yLOG_note    ("put string literal directly to output");
   yRPN_stack_tokens  ();         /* strait to tokens list                          */
   yRPN__token_save    (a_pos);
   myRPN.left_oper  = S_OPER_CLEAR;
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
   myRPN.t_type   = S_TTYPE_CHAR;
   x_pos        = a_pos;  /* starting point */
   while (yRPN__token_add (&x_pos) == 0);
   DEBUG_YRPN    yLOG_info    ("myRPN.t_name", myRPN.t_name);
   /*---(check if long enough)-------------*/
   DEBUG_YRPN    yLOG_value   ("myRPN.t_len" , myRPN.t_len);
   --rce;  if (myRPN.t_len <  3) {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("string too short");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (myRPN.t_len >  4) {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("string long short");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check start and stop)-------------*/
   x_last = myRPN.t_len - 1;
   --rce;  if (myRPN.t_name [0] != myRPN.t_name [x_last]) {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("char literal does not end properly");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (myRPN.t_len == 4 && myRPN.t_name [1] != '\\') {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("long char literal not escaped");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)-----------------------------*/
   DEBUG_YRPN    yLOG_note    ("put char literal directly to output");
   yRPN_stack_tokens  ();         /* strait to tokens list                          */
   yRPN__token_save    (a_pos);
   myRPN.left_oper  = S_OPER_CLEAR;
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
   --rce;  if (strchr (v_number, myRPN.working [a_pos]) == 0) {
      DEBUG_YRPN    yLOG_note    ("must start with a number");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN    yLOG_note    ("accumulate characters");
   x_pos        = a_pos;  /* starting point */
   myRPN.t_type   = yRPN__token_nums (x_pos);
   DEBUG_YRPN    yLOG_char    ("myRPN.t_type", myRPN.t_type);
   while (yRPN__token_add (&x_pos) == 0);
   DEBUG_YRPN    yLOG_info    ("myRPN.t_name", myRPN.t_name);
   /*---(check if long enough)-------------*/
   DEBUG_YRPN    yLOG_value   ("myRPN.t_len" , myRPN.t_len);
   switch (myRPN.t_type) {
   case S_TTYPE_INT  :
      if (myRPN.t_len < 1)  x_bad = 'y';
      break;
   case S_TTYPE_OCT  :
      if (myRPN.t_len < 2)  x_bad = 'y';
      break;
   default           :
      if (myRPN.t_len < 3)  x_bad = 'y';
      break;
   }
   --rce;  if (x_bad == 'y') {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("number too short");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check float decimals)-------------*/
   --rce;  if (myRPN.t_type == S_TTYPE_FLOAT) {
      rc = strldcnt (myRPN.t_name, '.', S_LEN_TOKEN);
      --rce;  if (rc != 1) {
         yRPN__token_error ();
         DEBUG_YRPN    yLOG_note    ("too many decimals");
         DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      --rce;  if (myRPN.t_name [myRPN.t_len - 1] == '.') {
         yRPN__token_error ();
         DEBUG_YRPN    yLOG_note    ("can not end with decimal");
         DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(save)-----------------------------*/
   DEBUG_YRPN    yLOG_note    ("put har literal directly to output");
   yRPN_stack_tokens  ();         /* strait to tokens list                          */
   yRPN__token_save    (a_pos);
   myRPN.left_oper  = S_OPER_CLEAR;
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
   myRPN.t_type   = S_TTYPE_CONST;
   x_pos        = a_pos;  /* starting point */
   while (yRPN__token_add (&x_pos) == 0);
   DEBUG_YRPN    yLOG_info    ("myRPN.t_name", myRPN.t_name);
   /*---(try to match constants)-----------*/
   DEBUG_YRPN    yLOG_note    ("search constants");
   for (i = 0; i < MAX_CONST; ++i) {
      if  (s_consts [i].name [0] == '\0')                   break;
      if  (s_consts [i].name [0] != myRPN.t_name [0])         continue;
      if  (strcmp (s_consts [i].name, myRPN.t_name ) != 0)    continue;
      x_found = i;
      DEBUG_YRPN    yLOG_value   ("x_found"   , x_found);
      break;
   }
   DEBUG_YRPN    yLOG_info    ("token name", myRPN.t_name);
   /*---(handle misses)--------------------*/
   --rce;  if (x_found < 0) {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("type not found");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)-----------------------------*/
   DEBUG_YRPN    yLOG_note    ("put constant directly to output");
   yRPN_stack_tokens  ();         /* strait to tokens list                          */
   yRPN__token_save    (a_pos);
   myRPN.left_oper  = S_OPER_CLEAR;
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
   char        rc          =    0;     /* generic return code                 */
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
   myRPN.t_type   = S_TTYPE_VARS;
   x_pos        = a_pos;  /* starting point */
   while (yRPN__token_add (&x_pos) == 0);
   DEBUG_YRPN    yLOG_info    ("myRPN.t_name", myRPN.t_name);
   /*---(check if long enough)-------------*/
   DEBUG_YRPN    yLOG_value   ("myRPN.t_len" , myRPN.t_len);
   --rce;  if (myRPN.t_len <  1) {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("name too short");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check func vs vars)---------------*/
   rc = yRPN_stack_peek ();
   if (myRPN.s_type == S_TTYPE_FPTR)  myRPN.t_type  = S_TTYPE_FUNC;
   else                             myRPN.t_type  = yRPN__token_paren (x_pos);
   /*---(push functions)-------------------*/
   if (myRPN.t_type == S_TTYPE_FUNC) {
      DEBUG_YRPN    yLOG_note    ("put function on stack");
      myRPN.t_prec = S_PREC_FUNC;
      yRPN_stack_tokens  ();         /* strait to tokens list                          */
      yRPN__token_push    (a_pos);
   }
   /*---(save variables)-------------------*/
   else {
      DEBUG_YRPN    yLOG_note    ("put variable directly to output");
      if (myRPN.l_type == S_TTYPE_OPER) {
         if (strcmp (myRPN.l_name, "." ) == 0)  myRPN.t_type = S_TTYPE_MEMB;
         if (strcmp (myRPN.l_name, "->") == 0)  myRPN.t_type = S_TTYPE_MEMB;
      }
      yRPN_stack_tokens  ();         /* strait to tokens list                          */
      yRPN__token_save    (a_pos);
   }
   myRPN.left_oper  = S_OPER_CLEAR;
   /*---(complete)-------------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return x_pos;
}



/*====================------------------------------------====================*/
/*===----                          token handling                      ----===*/
/*====================------------------------------------====================*/
static void        o___TOKENS__________________o (void) {;}

/*> int        /+ ---- : save off string literals --------------------------------+/   <* 
 *> yRPN__text         (int  a_pos)                                                    <* 
 *> {                                                                                  <* 
 *>    strcat (myRPN.tokens, myRPN.working + a_pos);                                       <* 
 *>    strcat (myRPN.tokens, " ");                                                       <* 
 *>    return strlen (myRPN.working);                                                    <* 
 *> }                                                                                  <*/

int          /*--> check for operators -------------------[--------[--------]-*/
yRPN__oper_splat     (int  a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   char  rc = 0;
   DEBUG_YRPN    yLOG_note    ("special pointer handling");
   DEBUG_YRPN    yLOG_char    ("line_type" , myRPN.line_type);
   rc = yRPN_stack_peek ();
   DEBUG_YRPN    yLOG_info    ("p_name"    , myRPN.p_name);
   DEBUG_YRPN    yLOG_char    ("p_type"    , myRPN.p_type);
   if (rc >= 0 && myRPN.p_type == S_TTYPE_CAST) {
      DEBUG_YRPN    yLOG_note    ("working in * casting mode");
      strlcpy (myRPN.t_name, "(*)", S_LEN_LABEL);
      myRPN.t_type = S_TTYPE_CAST;
      myRPN.t_prec = S_PREC_FUNC;
      myRPN.t_dir  = S_RIGHT;
      DEBUG_YRPN    yLOG_info    ("t_name"    , myRPN.t_name);
      DEBUG_YRPN    yLOG_char    ("t_type"    , myRPN.t_type);
      yRPN_stack_push       (a_pos);
      myRPN.left_oper  = S_OPER_CLEAR;
   /*> } else if (rc >= 0 && myRPN.p_type == S_TTYPE_FUNC) {                            <*/
   } else if (rc >= 0 && (myRPN.line_type == S_LINE_DEF_FPTR || myRPN.line_type == S_LINE_DEF_FUN || myRPN.line_type == S_LINE_DEF_PRO)) {
      DEBUG_YRPN    yLOG_note    ("working in * type modifier mode");
      strlcpy (myRPN.t_name, "(*)", S_LEN_LABEL);
      myRPN.t_type = S_TTYPE_PTYPE;
      DEBUG_YRPN    yLOG_info    ("t_name"    , myRPN.t_name);
      DEBUG_YRPN    yLOG_char    ("t_type"    , myRPN.t_type);
      yRPN__token_save    (a_pos);
      myRPN.left_oper  = S_OPER_CLEAR;
   } else if (myRPN.line_type == S_LINE_DEF_FPTR) {
      DEBUG_YRPN    yLOG_note    ("working in function pointer mode");
      strlcpy (myRPN.t_name, "(>", S_LEN_LABEL);
      myRPN.t_type = S_TTYPE_FPTR;
      DEBUG_YRPN    yLOG_info    ("t_name"    , myRPN.t_name);
      DEBUG_YRPN    yLOG_char    ("t_type"    , myRPN.t_type);
      yRPN__token_save    (a_pos);
      myRPN.left_oper  = S_OPER_CLEAR;
   } else if ((myRPN.line_type == S_LINE_DEF || myRPN.line_type == S_LINE_DEF_VAR) && myRPN.line_sect != '=') {
      DEBUG_YRPN    yLOG_note    ("working in * type modifier mode");
      strlcpy (myRPN.t_name, "(*)", S_LEN_LABEL);
      myRPN.t_type = S_TTYPE_TYPE;
      DEBUG_YRPN    yLOG_info    ("t_name"    , myRPN.t_name);
      DEBUG_YRPN    yLOG_char    ("t_type"    , myRPN.t_type);
      yRPN__token_save    (a_pos);
      myRPN.left_oper  = S_OPER_LEFT;
   } else {
      DEBUG_YRPN    yLOG_note    ("working in * dereference mode");
      DEBUG_YRPN    yLOG_info    ("t_name"    , myRPN.t_name);
      DEBUG_YRPN    yLOG_char    ("t_type"    , myRPN.t_type);
      yRPN_stack_oper       (a_pos);
      myRPN.left_oper  = S_OPER_LEFT;  /* an oper after an oper must be right-only */
   }
   return 0;
}

int          /*--> check for operators -------------------[--------[--------]-*/
yRPN__operators      (int  a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /* operators are symbols and stored in a table.                            */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   char        rc          =    0;     /* generic return code                 */
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
   myRPN.t_type   = S_TTYPE_OPER;
   x_pos        = a_pos;  /* starting point */
   while (yRPN__token_add (&x_pos) == 0);
   DEBUG_YRPN    yLOG_info    ("myRPN.t_name", myRPN.t_name);
   /*---(try to match operators)-----------*/
   DEBUG_YRPN    yLOG_note    ("search operators");
   for (i = 0; i < MAX_OPER; ++i) {
      if  (s_opers [i].name [0] == '\0')                            break;
      if  (s_opers [i].real     != 'r' )                            continue;
      if  (s_opers [i].who != zRPN_lang && s_opers [i].who != 'B')  continue;
      if  (s_opers [i].name [0] != myRPN.t_name [0])                  continue;
      if  (strcmp (s_opers [i].name, myRPN.t_name ) != 0)             continue;
      x_found = i;
      DEBUG_YRPN    yLOG_value   ("x_found"   , x_found);
      break;
   }
   if (x_found < 0 && myRPN.t_len == 2) {
      DEBUG_YRPN    yLOG_note    ("two character operator not found");
      DEBUG_YRPN    yLOG_note    ("switching to single character operator mode");
      myRPN.t_name [1] = '\0';
      myRPN.t_len      = 1;
      --x_pos;
      for (i = 0; i < MAX_OPER; ++i) {
         if  (s_opers [i].name [0] == '\0')                   break;
         if  (s_opers [i].real     != 'r' )                   continue;
         if  (s_opers [i].name [0] != myRPN.t_name [0])         continue;
         if  (strcmp (s_opers [i].name, myRPN.t_name ) != 0)    continue;
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
   if (myRPN.left_oper  == S_OPER_LEFT) {
      DEBUG_YRPN    yLOG_note    ("check for left operators in right only mode");
      if        (strcmp (myRPN.t_name, "+" ) == 0)  { strcpy (myRPN.t_name, "+:"); myRPN.t_len  = 2;
      } else if (strcmp (myRPN.t_name, "-" ) == 0)  { strcpy (myRPN.t_name, "-:"); myRPN.t_len  = 2;
      } else if (strcmp (myRPN.t_name, "*" ) == 0)  { strcpy (myRPN.t_name, "*:"); myRPN.t_len  = 2;
      } else if (strcmp (myRPN.t_name, "&" ) == 0)  { strcpy (myRPN.t_name, "&:"); myRPN.t_len  = 2;
      }
   } else {
      DEBUG_YRPN    yLOG_note    ("check for right operators in left only mode");
      if        (strcmp (myRPN.t_name, "++") == 0)  { strcpy (myRPN.t_name, ":+"); myRPN.t_len  = 2;
      } else if (strcmp (myRPN.t_name, "--") == 0)  { strcpy (myRPN.t_name, ":-"); myRPN.t_len  = 2;
      }
   }
   /*---(handle it)------------------------*/
   yRPN__prec ();
   yRPN_stack_tokens      ();
   if        (myRPN.pproc == S_PPROC_YES) {
      DEBUG_YRPN    yLOG_note    ("pre-processor directive");
      yRPN__token_save    (a_pos);
      myRPN.left_oper  = S_OPER_LEFT;  /* an oper after an oper must be right-only */
   } else if (strcmp (myRPN.t_name, "*:") == 0) {
      DEBUG_YRPN    yLOG_note    ("working with a pointer");
      yRPN__oper_splat  (a_pos);
   } else {
      DEBUG_YRPN    yLOG_note    ("working with normal operators");
      yRPN_stack_oper       (a_pos);
      myRPN.left_oper  = S_OPER_LEFT;  /* an oper after an oper must be right-only */
   }
   /*---(complete)-------------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return x_pos;
}



/*====================------------------------------------====================*/
/*===----                          group handling                      ----===*/
/*====================------------------------------------====================*/
static void        o___GROUPING________________o (void) {;}

int
yRPN__brack_open     (int a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /* push a close bracket and open paren to the stack                        */
   /*---(handle bracket)-----------------*/
   yRPN_stack_tokens      ();
   yRPN_stack_normal     (a_pos);
   /*---(put reverse on stack)-----------*/
   strcpy (myRPN.t_name, "]");
   myRPN.t_type     = S_TTYPE_OPER;
   yRPN_stack_push(a_pos);
   /*---(pretend open paren)-------------*/
   strcpy (myRPN.t_name, "(");
   myRPN.t_type     = S_TTYPE_GROUP;
   yRPN__prec ();
   yRPN_stack_tokens      ();
   yRPN_stack_push       (a_pos);
   myRPN.left_oper  = S_OPER_LEFT;
   /*---(complete)-----------------------*/
   return 0;
}


int
yRPN__paren_comma    (int a_pos)
{
   char rc = 0;
   yRPN_stack_tokens ();
   rc = yRPN_stack_peek();
   while (rc >= 0  &&  myRPN.p_prec != 'd' + 16) {
      yRPN_stack_pops ();
      rc = yRPN_stack_peek();
   }
   if (rc < 0) {
      /*> zRPN_DEBUG  printf ("      FATAL :: nothing more on stack\n");           <*/
      /*> return rc;                                                               <*/
   }
   if (zRPN_lang != S_LANG_GYGES) {
      myRPN.t_type = S_TTYPE_OPER;
      yRPN_stack_shuntd ();
      yRPN_stack_normal (a_pos);
      myRPN.left_oper  = S_OPER_LEFT;
   }
   return 0;
}

int          /*--> check for grouping symbols ------------[--------[--------]-*/
yRPN__sequencer      (int  a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /* grouping symbols are all one-char and specific.                         */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   char        rc          =    0;     /* generic return code                 */
   int         x_pos       =    0;     /* updated position in input           */
   int         i           =    0;     /* iterator for keywords               */
   int         x_found     =   -1;     /* index of keyword                    */
   int         x_type      =  '-';
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
   --rce;  if (strchr (v_sequence, myRPN.working [a_pos]) == 0) {
      DEBUG_YRPN    yLOG_note    ("not a valid grouping symbol");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (zRPN_lang != 'c' && strchr ("[]", myRPN.working [a_pos]) != 0) {
      DEBUG_YRPN    yLOG_note    ("braces not allowed in spreadsheet mode");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN    yLOG_note    ("accumulate characters");
   myRPN.t_name [0] = myRPN.working [a_pos];
   myRPN.t_name [1] = '\0';
   myRPN.t_len      = 1;
   myRPN.t_type     = S_TTYPE_GROUP;
   x_pos          = a_pos + 1;
   DEBUG_YRPN    yLOG_info    ("myRPN.t_name", myRPN.t_name);
   DEBUG_YRPN    yLOG_value   ("myRPN.t_len" , myRPN.t_len);
   yRPN__prec ();
   /*---(handle)---------------------------*/
   DEBUG_YRPN    yLOG_char    ("line_type" , myRPN.line_type);
   switch (myRPN.t_name [0]) {
   case '(' :
      ++myRPN.paren_lvl;
      yRPN_stack_tokens  ();         /* strait to tokens list                          */
      rc = yRPN_stack_peek    ();
      if (rc >= 0) {
         DEBUG_YRPN    yLOG_note    ("something in stack");
         DEBUG_YRPN    yLOG_info    ("myRPN.p_name", myRPN.p_name);
         DEBUG_YRPN    yLOG_char    ("myRPN.p_type", myRPN.p_type);
         if (myRPN.p_type == S_TTYPE_FUNC || strcmp (myRPN.p_name, "sizeof") == 0) {
            DEBUG_YRPN    yLOG_note    ("function openning");
            /*> strlcpy (myRPN.t_name, "(>", S_LEN_LABEL);                              <*/
            myRPN.t_type = S_TTYPE_FUNC;
            yRPN__token_push (a_pos);
            myRPN.left_oper  = S_OPER_LEFT;
         } else {
            DEBUG_YRPN    yLOG_note    ("normal parenthesis");
            /*> strlcpy (myRPN.t_name, "(:", S_LEN_LABEL);                              <* 
             *> myRPN.t_type = S_TTYPE_CAST;                                            <* 
             *> myRPN.t_prec = S_PREC_FUNC;                                             <* 
             *> yRPN__token_push (a_pos);                                             <* 
             *> myRPN.left_oper  = S_OPER_LEFT;                                         <*/
            yRPN__token_push (a_pos);
            myRPN.left_oper  = S_OPER_LEFT;
         }
      } else {  /* clever pointer stuff */
         DEBUG_YRPN    yLOG_note    ("empty stack");
         if (myRPN.line_type == S_LINE_NORMAL) {
            DEBUG_YRPN    yLOG_note    ("normal parenthesis");
            yRPN__token_push (a_pos);
            myRPN.left_oper  = S_OPER_LEFT;
         } else {
            DEBUG_YRPN    yLOG_note    ("function pointer paren (ignore)");
            /*> strlcpy (myRPN.t_name, "(>", S_LEN_LABEL);                              <* 
             *> myRPN.t_type = S_TTYPE_FPTR;                                            <* 
             *> yRPN__token_push (a_pos);                                             <*/
            myRPN.left_oper  = S_OPER_LEFT;
            myRPN.line_type  = S_LINE_DEF_FPTR;
         }
      }
      break;
   case ')' :
      x_type  = myRPN.s_type;
      DEBUG_YRPN    yLOG_char    ("x_type"    , x_type);
      DEBUG_YRPN    yLOG_char    ("line_type" , myRPN.line_type);
      if (myRPN.line_type == S_LINE_DEF_FPTR || myRPN.line_type == S_LINE_DEF_PRO) {
         if (x_type == S_TTYPE_PTYPE) {
            strlcpy (myRPN.t_name, "?", S_LEN_LABEL);
            myRPN.t_type = S_TTYPE_VARS;
            myRPN.t_prec = S_PREC_NONE;
            yRPN_stack_tokens      ();
            yRPN__token_save (a_pos);
            strlcpy (myRPN.t_name, ")", S_LEN_LABEL);
            myRPN.t_type = S_TTYPE_GROUP;
            yRPN__prec ();
         }
      }
      --myRPN.paren_lvl;
      yRPN_stack_tokens     ();
      if (myRPN.s_type == S_TTYPE_FPTR &&  myRPN.line_type == S_LINE_DEF_FPTR) {
         DEBUG_YRPN    yLOG_note    ("function pointer paren (ignore)");
      } else {
         DEBUG_YRPN    yLOG_note    ("normal closing parens");
         yRPN_stack_normal     (a_pos);
         yRPN_stack_paren      (a_pos);
      }
      myRPN.left_oper  = S_OPER_CLEAR;
      break;
   case '[' :
      DEBUG_YRPN    yLOG_note    ("openning brace");
      yRPN_stack_tokens     ();
      yRPN__token_push      (a_pos);
      myRPN.left_oper  = S_OPER_LEFT;
      break;
   case ']' :
      DEBUG_YRPN    yLOG_note    ("closing brace");
      yRPN_stack_tokens     ();
      yRPN_stack_paren      (a_pos);
      myRPN.t_type     = S_TTYPE_OPER;
      yRPN_stack_shuntd     ();
      break;
   case ',' :
      DEBUG_YRPN    yLOG_note    ("comma");
      x_type  = myRPN.s_type;
      myRPN.line_sect = '-';
      DEBUG_YRPN    yLOG_char    ("x_type"    , x_type);
      DEBUG_YRPN    yLOG_char    ("line_sect" , myRPN.line_sect);
      if (myRPN.line_type == S_LINE_DEF_FPTR || myRPN.line_type == S_LINE_DEF_PRO) {
         if (x_type == S_TTYPE_PTYPE) {
            strlcpy (myRPN.t_name, "?", S_LEN_LABEL);
            myRPN.t_type = S_TTYPE_VARS;
            myRPN.t_prec = S_PREC_NONE;
            yRPN_stack_tokens      ();
            yRPN__token_save (a_pos);
            strlcpy (myRPN.t_name, ",", S_LEN_LABEL);
            myRPN.t_type = S_TTYPE_GROUP;
            yRPN__prec ();
         }
      }
      yRPN_stack_tokens      ();
      yRPN_stack_normal     (a_pos);
      rc = yRPN_stack_comma (a_pos);
      myRPN.left_oper  = S_OPER_LEFT;
      break;
   }
   /*---(complete)-------------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return x_pos;
}

int          /*--> check for statement enders ------------[--------[--------]-*/
yRPN__enders         (int  a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /* ending symbols are very limited and specific.                           */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   char        rc          =    0;     /* generic return code                 */
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
   --rce;  if (strchr (v_enders  , myRPN.working [a_pos]) == 0) {
      DEBUG_YRPN    yLOG_note    ("not a valid grouping symbol");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN    yLOG_note    ("accumulate characters");
   myRPN.t_name [0] = myRPN.working [a_pos];
   myRPN.t_name [1] = '\0';
   myRPN.t_len      = 1;
   myRPN.t_type     = S_TTYPE_GROUP;
   x_pos          = a_pos + 1;
   DEBUG_YRPN    yLOG_info    ("myRPN.t_name", myRPN.t_name);
   DEBUG_YRPN    yLOG_value   ("myRPN.t_len" , myRPN.t_len);
   yRPN__prec ();
   /*---(handle)---------------------------*/
   switch (myRPN.t_name [0]) {
   case ';' :
      yRPN_stack_tokens  ();         /* strait to tokens list                          */
      myRPN.left_oper  = S_OPER_LEFT;
      break;
   case '{' :
      yRPN_stack_tokens      ();
      myRPN.left_oper  = S_OPER_CLEAR;
      break;
   }
   /*---(complete)-------------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return x_pos;
}



/*====================------------------------------------====================*/
/*===----                         unit testing                         ----===*/
/*====================------------------------------------====================*/
static void      o___UNITTEST________________o (void) {;};

char*      /* ---- : answer unit testing gray-box questions ------------------*/
yRPN_syms_unit       (char *a_question, int a_item)
{
   /*---(spreadsheet)--------------------*/
   if   (strcmp (a_question, "address"   )     == 0) {
      snprintf (unit_answer, S_LEN_OUTPUT, "yRPN address     : tab=%4d, col=%4d, row=%4d, abs=%1d", v_tab, v_col, v_row, v_abs);
   }
   /*---(UNKNOWN)------------------------*/
   else {
      snprintf(unit_answer, S_LEN_OUTPUT, "UNKNOWN          : question is not understood");
   }
   /*---(complete)-----------------------*/
   return unit_answer;
}



/*============================----end-of-source---============================*/
