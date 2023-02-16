/*============================----beg-of-source---============================*/
#include  "yRPN.h"
#include  "yRPN_priv.h"


/*===[[ HIGH-VALUE CONSTANTS ]]===============================================*/
#define   MAX_CONST   200
typedef   struct cCONST tCONST;
struct    cCONST {
   char      name  [30];
   double    value;
};
tCONST    s_consts [MAX_CONST] = {
   /*---(spreadsheet)------------*/
   /*> { "TRUE"                   , 1.0                },                             <* 
    *> { "FALSE"                  , 0.0                },                             <* 
    *> { "NULL"                   , 0.0                },                             <* 
    *> { "PI"                     , 3.1415927          },                             <* 
    *> { "÷"                      , 3.1415927          },                             <* 
    *> { "DEG2RAD"                , 0.0                },                             <* 
    *> { "RAD2DEG"                , 0.0                },                             <*/
   { ""                       , 0.0                },
};



/*===[[ STANDARD C TYPES ]]===================================================*/
#define   MAX_TYPES   200
typedef   struct cTYPES tTYPES;
struct    cTYPES {
   char      parts [30];
   char      name  [30];
};
tTYPES    s_types [MAX_TYPES] = {
   /*---(base types)-------------*/
   { "00.000.1000" , "void"                },
   /*---(char)-------------------*/
   { "00.100.0000.", "char"                },
   { "10.100.0000.", "signed char"         },
   { "01.100.0000.", "unsigned char"       },
   { "00.100.0100.", "char"                },
   { "10.100.0100.", "signed char"         },
   { "01.100.0100.", "unsigned char"       },
   /*---(short)------------------*/
   { "00.010.0000.", "short"               },
   { "10.010.0000.", "signed short"        },
   { "01.010.0000.", "unsigned short"      },
   { "00.010.0100.", "short"               },
   { "10.010.0100.", "signed short"        },
   { "01.010.0100.", "unsigned short"      },
   /*---(integer)----------------*/
   { "00.000.0100.", "int"                 },
   { "10.000.0100.", "signed int"          },
   { "01.000.0100.", "unsigned int"        },
   /*---(long)-------------------*/
   { "00.001.0000.", "long"                },
   { "10.001.0000.", "signed long"         },
   { "01.001.0000.", "unsigned long"       },
   { "00.001.0100.", "long"                },
   { "10.001.0100.", "signed long"         },
   { "01.001.0100.", "unsigned long"       },
   /*---(long long---------------*/
   { "00.002.0000.", "long long"           },
   { "10.002.0000.", "signed long long"    },
   { "01.002.0000.", "unsigned long long"  },
   { "00.002.0100.", "long long"           },
   { "10.002.0100.", "signed long long"    },
   { "01.002.0100.", "unsigned long long"  },
   /*---(float)------------------*/
   { "00.000.0010.", "float"               },
   /*---(double)-----------------*/
   { "00.000.0001.", "double"              },
   /*---(long double)------------*/
   { "00.001.0001.", "long double"         },
   /*---(done)-------------------*/
   { "00.000.0000.", ""                    },
};

struct {
   char      name       [30];
   char      seq;
} s_type_words [MAX_TYPES] = {
   /*---(modifiers)--------------*/
   { "signed"         ,  0 },
   { "unsigned"       ,  1 },
   /*---(size types)-------------*/
   { "char"           ,  3 },
   { "short"          ,  4 },
   { "long"           ,  5 },
   /*---(base types)-------------*/
   { "void"           ,  7 },
   { "int"            ,  8 },
   { "float"          ,  9 },
   { "double"         , 10 },
   /*---(storage)----------------*/
   { "auto"           , 12 },
   { "extern"         , 13 },
   { "static"         , 14 },
   /*---(qualifiers)-------------*/
   { "const"          , 15 },
   { "volitle"        , 16 },
   { "register"       , 17 },
   /*---(done)-------------------*/
   { ""               ,  0 },
};

char g_type_counts    [LEN_LABEL];



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



/*====================------------------------------------====================*/
/*===----                  single character validation                 ----===*/
/*====================------------------------------------====================*/
static void        o___SINGLES_________________o (void) {;}

char
yrpn_cbang_one          (short l, char f, char p, char c)
{
   DEBUG_YRPN_M  yLOG_snote   ("KEYW/TYPE");
   if (c == 0)                                        return 0;
   if (strchr (YSTR_LOWER, c) == NULL)                return 0;
   return 1;
}



/*====================------------------------------------====================*/
/*===----                        overall drivers                       ----===*/
/*====================------------------------------------====================*/
static void        o___DRIVERS_________________o (void) {;}

short
yrpn_cbang_keyword      (short a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /* keywords are only lowercase alphanumerics.                              */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   int         x_pos       =    0;     /* updated position in input           */
   int         i           =    0;     /* iterator for keywords               */
   int         x_found     =   -1;     /* index of keyword                    */
   int         x_use       =  '-';
   /*---(header)------------------------*/
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yrpn_token_error  ();
   --rce;  if (myRPN.lang != YRPN_CBANG) {
      DEBUG_YRPN     yLOG_note    ("skip if not explicitly in cbang mode");
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
   myRPN.t_type   = YRPN_KEYW;
   myRPN.t_prec   = S_PREC_FUNC;
   x_pos        = a_pos;  /* starting point */
   while (yrpn_token_accum (&x_pos) == 0);
   /*---(try to match keyword)-------------*/
   DEBUG_YRPN     yLOG_note    ("search keywords");
   for (i = 0; i < MAX_KEYWORDS; ++i) {
      if  (s_keywords [i].name [0] == '\0')                break;
      if  (s_keywords [i].name [0] != myRPN.t_name [0])      continue;
      if  (strcmp (s_keywords [i].name, myRPN.t_name ) != 0) continue;
      x_found = i;
      x_use = s_keywords [i].usage;
      DEBUG_YRPN     yLOG_value   ("x_found"   , x_found);
      DEBUG_YRPN     yLOG_char    ("x_use"     , x_use);
      break;
   }
   DEBUG_YRPN     yLOG_info    ("myRPN.t_name", myRPN.t_name);
   /*---(handle misses)--------------------*/
   --rce;  if (x_found < 0) {
      yrpn_token_error  ();
      DEBUG_YRPN     yLOG_note    ("keyword not found");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(mark includes)--------------------*/
   DEBUG_YRPN     yLOG_note    ("check for preprocessor keywords");
   if (myRPN.pproc == S_PPROC_YES) {
      if (strcmp (myRPN.t_name, "include") == 0)  myRPN.pproc = S_PPROC_INCL;
      else                                        myRPN.pproc = S_PPROC_OTHER;
   }
   /*---(save)-----------------------------*/
   strlcpy (myRPN.t_token, myRPN.t_name, LEN_FULL);
   DEBUG_YRPN     yLOG_note    ("keyword is a function");
   yrpn_output_infix (myRPN.t_type, myRPN.t_prec, myRPN.t_name, myRPN.t_token, a_pos);
   if (x_use == 'f')  yrpn_stack_push   (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
   else               yrpn_output_rpn   (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
   myRPN.left_oper  = S_OPER_LEFT;
   myRPN.combined   = S_NO;
   /*---(complete)-------------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return x_pos;
}

/*> char                                                                              <* 
 *> yrpn_type_reset      (void)                                                       <* 
 *> {                                                                                 <* 
 *>    strlcpy (g_type_counts, "00.000.0000.000.000", LEN_LABEL);                     <* 
 *>    return 0;                                                                      <* 
 *> }                                                                                 <*/

short
yrpn_cbang_type         (short a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   /* types are only lowercase alphanumerics.                                 */
   /*---(locals)-----------+-----------+-*/
   char        rce         =  -10;     /* return code for errors              */
   char        rc          =  -10;     /* generic return code                 */
   int         x_pos       =    0;     /* updated position in input           */
   int         i           =    0;     /* iterator for keywords               */
   int         x_found     =   -1;     /* index of keyword                    */
   /*---(header)------------------------*/
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yrpn_token_error  ();
   --rce;  if (myRPN.lang != YRPN_CBANG) {
      DEBUG_YRPN     yLOG_note    ("skip if not explicitly in cbang mode");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   DEBUG_YRPN     yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos <  0) {
      DEBUG_YRPN     yLOG_note    ("start can not be negative");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)----------*/
   DEBUG_YRPN     yLOG_note    ("accumulate characters");
   myRPN.t_type   = YRPN_TYPE   ;
   x_pos        = a_pos;  /* starting point */
   while (yrpn_token_accum (&x_pos) == 0);
   DEBUG_YRPN     yLOG_info    ("myRPN.t_name", myRPN.t_name);
   /*---(try to match types)-------------*/
   DEBUG_YRPN     yLOG_note    ("search types");
   for (i = 0; i < MAX_TYPES; ++i) {
      if  (s_type_words [i].name [0] == '\0')                     break;
      if  (s_type_words [i].name [0] != myRPN.t_name [0])         continue;
      if  (strcmp (s_type_words [i].name, myRPN.t_name ) != 0)    continue;
      x_found = i;
      DEBUG_YRPN     yLOG_value   ("x_found"   , x_found);
      break;
   }
   DEBUG_YRPN     yLOG_info    ("token name", myRPN.t_name);
   /*---(handle misses)------------------*/
   --rce;  if (x_found < 0) {
      yrpn_token_error  ();
      DEBUG_YRPN     yLOG_note    ("type not found");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(definition vs casting)----------*/
   strlcpy (myRPN.t_token, myRPN.t_name, LEN_FULL);
   yrpn_output_infix (myRPN.t_type, myRPN.t_prec, myRPN.t_name, myRPN.t_token, a_pos);
   if (myRPN.line_type == S_LINE_NORMAL) {
      DEBUG_YRPN     yLOG_note    ("treating as a casting onto stack");
      rc == yRPN_stack_peek_OLD ();
      DEBUG_YRPN     yLOG_char    ("myRPN.p_type", myRPN.p_type);
      switch (myRPN.p_type) {
      case YRPN_FUNC    :
         DEBUG_YRPN     yLOG_note    ("function parm type");
         yrpn_output_rpn   (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
         myRPN.left_oper  = S_OPER_LEFT;
         break;
      case YRPN_FPTR    :
         DEBUG_YRPN     yLOG_note    ("function pointer type");
         yrpn_output_rpn   (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
         myRPN.left_oper  = S_OPER_LEFT;
         break;
      case YRPN_TYPE    :
         DEBUG_YRPN     yLOG_note    ("normal type");
         yrpn_output_rpn   (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
         myRPN.left_oper  = S_OPER_LEFT;
         break;
      case YRPN_CAST    :
      default           :
         DEBUG_YRPN     yLOG_note    ("casting type");
         /*---(fix paren)----------------*/
         if (strcmp (myRPN.p_name, "(") == 0) {
            strlcpy (myRPN.p_name, "(:", LEN_LABEL);
            myRPN.p_type = YRPN_CAST   ;
            myRPN.p_prec = S_PREC_FUNC;
            yrpn_stack_update (myRPN.t_type, myRPN.t_prec, myRPN.t_name);
         }
         /*---(update current)-----------*/
         myRPN.t_type = YRPN_CAST   ;
         myRPN.t_prec = S_PREC_FUNC;
         /*---(push)---------------------*/
         yrpn_stack_push   (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
         myRPN.left_oper  = S_OPER_LEFT;
      }
   }
   /*---(save)---------------------------*/
   else if (myRPN.level > 0) {
      DEBUG_YRPN     yLOG_note    ("put c type directly to output");
      myRPN.t_type = YRPN_PTYPE;
      yrpn_output_rpn   (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
      myRPN.left_oper  = S_OPER_LEFT;
   }
   /*---(save)---------------------------*/
   else {
      DEBUG_YRPN     yLOG_note    ("put c type directly to output");
      yrpn_output_rpn   (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
      myRPN.left_oper  = S_OPER_LEFT;
   }
   /*---(complete)-----------------------*/
   myRPN.combined   = S_NO;
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return x_pos;
}


