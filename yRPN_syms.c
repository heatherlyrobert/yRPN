/*============================----beg-of-source---============================*/

#include  "yRPN.h"
#include  "yRPN_priv.h"



char     *v_alphanum  = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789_èéêëìíîïðñòóôõö÷øùúûüýþÿÀÁÂÃÄÅ";
char     *v_alpha     = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_èéêëìíîïðñòóôõö÷øùúûüýþÿ";
/*> char     *v_octal     = "o01234567";                                              <*/
char     *v_sequence  = "(),[]";   
char     *v_enders    = ";{";
/*> char     *v_operator  = "|&=!<>+/%+-.?:^~#ÑÐÒÔÓÕÆÇÈÎ";                            <*/
/*> char     *v_preproc   = "#";                                                      <*/
char     *v_preproc   = "";



/*===[[ OPERATORS ]]==========================================================*/
typedef   struct cOPER tOPER;
struct    cOPER {
   char        name        [5];        /* operator                            */
   char        orig        [5];        /* operator                            */
   char        who;                    /* who can use it c, gyges, both       */
   char        real;                   /* real/used, internal, or off         */
   char        offset;                 /* offset for precedence               */
   char        prec;                   /* percedence                          */
   char        dir;                    /* direction of evaluation             */
   char        arity;                  /* how many arguments it takes         */
   char        combine;                /* combining operator                  */
   char        post_un;                /* postfix unary operator              */
   char        pre;                    /* prefix in pretty mode               */
   char        suf;                    /* suffix in pretty mode               */
   char        pretty      [5];        /* replacement in pretty mode          */
   char        desc        [LEN_DESC]; /* helpful description vs comment      */
};
#define   MAX_OPER    200
tOPER     s_opers [MAX_OPER] = {
   /*-sym----who--real---prec- ---dir--- ar -comb- -post- -pre-- -suf-- prety ---comment-----------------------------*/
   /*---(spreadsheet)------------*/
   { ".."  , ".."  , 'g', 'r',  0, 'd',  S_LEFT , 2, S_YES, S_NO , S_NO , S_NO , ""  , "cell range"                           },
   /*---(preprocessor)-----------*/
   /*> { "#"   , 'c', 'r',  0, 'd',  S_LEFT , 2, S_NO , S_NO , S_NO , S_NO , ""  , "prefix"                               },   <*/
   /*> { "##"  , 'c', '-',  0, 'd',  S_LEFT , 2, S_NO , S_NO , S_NO , S_NO , ""  , "stringification"                      },   <*/
   /*---(unary/suffix)-----------*/
   { ":+"  , "++"  , 'B', 'I',  1, 'e',  S_LEFT , 1, S_NO , S_YES, S_NO , S_YES, ""  , "postfix increment"                    },
   { ":-"  , "--"  , 'B', 'I',  1, 'e',  S_LEFT , 1, S_NO , S_YES, S_NO , S_YES, ""  , "postfix decrement"                    },
   { "Æ"   , "Æ"   , 'B', 'r',  1, 'e',  S_LEFT , 1, S_NO , S_YES, S_NO , S_YES, ""  , "power of two, squared"                },
   { "Ç"   , "Ç"   , 'B', 'r',  1, 'e',  S_LEFT , 1, S_NO , S_YES, S_NO , S_YES, ""  , "power of three, cubed"                },
   { "È"   , "È"   , 'B', 'r',  1, 'e',  S_LEFT , 1, S_NO , S_YES, S_NO , S_YES, ""  , "power of four"                        },
   { "É"   , "É"   , 'B', 'r',  1, 'e',  S_LEFT , 1, S_NO , S_YES, S_NO , S_YES, ""  , "power of x"                           },
   { "Ê"   , "Ê"   , 'B', 'r',  1, 'e',  S_LEFT , 1, S_NO , S_YES, S_NO , S_YES, ""  , "power of y"                           },
   { "Î"   , "Î"   , 'B', 'r',  1, 'e',  S_LEFT , 1, S_NO , S_YES, S_NO , S_YES, ""  , "power of half, square root"           },
   { "Í"   , "Í"   , 'B', 'r',  1, 'e',  S_LEFT , 1, S_NO , S_YES, S_NO , S_YES, ""  , "radian operator"                      },
   { "Ì"   , "Ì"   , 'B', 'r',  1, 'e',  S_LEFT , 1, S_NO , S_YES, S_NO , S_YES, ""  , "degree operator"                      },
   /*---(element of)-------------*/
   { "["   , "["   , 'c', 'r',  1, 'e',  S_LEFT , 1, S_YES, S_NO , S_YES, S_NO , ""  , "array subscripting"                   },
   { "]"   , "]"   , 'c', 'r',  1, 'e',  S_LEFT , 1, S_YES, S_NO , S_NO , S_YES, ""  , "array subscripting"                   },
   { "."   , "."   , 'c', 'r',  1, 'e',  S_LEFT , 2, S_YES, S_NO , S_NO , S_NO , ""  , "element selection by reference"       },
   { "->"  , "->"  , 'c', 'r',  1, 'e',  S_LEFT , 2, S_YES, S_NO , S_NO , S_NO , "Ö" , "element selection thru pointer"       },
   /*---(unary/prefix)-----------*/
   { "++"  , "++"  , 'B', 'r',  2, 'f',  S_RIGHT, 1, S_NO , S_NO , S_YES, S_NO , ""  , "prefix increment"                     },
   { "--"  , "--"  , 'B', 'r',  2, 'f',  S_RIGHT, 1, S_NO , S_NO , S_YES, S_NO , ""  , "prefix decrement"                     },
   { "+:"  , "+"   , 'B', 'I',  2, 'f',  S_RIGHT, 1, S_NO , S_NO , S_YES, S_NO , ""  , "unary plus"                           },
   { "-:"  , "-"   , 'B', 'I',  2, 'f',  S_RIGHT, 1, S_NO , S_NO , S_YES, S_NO , ""  , "unary minus"                          },
   { "!"   , "!"   , 'B', 'r',  2, 'f',  S_RIGHT, 1, S_NO , S_NO , S_YES, S_NO , ""  , "logical NOT"                          },
   { "~"   , "~"   , 'B', 'r',  2, 'f',  S_RIGHT, 1, S_NO , S_NO , S_YES, S_NO , ""  , "bitwise NOT"                          },
   { "*:"  , "*"   , 'B', 'I',  2, 'f',  S_RIGHT, 1, S_NO , S_NO , S_YES, S_NO , ""  , "indirection/dereference"              },
   { "&:"  , "&"   , 'B', 'I',  2, 'f',  S_RIGHT, 1, S_NO , S_NO , S_YES, S_NO , ""  , "address-of"                           },
   { "(*)" , "*"   , 'c', 'I',  2, 'f',  S_RIGHT, 1, S_NO , S_NO , S_YES, S_YES, ""  , "casting modifier"                     },
   /*---(multiplicative)---------*/
   { "*"   , "*"   , 'B', 'r',  3, 'g',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, ""  , "multiplication"                       },
   { "/"   , "/"   , 'B', 'r',  3, 'g',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, ""  , "division"                             },
   { "%"   , "%"   , 'B', 'r',  3, 'g',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, ""  , "modulus"                              },
   /*---(additive)---------------*/
   { "+"   , "+"   , 'B', 'r',  4, 'h',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, ""  , "addition"                             },
   { "-"   , "-"   , 'B', 'r',  4, 'h',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, ""  , "substraction"                         },
   { "›"   , "›"   , 'g', 'r',  4, 'h',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, ""  , "string concatination"                 },
   { "››"  , "››"  , 'g', 'r',  4, 'h',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, ""  , "string concatination"                 },
   /*---(shift)------------------*/
   { "<<"  , "<<"  , 'B', 'r',  5, 'i',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, ""  , "bitwise shift left"                   },
   { ">>"  , ">>"  , 'B', 'r',  5, 'i',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, ""  , "bitwise shift right"                  },
   /*---(relational)-------------*/
   { "<"   , "<"   , 'B', 'r',  6, 'j',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, ""  , "relational lesser"                    },
   { "<="  , "<="  , 'B', 'r',  6, 'j',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, "Ü" , "relational less or equal"             },
   { ">"   , ">"   , 'B', 'r',  6, 'j',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, ""  , "relational greater"                   },
   { ">="  , ">="  , 'B', 'r',  6, 'j',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, "Ý" , "relational more or equal"             },
   { "›<"  , "›<"  , 'g', 'r',  6, 'j',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, ""  , "relational string lesser"             },
   { "›>"  , "›>"  , 'g', 'r',  6, 'j',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, ""  , "relational string greater"            },
   /*---(equality)---------------*/
   { "=="  , "=="  , 'B', 'r',  7, 'k',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, "à"  , "relational equality"                  },
   { "!="  , "!="  , 'B', 'r',  7, 'k',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, "Þ" , "relational inequality"                },
   { "›="  , "›="  , 'g', 'r',  7, 'k',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, ""  , "relational string equality"           },
   { "›!"  , "›!"  , 'g', 'r',  7, 'k',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, ""  , "relational string inequality"         },
   /*---(bitwise)----------------*/
   { "&"   , "&"   , 'B', 'r',  8, 'l',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, ""  , "bitwise AND"                          },
   { "^"   , "^"   , 'B', 'r',  9, 'm',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, ""  , "bitwise XOR"                          },
   { "|"   , "|"   , 'B', 'r', 10, 'n',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, ""  , "bitwise OR"                           },
   /*---(logical)----------------*/
   { "&&"  , "&&"  , 'B', 'r', 11, 'o',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, "Ð" , "logical AND"                          },
   { "!&"  , "!&"  , 'B', 'r', 11, 'o',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, "Ô" , "NAND (at least one is false)"         },
   { "||"  , "||"  , 'B', 'r', 12, 'p',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, "Ñ" , "logical OR"                           },
   { "!|"  , "!|"  , 'B', 'r', 12, 'p',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, "Õ" , "NOR (neither/nor)"                    },
   { "&|"  , "&|"  , 'B', 'r', 12, 'p',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, "Ò" , "XOR (one and only one)"               },
   { "|&"  , "|&"  , 'B', 'r', 12, 'p',  S_LEFT , 2, S_YES, S_NO , S_YES, S_YES, "Ó" , "NXOR (both the same)"                 },
   /*---(conditional)------------*/
   { "?"   , "?"   , 'c', 'r', 13, 'q',  S_RIGHT, 2, S_YES, S_NO , S_YES, S_YES, ""  , "trinary conditional"                  },
   { ":"   , ":"   , 'c', 'r', 13, 'q',  S_RIGHT, 2, S_YES, S_NO , S_YES, S_YES, ""  , "trinary conditional"                  },
   /*---(assignment)-------------*/
   { "="   , "="   , 'B', 'r', 14, 'r',  S_RIGHT, 2, S_YES, S_NO , S_YES, S_YES, ""  , "direct numeric assignment"            },
   { "+="  , "+="  , 'c', 'r', 14, 'r',  S_RIGHT, 2, S_YES, S_NO , S_YES, S_YES, ""  , "compound assignment (addition)"       },
   { "-="  , "-="  , 'c', 'r', 14, 'r',  S_RIGHT, 2, S_YES, S_NO , S_YES, S_YES, ""  , "compound assignment (subtract)"       },
   { "*="  , "*="  , 'c', 'r', 14, 'r',  S_RIGHT, 2, S_YES, S_NO , S_YES, S_YES, ""  , "compound assignment (multiply)"       },
   { "/="  , "/="  , 'c', 'r', 14, 'r',  S_RIGHT, 2, S_YES, S_NO , S_YES, S_YES, ""  , "compound assignment (divide)"         },
   { "%="  , "%="  , 'c', 'r', 14, 'r',  S_RIGHT, 2, S_YES, S_NO , S_YES, S_YES, ""  , "compound assignment (modulus)"        },
   { "<<=" , "<<=" , 'c', 'r', 14, 'r',  S_RIGHT, 2, S_YES, S_NO , S_YES, S_YES, ""  , "compound assignment (bitwise left)"   },
   { ">>=" , ">>=" , 'c', 'r', 14, 'r',  S_RIGHT, 2, S_YES, S_NO , S_YES, S_YES, ""  , "compound assignment (bitwise right)"  },
   { "&="  , "&="  , 'c', 'r', 14, 'r',  S_RIGHT, 2, S_YES, S_NO , S_YES, S_YES, ""  , "compound assignment (bitwise AND)"    },
   { "^="  , "^="  , 'c', 'r', 14, 'r',  S_RIGHT, 2, S_YES, S_NO , S_YES, S_YES, ""  , "compound assignment (bitwise XOR)"    },
   { "|="  , "|="  , 'c', 'r', 14, 'r',  S_RIGHT, 2, S_YES, S_NO , S_YES, S_YES, ""  , "compound assignment (bitwise OR)"     },
   { "#"   , "#"   , 'B', 'r', 14, 'r',  S_RIGHT, 2, S_YES, S_NO , S_YES, S_YES, ""  , "direct string assignment"             },
   { "›#"  , "›#"  , 'c', 'r', 14, 'r',  S_RIGHT, 2, S_YES, S_NO , S_YES, S_YES, ""  , "concatinate and assign"               },
   /*---(comma)------------------*/
   { ",;"  , ";"   , 'c', 'r', 15, 'u',  S_LEFT , 1, S_YES, S_NO , S_NO , S_YES, ""  , "sequence mega-separator"              },
   { ","   , ","   , 'B', 'r', 15, 's',  S_LEFT , 2, S_YES, S_NO , S_NO , S_YES, ""  , "sequence separator"                   },
   /*---(parenthesis)------------*/
   { "("   , "("   , 'B', 'r', 16, 't',  S_LEFT , 1, S_NO , S_NO , S_YES, S_NO , ""  , "sequence openning"                    },
   { ")"   , ")"   , 'B', 'r', 16, 't',  S_LEFT , 1, S_NO , S_NO , S_NO , S_YES, ""  , "sequence closing"                     },
   { "(>"  , "("   , 'c', 'I', 16, 't',  S_LEFT , 1, S_YES, S_NO , S_YES, S_NO , ""  , "function opening"                     },
   { "(:"  , "("   , 'c', 'I', 16, 't',  S_LEFT , 1, S_YES, S_NO , S_YES, S_NO , ""  , "casting opening"                      },
   { "):"  , ")"   , 'c', 'I', 16, 't',  S_LEFT , 1, S_YES, S_NO , S_NO , S_YES, ""  , "casting closing"                      },
   /*---(semicolon)--------------*/
   { ";"   , ";"   , 'c', 'r', 17, 'u',  S_LEFT , 1, S_NO , S_NO , S_NO , S_YES, ""  , "statement separator"                  },
   /*-------------(braces)-------*/
   { "{"   , "{"   , 'c', 'r', 18, 'v',  S_LEFT , 1, S_NO , S_NO , S_YES, S_YES, ""  , "function openning"                    },
   { "}"   , "}"   , 'c', 'r', 18, 'v',  S_LEFT , 1, S_NO , S_NO , S_YES, S_YES, ""  , "function closing"                     },
   /*---(end)--------------------*/
   { ""    , ""    , '-', '-',  0, '-',  '-'    , 0, S_NO , S_NO , S_NO , S_NO , ""  , ""                                     },
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





char       /* ---- : identify the symbol spacing -----------------------------*/
yRPN_symbols  (void)
{
   int       i         = 0;
   int       o         = -1;
   char      c         = '·';
   int       n         =  0;
   char      s         [LEN_LABEL] = "";
   char      t         [LEN_LABEL] = "";
   printf ("yRPN symbol table extract\n\n");
   printf ("\npre sym new qik dir ari  desc------------------------------\n");
   for (i = 0; i < MAX_OPER; ++i) {
      if  (strcmp (s_opers[i].name, ""  ) == 0)  break;
      if (s_opers [i].offset != o) {
         if (n % 25 > 20) {
            printf ("\npre sym new qik dir ari  desc------------------------------\n");
            n = 0;
         }
         printf ("\n%2d  ", o = s_opers [i].offset);
      }
      else                           printf ("    ");
      c = s_opers [i].pretty [0];
      if (c == '\0')  c = '·';
      if (strcmp (s_opers [i].orig, s_opers [i].name) != 0)  strcpy (s, s_opers [i].name);
      else                                                   strcpy (s, "·");
      sprintf (t, "  %c   %d  ", s_opers [i].dir, s_opers [i].arity);
      printf ("%-3.3s %-3.3s  %c %s %-35.35s \n", s_opers [i].orig, s, c, t, s_opers [i].desc);
      ++n;
   }
   printf ("\nfound %d symbols\n", i);
   return 0;
}



char       /* ---- : identify the symbol spacing -----------------------------*/
yRPN_space   (char  *a_token, char *a_pre, char *a_suf, char *a_new)
{
   int       i         = 0;
   for (i = 0; i < MAX_OPER; ++i) {
      if  (strcmp (s_opers[i].name, "end"  ) == 0)  break;
      if  (strcmp (s_opers[i].name, a_token) != 0)  continue;
      if (a_pre != NULL)  *a_pre = s_opers [i].pre;
      if (a_suf != NULL)  *a_suf = s_opers [i].suf;
      if (a_new != NULL)  strlcpy (a_new, s_opers [i].pretty, LEN_LABEL);
      return  0;
   }
   /*---(complete)----------------*/
   if (a_pre != NULL)  *a_pre = S_YES;
   if (a_suf != NULL)  *a_suf = S_YES;
   if (a_new != NULL)  strlcpy (a_new, "", LEN_LABEL);
   return -1;
}

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
      myRPN.t_post  = s_opers[i].post_un;
      myRPN.t_comb  = s_opers[i].combine;
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

char
yrpn_ready              (void)
{
   DEBUG_YRPN     yLOG_char    ("combined"  , myRPN.combined);
   return 1;
   if (myRPN.combined == S_YES)  return 1;
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
   int         x_use       =  '-';
   /*---(header)------------------------*/
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yRPN__token_error ();
   --rce;  if (zRPN_lang == YRPN_GYGES) {
      DEBUG_YRPN     yLOG_note    ("skip in gyges mode");
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
   myRPN.t_type   = S_TTYPE_KEYW;
   myRPN.t_prec   = S_PREC_FUNC;
   x_pos        = a_pos;  /* starting point */
   while (yrpn_token_add (&x_pos) == 0);
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
      yRPN__token_error ();
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
   yRPN_stack_tokens  ();         /* strait to tokens list                          */
   if (x_use == 'f')  yRPN__token_push    (a_pos);
   else               yRPN__token_save    (a_pos);
   myRPN.left_oper  = S_OPER_LEFT;
   myRPN.combined   = S_NO;
   /*---(complete)-------------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return x_pos;
}

char
yrpn_type_reset      (void)
{
   strlcpy (g_type_counts, "00.000.0000.000.000", LEN_LABEL);
   return 0;
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
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yRPN__token_error ();
   --rce;  if (zRPN_lang == YRPN_GYGES) {
      DEBUG_YRPN     yLOG_note    ("skip in gyges mode");
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
   myRPN.t_type   = S_TTYPE_TYPE;
   x_pos        = a_pos;  /* starting point */
   while (yrpn_token_add (&x_pos) == 0);
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
      yRPN__token_error ();
      DEBUG_YRPN     yLOG_note    ("type not found");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(definition vs casting)----------*/
   strlcpy (myRPN.t_token, myRPN.t_name, LEN_FULL);
   yRPN_stack_tokens  ();         /* strait to tokens list                          */
   if (myRPN.line_type == S_LINE_NORMAL) {
      DEBUG_YRPN     yLOG_note    ("treating as a casting onto stack");
      rc == yRPN_stack_peek ();
      DEBUG_YRPN     yLOG_char    ("myRPN.p_type", myRPN.p_type);
      switch (myRPN.p_type) {
      case S_TTYPE_FUNC :
         DEBUG_YRPN     yLOG_note    ("function parm type");
         yRPN__token_save    (a_pos);
         myRPN.left_oper  = S_OPER_LEFT;
         break;
      case S_TTYPE_FPTR :
         DEBUG_YRPN     yLOG_note    ("function pointer type");
         yRPN__token_save    (a_pos);
         myRPN.left_oper  = S_OPER_LEFT;
         break;
      case S_TTYPE_TYPE :
         DEBUG_YRPN     yLOG_note    ("normal type");
         yRPN__token_save    (a_pos);
         myRPN.left_oper  = S_OPER_LEFT;
         break;
      case S_TTYPE_CAST :
      default           :
         DEBUG_YRPN     yLOG_note    ("casting type");
         /*---(fix paren)----------------*/
         if (strcmp (myRPN.p_name, "(") == 0) {
            strlcpy (myRPN.p_name, "(:", LEN_LABEL);
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
      DEBUG_YRPN     yLOG_note    ("put c type directly to output");
      myRPN.t_type = S_TTYPE_PTYPE;
      yRPN__token_save    (a_pos);
      myRPN.left_oper  = S_OPER_LEFT;
   }
   /*---(save)---------------------------*/
   else {
      DEBUG_YRPN     yLOG_note    ("put c type directly to output");
      yRPN__token_save    (a_pos);
      myRPN.left_oper  = S_OPER_LEFT;
   }
   /*---(complete)-----------------------*/
   myRPN.combined   = S_NO;
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
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
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yRPN__token_error ();
   DEBUG_YRPN     yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos <  0) {
      DEBUG_YRPN     yLOG_note    ("start can not be negative");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN     yLOG_note    ("accumulate characters");
   myRPN.t_type   = S_TTYPE_STR;
   x_pos        = a_pos;  /* starting point */
   while (yrpn_token_add (&x_pos) == 0);
   DEBUG_YRPN     yLOG_info    ("myRPN.t_name", myRPN.t_name);
   /*---(check if long enough)-------------*/
   DEBUG_YRPN     yLOG_value   ("myRPN.t_len" , myRPN.t_len);
   --rce;  if (myRPN.t_len <  2) {
      yRPN__token_error ();
      DEBUG_YRPN     yLOG_note    ("string too short");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check matching quotes)------------*/
   x_last = myRPN.t_len - 1;
   --rce;  if (myRPN.t_name [0] == '\"' && myRPN.t_name [x_last] != '\"') {
      yRPN__token_error ();
      DEBUG_YRPN     yLOG_note    ("string does not end properly");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (myRPN.t_name [0] == '<' && myRPN.t_name [x_last] != '>') {
      yRPN__token_error ();
      DEBUG_YRPN     yLOG_note    ("include does not end properly");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)-----------------------------*/
   DEBUG_YRPN     yLOG_note    ("put string literal directly to output");
   strlcpy (myRPN.t_token, myRPN.t_name, LEN_FULL);
   yRPN_stack_tokens  ();         /* strait to tokens list                          */
   yRPN__token_save    (a_pos);
   myRPN.left_oper  = S_OPER_CLEAR;
   myRPN.combined   = S_NO;
   /*---(complete)-----------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
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
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yRPN__token_error ();
   DEBUG_YRPN     yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos <  0) {
      DEBUG_YRPN     yLOG_note    ("start can not be negative");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN     yLOG_note    ("accumulate characters");
   myRPN.t_type   = S_TTYPE_CHAR;
   x_pos        = a_pos;  /* starting point */
   while (yrpn_token_add (&x_pos) == 0);
   DEBUG_YRPN     yLOG_info    ("myRPN.t_name", myRPN.t_name);
   /*---(check if long enough)-------------*/
   DEBUG_YRPN     yLOG_value   ("myRPN.t_len" , myRPN.t_len);
   --rce;  if (myRPN.t_len <=  0) {
      yRPN__token_error ();
      DEBUG_YRPN     yLOG_note    ("char was illegal");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save)-----------------------------*/
   DEBUG_YRPN     yLOG_note    ("put char literal directly to output");
   strlcpy (myRPN.t_token, myRPN.t_name, LEN_FULL);
   yRPN_stack_tokens  ();         /* strait to tokens list                          */
   yRPN__token_save    (a_pos);
   myRPN.left_oper  = S_OPER_CLEAR;
   myRPN.combined   = S_NO;
   /*---(complete)-----------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
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
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yRPN__token_error ();
   DEBUG_YRPN     yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos <  0) {
      DEBUG_YRPN     yLOG_note    ("start can not be negative");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (strchr ("éöõ" YSTR_NUMBER, myRPN.working [a_pos]) == 0) {
      DEBUG_YRPN     yLOG_note    ("must start with a number");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN     yLOG_note    ("accumulate characters");
   x_pos          = a_pos;  /* starting point */
   myRPN.t_type   = yrpn_token_numtype (x_pos);
   DEBUG_YRPN     yLOG_char    ("myRPN.t_type", myRPN.t_type);
   while (yrpn_token_add (&x_pos) == 0);
   DEBUG_YRPN     yLOG_info    ("myRPN.t_name", myRPN.t_name);
   /*---(check if long enough)-------------*/
   DEBUG_YRPN     yLOG_value   ("myRPN.t_len" , myRPN.t_len);
   switch (myRPN.t_type) {
   case S_TTYPE_INT  :
      if (myRPN.t_len < 1)  x_bad = 'y';
      break;
   case S_TTYPE_OCT  :
      if (myRPN.t_len < 2)  x_bad = 'y';
      break;
   default           :
      if (myRPN.t_len < 2)  x_bad = 'y';
      break;
   }
   --rce;  if (x_bad == 'y') {
      yRPN__token_error ();
      DEBUG_YRPN     yLOG_note    ("number too short");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check float decimals)-------------*/
   --rce;  if (myRPN.t_type == S_TTYPE_FLOAT) {
      rc = strldcnt (myRPN.t_name, '.', LEN_FULL);
      --rce;  if (rc != 1) {
         yRPN__token_error ();
         DEBUG_YRPN     yLOG_note    ("too many decimals");
         DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
      --rce;  if (myRPN.t_name [myRPN.t_len - 1] == '.') {
         yRPN__token_error ();
         DEBUG_YRPN     yLOG_note    ("can not end with decimal");
         DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
         return rce;
      }
   }
   /*---(save)-----------------------------*/
   DEBUG_YRPN     yLOG_note    ("put har literal directly to output");
   strlcpy (myRPN.t_token, myRPN.t_name, LEN_FULL);
   yRPN_stack_tokens  ();         /* strait to tokens list                          */
   yRPN__token_save    (a_pos);
   myRPN.left_oper  = S_OPER_CLEAR;
   myRPN.combined   = S_NO;
   /*---(complete)-----------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return x_pos;
}



/*====================------------------------------------====================*/
/*===----                       symbol handling                        ----===*/
/*====================------------------------------------====================*/
static void        o___SYMBOLS_________________o (void) {;}

/*> int          /+--> check for constants -------------------[--------[--------]-+/         <* 
 *> yRPN__constants      (int  a_pos)                                                        <* 
 *> {  /+---(design notes)--------------------------------------------------------+/         <* 
 *>    /+ constants only contain alphnanumerics plus the underscore.              +/         <* 
 *>    /+---(locals)-----------+-----------+-+/                                              <* 
 *>    char        rce         =  -10;     /+ return code for errors              +/         <* 
 *>    int         x_pos       =    0;     /+ updated position in input           +/         <* 
 *>    int         i           =    0;     /+ iterator for keywords               +/         <* 
 *>    int         x_found     =   -1;     /+ index of keyword                    +/         <* 
 *>    /+---(header)------------------------+/                                               <* 
 *>    DEBUG_YRPN     yLOG_enter   (__FUNCTION__);                                           <* 
 *>    /+---(defenses)-----------------------+/                                              <* 
 *>    yRPN__token_error ();                                                                 <* 
 *>    DEBUG_YRPN     yLOG_value   ("a_pos"     , a_pos);                                    <* 
 *>    --rce;  if (a_pos <  0) {                                                             <* 
 *>       DEBUG_YRPN     yLOG_note    ("start can not be negative");                         <* 
 *>       DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);                                   <* 
 *>       return rce;                                                                        <* 
 *>    }                                                                                     <* 
 *>    /+---(accumulate characters)------------+/                                            <* 
 *>    DEBUG_YRPN     yLOG_note    ("accumulate characters");                                <* 
 *>    myRPN.t_type   = S_TTYPE_CONST;                                                       <* 
 *>    x_pos        = a_pos;  /+ starting point +/                                           <* 
 *>    while (yrpn_token_add (&x_pos) == 0);                                                 <* 
 *>    DEBUG_YRPN     yLOG_info    ("myRPN.t_name", myRPN.t_name);                           <* 
 *>    /+---(try to match constants)-----------+/                                            <* 
 *>    DEBUG_YRPN     yLOG_note    ("search constants");                                     <* 
 *>    for (i = 0; i < MAX_CONST; ++i) {                                                     <* 
 *>       if  (s_consts [i].name [0] == '\0')                   break;                       <* 
 *>       if  (s_consts [i].name [0] != myRPN.t_name [0])         continue;                  <* 
 *>       if  (strcmp (s_consts [i].name, myRPN.t_name ) != 0)    continue;                  <* 
 *>       x_found = i;                                                                       <* 
 *>       DEBUG_YRPN     yLOG_value   ("x_found"   , x_found);                               <* 
 *>       break;                                                                             <* 
 *>    }                                                                                     <* 
 *>    DEBUG_YRPN     yLOG_info    ("token name", myRPN.t_name);                             <* 
 *>    /+---(handle misses)--------------------+/                                            <* 
 *>    --rce;  if (x_found < 0) {                                                            <* 
 *>       yRPN__token_error ();                                                              <* 
 *>       DEBUG_YRPN     yLOG_note    ("type not found");                                    <* 
 *>       DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);                                   <* 
 *>       return rce;                                                                        <* 
 *>    }                                                                                     <* 
 *>    /+---(save)-----------------------------+/                                            <* 
 *>    DEBUG_YRPN     yLOG_note    ("put constant directly to output");                      <* 
 *>    strlcpy (myRPN.t_token, myRPN.t_name, LEN_FULL);                                      <* 
 *>    yRPN_stack_tokens  ();         /+ strait to tokens list                          +/   <* 
 *>    yRPN__token_save    (a_pos);                                                          <* 
 *>    myRPN.left_oper  = S_OPER_CLEAR;                                                      <* 
 *>    myRPN.combined   = S_NO;                                                              <* 
 *>    /+---(complete)-------------------------+/                                            <* 
 *>    DEBUG_YRPN     yLOG_exit    (__FUNCTION__);                                           <* 
 *>    return x_pos;                                                                         <* 
 *> }                                                                                        <*/

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
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yRPN__token_error ();
   DEBUG_YRPN     yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos <  0) {
      DEBUG_YRPN     yLOG_note    ("start can not be negative");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN     yLOG_note    ("accumulate characters");
   myRPN.t_type   = S_TTYPE_VARS;
   x_pos        = a_pos;  /* starting point */
   while (yrpn_token_add (&x_pos) == 0);
   DEBUG_YRPN     yLOG_info    ("myRPN.t_name", myRPN.t_name);
   /*---(check if long enough)-------------*/
   DEBUG_YRPN     yLOG_value   ("myRPN.t_len" , myRPN.t_len);
   --rce;  if (myRPN.t_len <  1) {
      yRPN__token_error ();
      DEBUG_YRPN     yLOG_note    ("name too short");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check if ready)-------------------*/
   --rce;  if (!yrpn_ready ()) {
      DEBUG_YRPN     yLOG_note    ("does not follow a combining operator");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(check func vs vars)---------------*/
   rc = yRPN_stack_peek ();
   if (myRPN.s_type == S_TTYPE_FPTR)  myRPN.t_type  = S_TTYPE_FUNC;
   else                               myRPN.t_type  = yRPN__token_paren (x_pos);
   /*---(push functions)-------------------*/
   if (myRPN.t_type == S_TTYPE_FUNC) {
      DEBUG_YRPN     yLOG_note    ("put function on stack");
      myRPN.t_prec = S_PREC_FUNC;
      strlcpy (myRPN.t_token, myRPN.t_name, LEN_FULL);
      yRPN_stack_tokens  ();         /* strait to tokens list                          */
      yRPN__token_push    (a_pos);
   }
   /*---(save variables)-------------------*/
   else {
      DEBUG_YRPN     yLOG_note    ("put variable directly to output");
      if (myRPN.l_type == S_TTYPE_OPER) {
         if (strcmp (myRPN.l_name, "." ) == 0)  myRPN.t_type = S_TTYPE_MEMB;
         if (strcmp (myRPN.l_name, "->") == 0)  myRPN.t_type = S_TTYPE_MEMB;
      }
      if (myRPN.t_name [0] == '‘')              myRPN.t_type = S_TTYPE_LOCAL;
      strlcpy (myRPN.t_token, myRPN.t_name, LEN_FULL);
      yRPN_stack_tokens  ();         /* strait to tokens list                          */
      yRPN__token_save    (a_pos);
   }
   myRPN.left_oper  = S_OPER_CLEAR;
   myRPN.combined   = S_NO;
   /*---(complete)-------------------------*/
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
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
   DEBUG_YRPN     yLOG_note    ("special pointer handling");
   DEBUG_YRPN     yLOG_char    ("line_type" , myRPN.line_type);
   rc = yRPN_stack_peek ();
   DEBUG_YRPN     yLOG_info    ("p_name"    , myRPN.p_name);
   DEBUG_YRPN     yLOG_char    ("p_type"    , myRPN.p_type);
   if (rc >= 0 && myRPN.p_type == S_TTYPE_CAST) {
      DEBUG_YRPN     yLOG_note    ("working in * casting mode");
      strlcpy (myRPN.t_name, "(*)", LEN_LABEL);
      myRPN.t_type = S_TTYPE_CAST;
      myRPN.t_prec = S_PREC_FUNC;
      myRPN.t_dir  = S_RIGHT;
      DEBUG_YRPN     yLOG_info    ("t_name"    , myRPN.t_name);
      DEBUG_YRPN     yLOG_char    ("t_type"    , myRPN.t_type);
      yRPN_stack_push       (a_pos);
      myRPN.left_oper  = S_OPER_CLEAR;
   } else if (rc >= 0 && (myRPN.line_type == S_LINE_DEF_FPTR || myRPN.line_type == S_LINE_DEF_FUN || myRPN.line_type == S_LINE_DEF_PRO)) {
      DEBUG_YRPN     yLOG_note    ("working in * type modifier mode");
      strlcpy (myRPN.t_name, "(*)", LEN_LABEL);
      myRPN.t_type = S_TTYPE_PTYPE;
      DEBUG_YRPN     yLOG_info    ("t_name"    , myRPN.t_name);
      DEBUG_YRPN     yLOG_char    ("t_type"    , myRPN.t_type);
      yRPN__token_save    (a_pos);
      myRPN.left_oper  = S_OPER_CLEAR;
   } else if (myRPN.line_type == S_LINE_DEF_FPTR) {
      DEBUG_YRPN     yLOG_note    ("working in function pointer mode");
      strlcpy (myRPN.t_name, "(>", LEN_LABEL);
      myRPN.t_type = S_TTYPE_FPTR;
      DEBUG_YRPN     yLOG_info    ("t_name"    , myRPN.t_name);
      DEBUG_YRPN     yLOG_char    ("t_type"    , myRPN.t_type);
      yRPN__token_save    (a_pos);
      myRPN.left_oper  = S_OPER_CLEAR;
   } else if ((myRPN.line_type == S_LINE_DEF || myRPN.line_type == S_LINE_DEF_VAR) && myRPN.line_sect != '=') {
      DEBUG_YRPN     yLOG_note    ("working in * type modifier mode");
      strlcpy (myRPN.t_name, "(*)", LEN_LABEL);
      myRPN.t_type = S_TTYPE_TYPE;
      DEBUG_YRPN     yLOG_info    ("t_name"    , myRPN.t_name);
      DEBUG_YRPN     yLOG_char    ("t_type"    , myRPN.t_type);
      yRPN__token_save    (a_pos);
      myRPN.left_oper  = S_OPER_LEFT;
   } else {
      DEBUG_YRPN     yLOG_note    ("working in * dereference mode");
      DEBUG_YRPN     yLOG_info    ("t_name"    , myRPN.t_name);
      DEBUG_YRPN     yLOG_char    ("t_type"    , myRPN.t_type);
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
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yRPN__token_error ();
   DEBUG_YRPN     yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos <  0) {
      DEBUG_YRPN     yLOG_note    ("start can not be negative");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN     yLOG_note    ("accumulate characters");
   myRPN.t_type   = S_TTYPE_OPER;
   x_pos        = a_pos;  /* starting point */
   while (yrpn_token_add (&x_pos) == 0);
   DEBUG_YRPN     yLOG_info    ("myRPN.t_name", myRPN.t_name);
   /*---(try to match normal operators)----*/
   DEBUG_YRPN     yLOG_note    ("search operators normal");
   for (i = 0; i < MAX_OPER; ++i) {
      if  (s_opers [i].name [0] == '\0')                            break;
      if  (s_opers [i].real     != 'r' )                            continue;
      if  (s_opers [i].who != zRPN_lang && s_opers [i].who != 'B')  continue;
      if  (s_opers [i].name [0] != myRPN.t_name [0])                continue;
      if  (strcmp (s_opers [i].name, myRPN.t_name ) != 0)           continue;
      x_found = i;
      DEBUG_YRPN     yLOG_value   ("x_found"   , x_found);
      break;
   }
   /*---(try to match pretty operators)----*/
   if (x_found < 0 && myRPN.t_len == 1) {
      DEBUG_YRPN     yLOG_note    ("search operators pretty");
      for (i = 0; i < MAX_OPER; ++i) {
         if  (s_opers [i].name [0] == '\0')                            break;
         if  (s_opers [i].real     != 'r' )                            continue;
         if  (s_opers [i].who != zRPN_lang && s_opers [i].who != 'B')  continue;
         if  (s_opers [i].pretty [0] != myRPN.t_name [0])              continue;
         x_found = i;
         strlcpy (myRPN.t_name, s_opers [i].name, LEN_LABEL);
         myRPN.t_len = strlen (myRPN.t_name);
         DEBUG_YRPN     yLOG_value   ("x_found"   , x_found);
         break;
      }
   }
   /*---(try singles (if double))----------*/
   if (x_found < 0 && myRPN.t_len == 2) {
      DEBUG_YRPN     yLOG_note    ("two character operator not found");
      DEBUG_YRPN     yLOG_note    ("switching to single character operator mode");
      myRPN.t_name [1] = '\0';
      myRPN.t_len      = 1;
      --x_pos;
      for (i = 0; i < MAX_OPER; ++i) {
         if  (s_opers [i].name [0] == '\0')                   break;
         if  (s_opers [i].real     != 'r' )                   continue;
         if  (s_opers [i].who != zRPN_lang && s_opers [i].who != 'B')  continue;
         if  (s_opers [i].name [0] != myRPN.t_name [0])         continue;
         if  (strcmp (s_opers [i].name, myRPN.t_name ) != 0)           continue;
         x_found = i;
         DEBUG_YRPN     yLOG_value   ("x_found"   , x_found);
         break;
      }
   }
   /*---(handle misses)--------------------*/
   --rce;  if (x_found < 0) {
      yRPN__token_error ();
      DEBUG_YRPN     yLOG_note    ("operator not found");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save original)--------------------*/
   strlcpy (myRPN.t_token, myRPN.t_name, LEN_FULL);
   /*---(check for unary)------------------*/
   if (myRPN.left_oper  == S_OPER_LEFT) {
      DEBUG_YRPN     yLOG_note    ("check for left operators in right only mode");
      if      (strcmp (myRPN.t_name, "+" ) == 0)  { strcpy (myRPN.t_name, "+:"); myRPN.t_len  = 2; }
      else if (strcmp (myRPN.t_name, "-" ) == 0)  { strcpy (myRPN.t_name, "-:"); myRPN.t_len  = 2; }
      else if (strcmp (myRPN.t_name, "*" ) == 0)  { strcpy (myRPN.t_name, "*:"); myRPN.t_len  = 2; }
      else if (strcmp (myRPN.t_name, "&" ) == 0)  { strcpy (myRPN.t_name, "&:"); myRPN.t_len  = 2; }
   } else {
      DEBUG_YRPN     yLOG_note    ("check for right operators in left only mode");
      if      (strcmp (myRPN.t_name, "++") == 0)  { strcpy (myRPN.t_name, ":+"); myRPN.t_len  = 2; }
      else if (strcmp (myRPN.t_name, "--") == 0)  { strcpy (myRPN.t_name, ":-"); myRPN.t_len  = 2; }
   }
   /*---(handle it)------------------------*/
   yRPN__prec ();
   yRPN_stack_tokens      ();
   if        (myRPN.pproc == S_PPROC_YES) {
      DEBUG_YRPN     yLOG_note    ("pre-processor directive");
      yRPN__token_save    (a_pos);
      myRPN.left_oper  = S_OPER_LEFT;  /* an oper after an oper must be right-only */
   } else if (strcmp (myRPN.t_name, "*:") == 0) {
      DEBUG_YRPN     yLOG_note    ("working with a pointer");
      yRPN__oper_splat  (a_pos);
      /*> } else if (myRPN.combined == S_YES && myRPN.t_dir == S_RIGHT && myRPN.t_arity == 1) {   <* 
       *>    DEBUG_YRPN     yLOG_note    ("working with normal operators");                        <* 
       *>    yRPN_stack_oper       (a_pos);                                                       <*/
} else {
   DEBUG_YRPN     yLOG_note    ("working with normal operators");
   yRPN_stack_oper       (a_pos);
   if (myRPN.t_post == S_YES) {
      DEBUG_YRPN     yLOG_note    ("working with post-unary, any operator next");
      myRPN.left_oper  = S_OPER_CLEAR;
      myRPN.combined   = S_NO;
   } else {
      DEBUG_YRPN     yLOG_note    ("switch to left-only operators for next run");
      myRPN.left_oper  = S_OPER_LEFT;  /* an oper after an oper must be right-only */
      myRPN.combined   = myRPN.t_comb;
   }
}
/*---(complete)-------------------------*/
DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
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
   if (zRPN_lang != YRPN_GYGES) {
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
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yRPN__token_error ();
   DEBUG_YRPN     yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos <  0) {
      DEBUG_YRPN     yLOG_note    ("start can not be negative");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (strchr (v_sequence, myRPN.working [a_pos]) == 0) {
      DEBUG_YRPN     yLOG_note    ("not a valid grouping symbol");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (zRPN_lang != 'c' && strchr ("[]", myRPN.working [a_pos]) != 0) {
      DEBUG_YRPN     yLOG_note    ("braces not allowed in spreadsheet mode");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN     yLOG_note    ("accumulate characters");
   myRPN.t_name [0] = myRPN.working [a_pos];
   myRPN.t_name [1] = '\0';
   myRPN.t_len      = 1;
   myRPN.t_type     = S_TTYPE_GROUP;
   strlcpy (myRPN.t_token, myRPN.t_name, LEN_FULL);
   x_pos          = a_pos + 1;
   DEBUG_YRPN     yLOG_info    ("myRPN.t_name", myRPN.t_name);
   DEBUG_YRPN     yLOG_value   ("myRPN.t_len" , myRPN.t_len);
   yRPN__prec ();
   /*---(handle)---------------------------*/
   DEBUG_YRPN     yLOG_char    ("line_type" , myRPN.line_type);
   switch (myRPN.t_name [0]) {
   case '(' :
      ++myRPN.paren_lvl;
      yRPN_stack_tokens  ();         /* strait to tokens list                          */
      rc = yRPN_stack_peek    ();
      if (rc >= 0) {
         DEBUG_YRPN     yLOG_note    ("something in stack");
         DEBUG_YRPN     yLOG_info    ("myRPN.p_name", myRPN.p_name);
         DEBUG_YRPN     yLOG_char    ("myRPN.p_type", myRPN.p_type);
         if (myRPN.p_type == S_TTYPE_FUNC || strcmp (myRPN.p_name, "sizeof") == 0) {
            DEBUG_YRPN     yLOG_note    ("function openning");
            /*> strlcpy (myRPN.t_name, "(>", LEN_LABEL);                              <*/
            myRPN.t_type = S_TTYPE_FUNC;
            yRPN__token_push (a_pos);
            myRPN.left_oper  = S_OPER_LEFT;
         } else {
            DEBUG_YRPN     yLOG_note    ("normal parenthesis");
            /*> strlcpy (myRPN.t_name, "(:", LEN_LABEL);                              <* 
             *> myRPN.t_type = S_TTYPE_CAST;                                            <* 
             *> myRPN.t_prec = S_PREC_FUNC;                                             <* 
             *> yRPN__token_push (a_pos);                                             <* 
             *> myRPN.left_oper  = S_OPER_LEFT;                                         <*/
            yRPN__token_push (a_pos);
            myRPN.left_oper  = S_OPER_LEFT;
         }
      } else {  /* clever pointer stuff */
         DEBUG_YRPN     yLOG_note    ("empty stack");
         if (myRPN.line_type == S_LINE_NORMAL) {
            DEBUG_YRPN     yLOG_note    ("normal parenthesis");
            yRPN__token_push (a_pos);
            myRPN.left_oper  = S_OPER_LEFT;
         } else {
            DEBUG_YRPN     yLOG_note    ("function pointer paren (ignore)");
            /*> strlcpy (myRPN.t_name, "(>", LEN_LABEL);                              <* 
             *> myRPN.t_type = S_TTYPE_FPTR;                                            <* 
             *> yRPN__token_push (a_pos);                                             <*/
            myRPN.left_oper  = S_OPER_LEFT;
            myRPN.line_type  = S_LINE_DEF_FPTR;
         }
      }
      break;
   case ')' :
      x_type  = myRPN.s_type;
      DEBUG_YRPN     yLOG_char    ("x_type"    , x_type);
      DEBUG_YRPN     yLOG_char    ("line_type" , myRPN.line_type);
      if (myRPN.line_type == S_LINE_DEF_FPTR || myRPN.line_type == S_LINE_DEF_PRO) {
         if (x_type == S_TTYPE_PTYPE) {
            strlcpy (myRPN.t_name, "?", LEN_LABEL);
            myRPN.t_type = S_TTYPE_VARS;
            myRPN.t_prec = S_PREC_NONE;
            yRPN_stack_tokens      ();
            yRPN__token_save (a_pos);
            strlcpy (myRPN.t_name, ")", LEN_LABEL);
            myRPN.t_type = S_TTYPE_GROUP;
            yRPN__prec ();
         }
      }
      --myRPN.paren_lvl;
      yRPN_stack_tokens     ();
      if (myRPN.s_type == S_TTYPE_FPTR &&  myRPN.line_type == S_LINE_DEF_FPTR) {
         DEBUG_YRPN     yLOG_note    ("function pointer paren (ignore)");
      } else {
         DEBUG_YRPN     yLOG_note    ("normal closing parens");
         yRPN_stack_normal     (a_pos);
         yRPN_stack_paren      (a_pos);
      }
      myRPN.left_oper  = S_OPER_CLEAR;
      break;
   case '[' :
      DEBUG_YRPN     yLOG_note    ("openning brace");
      yRPN_stack_tokens     ();
      yRPN__token_push      (a_pos);
      myRPN.left_oper  = S_OPER_LEFT;
      break;
   case ']' :
      DEBUG_YRPN     yLOG_note    ("closing brace");
      yRPN_stack_tokens     ();
      yRPN_stack_paren      (a_pos);
      myRPN.t_type     = S_TTYPE_OPER;
      yRPN_stack_shuntd     ();
      break;
   case ',' :
      DEBUG_YRPN     yLOG_note    ("comma");
      x_type  = myRPN.s_type;
      myRPN.line_sect = '-';
      DEBUG_YRPN     yLOG_char    ("x_type"    , x_type);
      DEBUG_YRPN     yLOG_char    ("line_sect" , myRPN.line_sect);
      if (myRPN.line_type == S_LINE_DEF_FPTR || myRPN.line_type == S_LINE_DEF_PRO) {
         if (x_type == S_TTYPE_PTYPE) {
            strlcpy (myRPN.t_name, "?", LEN_LABEL);
            myRPN.t_type = S_TTYPE_VARS;
            myRPN.t_prec = S_PREC_NONE;
            yRPN_stack_tokens      ();
            yRPN__token_save (a_pos);
            strlcpy (myRPN.t_name, ",", LEN_LABEL);
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
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
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
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   /*---(defenses)-----------------------*/
   yRPN__token_error ();
   DEBUG_YRPN     yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos <  0) {
      DEBUG_YRPN     yLOG_note    ("start can not be negative");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   --rce;  if (strchr (v_enders  , myRPN.working [a_pos]) == 0) {
      DEBUG_YRPN     yLOG_note    ("not a valid grouping symbol");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN     yLOG_note    ("accumulate characters");
   myRPN.t_name [0] = myRPN.working [a_pos];
   myRPN.t_name [1] = '\0';
   myRPN.t_len      = 1;
   myRPN.t_type     = S_TTYPE_GROUP;
   strlcpy (myRPN.t_token, myRPN.t_name, LEN_FULL);
   x_pos          = a_pos + 1;
   DEBUG_YRPN     yLOG_info    ("myRPN.t_name", myRPN.t_name);
   DEBUG_YRPN     yLOG_value   ("myRPN.t_len" , myRPN.t_len);
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
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return x_pos;
}




/*============================----end-of-source---============================*/
