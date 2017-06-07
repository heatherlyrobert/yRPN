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
   { "#"   , 'c', '-',  0, 'd',  S_LEFT , 2, "prefix"                               },
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
   { "&"   , 'B', 'r',  8, 'l',  S_LEFT , 2, "bitwise AND"                          },
   { "^"   , 'B', 'r',  9, 'm',  S_LEFT , 2, "bitwise XOR"                          },
   { "|"   , 'B', 'r', 10, 'n',  S_LEFT , 2, "bitwise OR"                           },
   /*---(logical)----------------*/
   { "&&"  , 'B', 'r', 11, 'o',  S_LEFT , 2, "logical AND"                          },
   { "||"  , 'B', 'r', 12, 'p',  S_LEFT , 2, "logical OR"                           },
   /*---(conditional)------------*/
   { "?"   , 'c', 'r', 13, 'q',  S_RIGHT, 2, "trinary conditional"                  },
   { ":"   , 'c', 'r', 13, 'q',  S_RIGHT, 2, "trinary conditional"                  },
   /*---(assignment)-------------*/
   { "="   , 'c', 'r', 14, 'r',  S_RIGHT, 2, "direct assignment"                    },
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

char       /* ---- : identify the symbol precedence --------------------------*/
yRPN__p_prec       (void)
{
   int       i         = 0;
   for (i = 0; i < MAX_OPER; ++i) {
      if  (strcmp (s_opers[i].name, "end"     ) == 0)  break;
      if  (strcmp (s_opers[i].name, rpn.p_name) != 0)  continue;
      rpn.p_prec  = s_opers[i].prec;
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
   rpn.t_type   = S_TTYPE_KEYW;
   rpn.t_prec   = S_PREC_FUNC;
   x_pos        = a_pos;  /* starting point */
   while (yRPN__token_add (&x_pos) == 0);
   /*---(try to match keyword)-------------*/
   DEBUG_YRPN    yLOG_note    ("search keywords");
   for (i = 0; i < MAX_KEYWORDS; ++i) {
      if  (s_keywords [i].name [0] == '\0')                break;
      if  (s_keywords [i].name [0] != rpn.t_name [0])      continue;
      if  (strcmp (s_keywords [i].name, rpn.t_name ) != 0) continue;
      x_found = i;
      x_use = s_keywords [i].usage;
      DEBUG_YRPN    yLOG_value   ("x_found"   , x_found);
      DEBUG_YRPN    yLOG_char    ("x_use"     , x_use);
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
   DEBUG_YRPN    yLOG_note    ("keyword is a function");
   yRPN_stack_tokens  ();         /* strait to tokens list                          */
   if (x_use == 'f')  yRPN__token_push    (a_pos);
   else               yRPN__token_save    (a_pos);
   rpn.left_oper = S_OPER_LEFT;
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
   rpn.t_type   = S_TTYPE_TYPE;
   x_pos        = a_pos;  /* starting point */
   while (yRPN__token_add (&x_pos) == 0);
   DEBUG_YRPN    yLOG_info    ("rpn.t_name", rpn.t_name);
   /*---(try to match types)-------------*/
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
   /*---(handle misses)------------------*/
   --rce;  if (x_found < 0) {
      yRPN__token_error ();
      DEBUG_YRPN    yLOG_note    ("type not found");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(definition vs casting)----------*/
   yRPN_stack_tokens  ();         /* strait to tokens list                          */
   if (rpn.line_type == S_LINE_NORMAL) {
      DEBUG_YRPN    yLOG_note    ("treating as a casting onto stack");
      rc == yRPN_stack_peek ();
      DEBUG_YRPN    yLOG_char    ("rpn.p_type", rpn.p_type);
      switch (rpn.p_type) {
      case S_TTYPE_FUNC :
         DEBUG_YRPN    yLOG_note    ("function parm type");
         yRPN__token_save    (a_pos);
         rpn.left_oper  = S_OPER_LEFT;
         break;
      case S_TTYPE_FPTR :
         DEBUG_YRPN    yLOG_note    ("function pointer type");
         yRPN__token_save    (a_pos);
         rpn.left_oper  = S_OPER_LEFT;
         break;
      case S_TTYPE_TYPE :
         DEBUG_YRPN    yLOG_note    ("normal type");
         yRPN__token_save    (a_pos);
         rpn.left_oper  = S_OPER_LEFT;
         break;
      case S_TTYPE_CAST :
      default           :
         DEBUG_YRPN    yLOG_note    ("casting type");
         /*---(fix paren)----------------*/
         if (strcmp (rpn.p_name, "(") == 0) {
            strlcpy (rpn.p_name, "(:", S_LEN_LABEL);
            rpn.p_type = S_TTYPE_CAST;
            rpn.p_prec = S_PREC_FUNC;
            yRPN_stack_update ();
         }
         /*---(update current)-----------*/
         rpn.t_type = S_TTYPE_CAST;
         rpn.t_prec = S_PREC_FUNC;
         /*---(push)---------------------*/
         yRPN__token_push    (a_pos);
         rpn.left_oper  = S_OPER_LEFT;
      }
   }
   /*---(save)---------------------------*/
   else if (rpn.paren_lvl > 0) {
      DEBUG_YRPN    yLOG_note    ("put c type directly to output");
      rpn.t_type = S_TTYPE_PTYPE;
      yRPN__token_save    (a_pos);
      rpn.left_oper  = S_OPER_LEFT;
   }
   /*---(save)---------------------------*/
   else {
      DEBUG_YRPN    yLOG_note    ("put c type directly to output");
      yRPN__token_save    (a_pos);
      rpn.left_oper  = S_OPER_LEFT;
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
   yRPN_stack_tokens  ();         /* strait to tokens list                          */
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
   yRPN_stack_tokens  ();         /* strait to tokens list                          */
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
   yRPN_stack_tokens  ();         /* strait to tokens list                          */
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
   yRPN_stack_tokens  ();         /* strait to tokens list                          */
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
   rc = yRPN_stack_peek ();
   if (rpn.s_type == S_TTYPE_FPTR)  rpn.t_type  = S_TTYPE_FUNC;
   else                             rpn.t_type  = yRPN__token_paren (x_pos);
   /*---(push functions)-------------------*/
   if (rpn.t_type == S_TTYPE_FUNC) {
      DEBUG_YRPN    yLOG_note    ("put function on stack");
      rpn.t_prec = S_PREC_FUNC;
      yRPN_stack_tokens  ();         /* strait to tokens list                          */
      yRPN__token_push    (a_pos);
   }
   /*---(save variables)-------------------*/
   else {
      DEBUG_YRPN    yLOG_note    ("put variable directly to output");
      yRPN_stack_tokens  ();         /* strait to tokens list                          */
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

/*> int        /+ ---- : save off string literals --------------------------------+/   <* 
 *> yRPN__text         (int  a_pos)                                                    <* 
 *> {                                                                                  <* 
 *>    strcat (rpn.tokens, rpn.working + a_pos);                                       <* 
 *>    strcat (rpn.tokens, " ");                                                       <* 
 *>    return strlen (rpn.working);                                                    <* 
 *> }                                                                                  <*/

int          /*--> check for operators -------------------[--------[--------]-*/
yRPN__oper_splat     (int  a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   char  rc = 0;
   DEBUG_YRPN    yLOG_note    ("special pointer handling");
   rc = yRPN_stack_peek ();
   DEBUG_YRPN    yLOG_info    ("p_name"    , rpn.p_name);
   DEBUG_YRPN    yLOG_char    ("p_type"    , rpn.p_type);
   if (rc >= 0 && rpn.p_type == S_TTYPE_CAST) {
      DEBUG_YRPN    yLOG_note    ("working in * casting mode");
      strlcpy (rpn.t_name, "(*)", S_LEN_LABEL);
      rpn.t_type = S_TTYPE_CAST;
      rpn.t_prec = S_PREC_FUNC;
      rpn.t_dir  = S_RIGHT;
      DEBUG_YRPN    yLOG_info    ("t_name"    , rpn.t_name);
      DEBUG_YRPN    yLOG_char    ("t_type"    , rpn.t_type);
      yRPN_stack_push       (a_pos);
      rpn.left_oper  = S_OPER_CLEAR;
   } else if (rc >= 0 && rpn.p_type == S_TTYPE_FUNC) {
      DEBUG_YRPN    yLOG_note    ("working in * type modifier mode");
      strlcpy (rpn.t_name, "(*)", S_LEN_LABEL);
      rpn.t_type = S_TTYPE_PTYPE;
      DEBUG_YRPN    yLOG_info    ("t_name"    , rpn.t_name);
      DEBUG_YRPN    yLOG_char    ("t_type"    , rpn.t_type);
      yRPN__token_save    (a_pos);
      rpn.left_oper  = S_OPER_CLEAR;
   } else if (rpn.line_type == S_LINE_DEF_FPTR) {
      DEBUG_YRPN    yLOG_note    ("working in function pointer mode");
      strlcpy (rpn.t_name, "(>", S_LEN_LABEL);
      rpn.t_type = S_TTYPE_FPTR;
      DEBUG_YRPN    yLOG_info    ("t_name"    , rpn.t_name);
      DEBUG_YRPN    yLOG_char    ("t_type"    , rpn.t_type);
      yRPN__token_save    (a_pos);
      rpn.left_oper  = S_OPER_CLEAR;
   } else if (rpn.line_type == S_LINE_DEF) {
      DEBUG_YRPN    yLOG_note    ("working in * type modifier mode");
      strlcpy (rpn.t_name, "(*)", S_LEN_LABEL);
      rpn.t_type = S_TTYPE_TYPE;
      DEBUG_YRPN    yLOG_info    ("t_name"    , rpn.t_name);
      DEBUG_YRPN    yLOG_char    ("t_type"    , rpn.t_type);
      yRPN__token_save    (a_pos);
      rpn.left_oper  = S_OPER_CLEAR;
   } else {
      DEBUG_YRPN    yLOG_note    ("working in * dereference mode");
      DEBUG_YRPN    yLOG_info    ("t_name"    , rpn.t_name);
      DEBUG_YRPN    yLOG_char    ("t_type"    , rpn.t_type);
      yRPN_stack_oper       (a_pos);
      rpn.left_oper  = S_OPER_LEFT;  /* an oper after an oper must be right-only */
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
   yRPN__prec ();
   yRPN_stack_tokens      ();
   if (strcmp (rpn.t_name, "*:") == 0) {
      DEBUG_YRPN    yLOG_note    ("working with a pointer");
      yRPN__oper_splat  (a_pos);
   } else {
      DEBUG_YRPN    yLOG_note    ("working with normal operators");
      yRPN_stack_oper       (a_pos);
      rpn.left_oper  = S_OPER_LEFT;  /* an oper after an oper must be right-only */
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
   strcpy (rpn.t_name, "]");
   rpn.t_type     = S_TTYPE_OPER;
   yRPN_stack_push(a_pos);
   /*---(pretend open paren)-------------*/
   strcpy (rpn.t_name, "(");
   rpn.t_type     = S_TTYPE_GROUP;
   yRPN__prec ();
   yRPN_stack_tokens      ();
   yRPN_stack_push       (a_pos);
   rpn.left_oper  = S_OPER_LEFT;
   /*---(complete)-----------------------*/
   return 0;
}


int
yRPN__paren_comma    (int a_pos)
{
   char rc = 0;
   yRPN_stack_tokens ();
   rc = yRPN_stack_peek();
   while (rc >= 0  &&  rpn.p_prec != 'd' + 16) {
      yRPN_stack_pops ();
      rc = yRPN_stack_peek();
   }
   if (rc < 0) {
      /*> zRPN_DEBUG  printf ("      FATAL :: nothing more on stack\n");           <*/
      /*> return rc;                                                               <*/
   }
   if (zRPN_lang != S_LANG_GYGES) {
      rpn.t_type = S_TTYPE_OPER;
      yRPN_stack_shuntd ();
      yRPN_stack_normal (a_pos);
      rpn.left_oper  = S_OPER_LEFT;
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
   --rce;  if (strchr (v_sequence, rpn.working [a_pos]) == 0) {
      DEBUG_YRPN    yLOG_note    ("not a valid grouping symbol");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN    yLOG_note    ("accumulate characters");
   rpn.t_name [0] = rpn.working [a_pos];
   rpn.t_name [1] = '\0';
   rpn.t_len      = 1;
   rpn.t_type     = S_TTYPE_GROUP;
   x_pos          = a_pos + 1;
   DEBUG_YRPN    yLOG_info    ("rpn.t_name", rpn.t_name);
   DEBUG_YRPN    yLOG_value   ("rpn.t_len" , rpn.t_len);
   yRPN__prec ();
   /*---(handle)---------------------------*/
   DEBUG_YRPN    yLOG_char    ("line_type" , rpn.line_type);
   switch (rpn.t_name [0]) {
   case '(' :
      ++rpn.paren_lvl;
      yRPN_stack_tokens  ();         /* strait to tokens list                          */
      rc = yRPN_stack_peek    ();
      if (rc >= 0) {
         DEBUG_YRPN    yLOG_note    ("something in stack");
         DEBUG_YRPN    yLOG_info    ("rpn.p_name", rpn.p_name);
         DEBUG_YRPN    yLOG_char    ("rpn.p_type", rpn.p_type);
         if (rpn.p_type == S_TTYPE_FUNC || strcmp (rpn.p_name, "sizeof") == 0) {
            DEBUG_YRPN    yLOG_note    ("function openning");
            /*> strlcpy (rpn.t_name, "(>", S_LEN_LABEL);                              <*/
            rpn.t_type = S_TTYPE_FUNC;
            yRPN__token_push (a_pos);
            rpn.left_oper  = S_OPER_LEFT;
         } else {
            DEBUG_YRPN    yLOG_note    ("normal parenthesis");
            /*> strlcpy (rpn.t_name, "(:", S_LEN_LABEL);                              <* 
             *> rpn.t_type = S_TTYPE_CAST;                                            <* 
             *> rpn.t_prec = S_PREC_FUNC;                                             <* 
             *> yRPN__token_push (a_pos);                                             <* 
             *> rpn.left_oper  = S_OPER_LEFT;                                         <*/
            yRPN__token_push (a_pos);
            rpn.left_oper  = S_OPER_LEFT;
         }
      } else {  /* clever pointer stuff */
         DEBUG_YRPN    yLOG_note    ("empty stack");
         if (rpn.line_type == S_LINE_NORMAL) {
            DEBUG_YRPN    yLOG_note    ("normal parenthesis");
            yRPN__token_push (a_pos);
            rpn.left_oper  = S_OPER_LEFT;
         } else {
            DEBUG_YRPN    yLOG_note    ("function pointer paren (ignore)");
            /*> strlcpy (rpn.t_name, "(>", S_LEN_LABEL);                              <* 
             *> rpn.t_type = S_TTYPE_FPTR;                                            <* 
             *> yRPN__token_push (a_pos);                                             <*/
            rpn.left_oper  = S_OPER_LEFT;
            rpn.line_type  = S_LINE_DEF_FPTR;
         }
      }
      break;
   case ')' :
      --rpn.paren_lvl;
      yRPN_stack_tokens     ();
      if (rpn.s_type == S_TTYPE_FPTR &&  rpn.line_type == S_LINE_DEF_FPTR) {
         DEBUG_YRPN    yLOG_note    ("function pointer paren (ignore)");
      } else {
         DEBUG_YRPN    yLOG_note    ("normal closing parens");
         yRPN_stack_normal     (a_pos);
         yRPN_stack_paren      (a_pos);
      }
      rpn.left_oper  = S_OPER_CLEAR;
      break;
   case '[' :
      DEBUG_YRPN    yLOG_note    ("openning brace");
      yRPN_stack_tokens     ();
      yRPN__token_push      (a_pos);
      rpn.left_oper  = S_OPER_LEFT;
      break;
   case ']' :
      DEBUG_YRPN    yLOG_note    ("closing brace");
      yRPN_stack_tokens     ();
      yRPN_stack_paren      (a_pos);
      rpn.t_type     = S_TTYPE_OPER;
      yRPN_stack_shuntd     ();
      break;
   case ',' :
      DEBUG_YRPN    yLOG_note    ("comma");
      yRPN_stack_tokens      ();
      yRPN_stack_normal     (a_pos);
      rc = yRPN_stack_paren (a_pos);
      rpn.left_oper  = S_OPER_LEFT;
      break;
   }
   /*---(complete)-------------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return x_pos;
}

/*> int        /+ ---- : process grouping ----------------------------------------+/                       <* 
 *> yRPN__sequencer      (int  a_pos)                                                                      <* 
 *> {                                                                                                      <* 
 *>    /+---(design notes)-------------------+/                                                            <* 
 *>    /+                                                                                                  <* 
 *>     *  operators can have a wild number of forms and precedences                                       <* 
 *>     *                                                                                                  <* 
 *>     +/                                                                                                 <* 
 *>    /+---(begin)--------------------------+/                                                            <* 
 *>    zRPN_DEBUG  printf("   group------------------\n");                                                 <* 
 *>    /+---(locals)---------------------------+/                                                          <* 
 *>    int       i         = a_pos;                                                                        <* 
 *>    int       rc        = 0;                                                                            <* 
 *>    char      x_match   = ' ';                                                                          <* 
 *>    char      x_fake    = 'n';                                                                          <* 
 *>    /+---(prepare)------------------------+/                                                            <* 
 *>    strlcpy (rpn.t_name, YRPN_TOKEN_NULL, S_LEN_TOKEN);                                                 <* 
 *>    rpn.t_name[0] = rpn.working[i];                                                                     <* 
 *>    rpn.t_name[1] = '\0';                                                                               <* 
 *>    rpn.t_type     = S_TTYPE_GROUP;                                                                     <* 
 *>    rpn.t_prec     = S_PREC_NONE;                                                                       <* 
 *>    /+---(defenses)-----------------------+/                                                            <* 
 *>    if (i              >= rpn.l_working)           return  zRPN_ERR_INPUT_NOT_AVAIL;                    <* 
 *>    if (strchr(v_sequence   , rpn.working[i]) == 0)  return  zRPN_ERR_NOT_GROUPING;                     <* 
 *>    /+---(main loop)------------------------+/                                                          <* 
 *>    rpn.t_len  = 1;                                                                                     <* 
 *>    ++i;                                                                                                <* 
 *>    /+---(end)------------------------------+/                                                          <* 
 *>    zRPN_DEBUG  printf("      fin (%02d) <<%s>>\n", rpn.t_len , rpn.t_name);                            <* 
 *>    /+---(handle it)------------------------+/                                                          <* 
 *>    yRPN__prec ();                                                                                <* 
 *>    zRPN_DEBUG  printf("      prec = %c\n", rpn.t_prec);                                                <* 
 *>    /+---(open bracket)---------------------+/                                                          <* 
 *>    /+> if (rpn.t_name[0] == '[') {                                                               <*    <* 
 *>     *>    yRPN_stack_tokens ();                                                                        <*    <* 
 *>     *>    /+> strcpy (rpn.t_name, "]*");                                                   <+/   <*    <* 
 *>     *>    strcpy (rpn.t_name, "]");                                                              <*    <* 
 *>     *>    rpn.t_type         = S_TTYPE_OPER;                                                     <*    <* 
 *>     *>    yRPN_stack_push(a_pos);                                                                     <*    <* 
 *>     *>    strcpy (rpn.t_name, "[");                                                              <*    <* 
 *>     *>    yRPN_stack_normal (a_pos);                                                                  <*    <* 
 *>     *>    x_fake = 'y';                                                                          <*    <* 
 *>     *>    strcpy (rpn.t_name, "(");                                                              <*    <* 
 *>     *>    rpn.t_type         = S_TTYPE_GROUP;                                                    <*    <* 
 *>     *>    yRPN__prec ();                                                                   <*    <* 
 *>     *> }                                                                                         <+/   <* 
 *>    /+---(open paren)-----------------------+/                                                          <* 
 *>    if (rpn.t_name[0] == '(') {                                                                         <* 
 *>       yRPN_stack_tokens ();                                                                                  <* 
 *>       yRPN_stack_push(a_pos);                                                                               <* 
 *>       if (x_fake == 'n')  yRPN_stack_normal (a_pos);                                                        <* 
 *>       rpn.left_oper  = S_OPER_LEFT;                                                                    <* 
 *>       return 0;                                                                                        <* 
 *>    }                                                                                                   <* 
 *>    /+---(close bracket)-------------------+/                                                           <* 
 *>    /+> if (rpn.t_name[0] == ']') {                                                    <*               <* 
 *>     *>    yRPN_stack_tokens ();                                                             <*               <* 
 *>     *>    strcpy (rpn.t_name, ")");                                                   <*               <* 
 *>     *>    rpn.t_type         = S_TTYPE_GROUP;                                         <*               <* 
 *>     *>    yRPN__prec ();                                                        <*               <* 
 *>     *>    x_fake = 'y';                                                               <*               <* 
 *>     *> }                                                                              <+/              <* 
 *>    /+---(close paren)----------------------+/                                                          <* 
 *>    if (rpn.t_name[0] == ')') {                                                                         <* 
 *>       yRPN_stack_tokens ();                                                                                  <* 
 *>       if (x_fake == 'n')  yRPN_stack_normal (a_pos);                                                        <* 
 *>       rc = yRPN_stack_peek();                                                                               <* 
 *>       while (rc >= 0  &&  rpn.p_prec != 'd' + 16) {                                                    <* 
 *>          yRPN_stack_pops ();                                                                                 <* 
 *>          rc = yRPN_stack_peek();                                                                            <* 
 *>       }                                                                                                <* 
*>       if (rc < 0) {                                                                                    <* 
   *>          zRPN_DEBUG  printf ("      FATAL :: nothing more on stack\n");                                <* 
      *>          return rc;                                                                                    <* 
      *>       }                                                                                                <* 
      *>       yRPN_stack_toss();                                                                                    <* 
      *>       rc = yRPN_stack_peek();                                                                               <* 
      *>       if (rpn.t_token[0] == ']')  yRPN_stack_pops ();                                                        <* 
      *>       rpn.left_oper  = S_OPER_CLEAR;                                                                   <* 
      *>    }                                                                                                   <* 
      *>    /+---(comma)----------------------------+/                                                          <* 
      *>    /+> if (strncmp(rpn.t_name, ",", 1) == 0) {                                                  <*     <* 
         *>     *>    yRPN_stack_tokens ();                                                                       <*     <* 
            *>     *>    rc = yRPN_stack_peek();                                                                    <*     <* 
            *>     *>    while (rc >= 0  &&  rpn.p_prec != 'd' + 16) {                                         <*     <* 
               *>     *>       yRPN_stack_pops ();                                                                      <*     <* 
                  *>     *>       rc = yRPN_stack_peek();                                                                 <*     <* 
                  *>     *>    }                                                                                     <*     <* 
                  *>     *>    if (rc < 0) {                                                                         <*     <* 
                     *>     *>       /+> zRPN_DEBUG  printf ("      FATAL :: nothing more on stack\n");           <+/   <*     <* 
                        *>     *>       /+> return rc;                                                               <+/   <*     <* 
                        *>     *>    }                                                                                     <*     <* 
                        *>     *>    if (zRPN_lang != S_LANG_GYGES) {                                                      <*     <* 
                           *>     *>       rpn.t_type = S_TTYPE_OPER;                                                         <*     <* 
                              *>     *>       yRPN_stack_shuntd ();                                                                     <*     <* 
                              *>     *>       yRPN_stack_normal (a_pos);                                                              <*     <* 
                              *>     *>       rpn.left_oper  = S_OPER_LEFT;                                                      <*     <* 
                              *>     *>    }                                                                                     <*     <* 
                              *>     *> }                                                                                        <+/    <* 
                              *>    /+---(complete)-------------------------+/                                                          <* 
                              *>    return i;                                                                                           <* 
                              *> }                                                                                                      <*/

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
   --rce;  if (strchr (v_enders  , rpn.working [a_pos]) == 0) {
      DEBUG_YRPN    yLOG_note    ("not a valid grouping symbol");
      DEBUG_YRPN    yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN    yLOG_note    ("accumulate characters");
   rpn.t_name [0] = rpn.working [a_pos];
   rpn.t_name [1] = '\0';
   rpn.t_len      = 1;
   rpn.t_type     = S_TTYPE_GROUP;
   x_pos          = a_pos + 1;
   DEBUG_YRPN    yLOG_info    ("rpn.t_name", rpn.t_name);
   DEBUG_YRPN    yLOG_value   ("rpn.t_len" , rpn.t_len);
   yRPN__prec ();
   /*---(handle)---------------------------*/
   switch (rpn.t_name [0]) {
   case ';' :
      yRPN_stack_tokens  ();         /* strait to tokens list                          */
      rpn.left_oper  = S_OPER_LEFT;
      break;
   case '{' :
      yRPN_stack_tokens      ();
      rpn.left_oper  = S_OPER_CLEAR;
      break;
   }
   /*---(complete)-------------------------*/
   DEBUG_YRPN    yLOG_exit    (__FUNCTION__);
   return x_pos;
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
