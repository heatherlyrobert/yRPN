/*============================----beg-of-source---============================*/
#include  "yRPN.h"
#include  "yRPN_priv.h"



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
   /*-range--------- who real off prec    --dir--  ar -comb-  -post-    -pre-- -suf- pretty   --comment-----------------------------*/
   { ".."  , ".."  , 'g', 'r',  0, 'd',    S_LEFT , 2, S_YES, S_NO ,    S_NO , S_NO , ""  ,   "cell range"                           },
   /*-preproc------- who real off prec    --dir--  ar -comb-  -post-    -pre-- -suf- pretty   --comment-----------------------------*/
   { "#"   , "#"   , 'c', '-',  0, 'd',    S_LEFT , 2, S_NO , S_NO ,    S_NO , S_NO , ""  ,   "prefix"                               },
   { "##"  , "##"  , 'c', '-',  0, 'd',    S_LEFT , 2, S_NO , S_NO ,    S_NO , S_NO , ""  ,   "stringification"                      },
   /*-unary/suffix-- who real off prec    --dir--  ar -comb-  -post-    -pre-- -suf- pretty   --comment-----------------------------*/
   { ":+"  , "++"  , 'B', 'I',  1, 'e',    S_LEFT , 1, S_NO , S_YES,    S_NO , S_YES, ""  ,   "postfix increment"                    },
   { ":-"  , "--"  , 'B', 'I',  1, 'e',    S_LEFT , 1, S_NO , S_YES,    S_NO , S_YES, ""  ,   "postfix decrement"                    },
   { "Ë"   , "Ë"   , 'B', 'r',  1, 'e',    S_LEFT , 2, S_YES, S_NO ,    S_NO , S_NO , ""  ,   "scientific exponent"                  },
   { "Ë-"  , "Ë-"  , 'B', 'r',  1, 'e',    S_LEFT , 2, S_YES, S_NO ,    S_NO , S_NO , ""  ,   "scientific exponent"                  },
   { "Ë+"  , "Ë+"  , 'B', 'r',  1, 'e',    S_LEFT , 2, S_YES, S_NO ,    S_NO , S_NO , ""  ,   "scientific exponent"                  },
   { "Æ"   , "Æ"   , 'B', 'r',  1, 'e',    S_LEFT , 1, S_NO , S_YES,    S_NO , S_YES, ""  ,   "power of two, squared"                },
   { "Ç"   , "Ç"   , 'B', 'r',  1, 'e',    S_LEFT , 1, S_NO , S_YES,    S_NO , S_YES, ""  ,   "power of three, cubed"                },
   { "È"   , "È"   , 'B', 'r',  1, 'e',    S_LEFT , 1, S_NO , S_YES,    S_NO , S_YES, ""  ,   "power of four"                        },
   { "É"   , "É"   , 'B', 'r',  1, 'e',    S_LEFT , 1, S_NO , S_YES,    S_NO , S_YES, ""  ,   "power of x"                           },
   { "Ê"   , "Ê"   , 'B', 'r',  1, 'e',    S_LEFT , 1, S_NO , S_YES,    S_NO , S_YES, ""  ,   "power of y"                           },
   { "`"   , "`"   , 'B', 'r',  1, 'e',    S_LEFT , 2, S_YES, S_NO ,    S_NO , S_NO , ""  ,   "arbitrary power"                      },
   { "Î"   , "Î"   , 'B', 'r',  1, 'e',    S_LEFT , 1, S_NO , S_YES,    S_NO , S_YES, ""  ,   "power of half, square root"           },
   { "Í"   , "Í"   , 'B', 'r',  1, 'e',    S_LEFT , 1, S_NO , S_YES,    S_NO , S_YES, ""  ,   "radian operator"                      },
   { "Ì"   , "Ì"   , 'B', 'r',  1, 'e',    S_LEFT , 1, S_NO , S_YES,    S_NO , S_YES, ""  ,   "degree operator"                      },
   /*-element-of---- who real off prec    --dir--  ar -comb-  -post-    -pre-- -suf- pretty   --comment-----------------------------*/
   { "["   , "["   , 'c', 'r',  1, 'e',    S_LEFT , 1, S_YES, S_NO ,    S_YES, S_NO , ""  ,   "array subscripting"                   },
   { "]"   , "]"   , 'c', 'r',  1, 'e',    S_LEFT , 1, S_YES, S_NO ,    S_NO , S_YES, ""  ,   "array subscripting"                   },
   { "."   , "."   , 'c', 'r',  1, 'e',    S_LEFT , 2, S_YES, S_NO ,    S_NO , S_NO , ""  ,   "element selection by reference"       },
   { "->"  , "->"  , 'c', 'r',  1, 'e',    S_LEFT , 2, S_YES, S_NO ,    S_NO , S_NO , "Ö" ,   "element selection thru pointer"       },
   /*-unary/prefix-- who real off prec    --dir--  ar -comb-  -post-    -pre-- -suf- pretty   --comment-----------------------------*/
   { "++"  , "++"  , 'B', 'r',  2, 'f',    S_RIGHT, 1, S_NO , S_NO ,    S_YES, S_NO , ""  ,   "prefix increment"                     },
   { "--"  , "--"  , 'B', 'r',  2, 'f',    S_RIGHT, 1, S_NO , S_NO ,    S_YES, S_NO , ""  ,   "prefix decrement"                     },
   { "+:"  , "+"   , 'B', 'I',  2, 'f',    S_RIGHT, 1, S_NO , S_NO ,    S_YES, S_NO , ""  ,   "unary plus"                           },
   { "-:"  , "-"   , 'B', 'I',  2, 'f',    S_RIGHT, 1, S_NO , S_NO ,    S_YES, S_NO , ""  ,   "unary minus"                          },
   { "!"   , "!"   , 'B', 'r',  2, 'f',    S_RIGHT, 1, S_NO , S_NO ,    S_YES, S_NO , ""  ,   "logical NOT"                          },
   { "~"   , "~"   , 'B', 'r',  2, 'f',    S_LEFT , 2, S_YES, S_NO ,    S_NO , S_NO , ""  ,   "bitwise NOT (len limited)"            },
   { "á"   , "*"   , 'B', 'I',  2, 'f',    S_RIGHT, 1, S_NO , S_NO ,    S_YES, S_NO , "á" ,   "indirection/dereference"              },
   { "á"   , "á"   , 'B', 'r',  2, 'f',    S_RIGHT, 1, S_NO , S_NO ,    S_YES, S_NO , ""  ,   "indirection/dereference"              },
   { "&:"  , "&"   , 'B', 'I',  2, 'f',    S_RIGHT, 1, S_NO , S_NO ,    S_YES, S_NO , ""  ,   "address-of"                           },
   { "(*)" , "*"   , 'c', 'I',  2, 'f',    S_RIGHT, 1, S_NO , S_NO ,    S_YES, S_YES, ""  ,   "casting modifier"                     },
   /*-mult-family--- who real off prec    --dir--  ar -comb-  -post-    -pre-- -suf- pretty   --comment-----------------------------*/
   { "*"   , "*"   , 'B', 'r',  3, 'g',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "multiplication"                       },
   { "´"   , "´"   , 'B', 'r',  3, 'g',    S_LEFT , 2, S_YES, S_NO ,    S_NO , S_NO , ""  ,   "multiplication"                       },
   { "/"   , "/"   , 'B', 'r',  3, 'g',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "division"                             },
   { "%"   , "%"   , 'B', 'r',  3, 'g',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "modulus"                              },
   { "©-"  , "©-"  , 'g', 'r',  3, 'g',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "sub-string removal"                   },
   { "©*"  , "©*"  , 'g', 'r',  3, 'g',    S_LEFT , 2, S_YES, S_NO ,    S_NO , S_NO , ""  ,   "word parsing"                         },
   { "©´"  , "©´"  , 'g', 'r',  3, 'g',    S_LEFT , 2, S_YES, S_NO ,    S_NO , S_NO , ""  ,   "list parsing"                         },
   { "©/"  , "©/"  , 'g', 'r',  3, 'g',    S_LEFT , 2, S_YES, S_NO ,    S_NO , S_NO , ""  ,   "field parsing"                        },
   { "©¬"  , "©¬"  , 'g', 'r',  3, 'g',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "character masking"                    },
   { "©°"  , "©°"  , 'g', 'r',  3, 'g',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "character redaction"                  },
   /*-add-family---- who real off prec    --dir--  ar -comb-  -post-    -pre-- -suf- pretty   --comment-----------------------------*/
   { "+"   , "+"   , 'B', 'r',  4, 'h',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "addition"                             },
   { "-"   , "-"   , 'B', 'r',  4, 'h',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "substraction"                         },
   { "©"   , "©"   , 'g', 'r',  4, 'h',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "string concatination"                 },
   { "©©"  , "©©"  , 'g', 'r',  4, 'h',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "string concatination"                 },
   { "©+"  , "©+"  , 'g', 'r',  4, 'h',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "string concatination"                 },
   /*-shift--------- who real off prec    --dir--  ar -comb-  -post-    -pre-- -suf- pretty   --comment-----------------------------*/
   { "<<"  , "<<"  , 'B', 'r',  5, 'i',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "bitwise shift left"                   },
   { ">>"  , ">>"  , 'B', 'r',  5, 'i',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "bitwise shift right"                  },
   /*-relational---- who real off prec    --dir--  ar -comb-  -post-    -pre-- -suf- pretty   --comment-----------------------------*/
   { "<"   , "<"   , 'B', 'r',  6, 'j',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "relational lesser"                    },
   { "<="  , "<="  , 'B', 'r',  6, 'j',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, "Ü" ,   "relational less or equal"             },
   { ">"   , ">"   , 'B', 'r',  6, 'j',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "relational greater"                   },
   { ">="  , ">="  , 'B', 'r',  6, 'j',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, "Ý" ,   "relational more or equal"             },
   { "©<"  , "©<"  , 'g', 'r',  6, 'j',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "relational string lesser"             },
   { "©>"  , "©>"  , 'g', 'r',  6, 'j',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "relational string greater"            },
   { "©Ü"  , "©Ü"  , 'g', 'r',  6, 'j',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "relational string lesser"             },
   { "©Ý"  , "©Ý"  , 'g', 'r',  6, 'j',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "relational string greater"            },
   { "â"   , "â"   , 'g', 'r',  6, 'j',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "sub-string found"                     },
   { "ã"   , "ã"   , 'g', 'r',  6, 'j',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "list item found"                      },
   /*-equality------ who real off prec    --dir--  ar -comb-  -post-    -pre-- -suf- pretty   --comment-----------------------------*/
   { "=="  , "=="  , 'B', 'r',  7, 'k',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, "à"    , "relational equality"                  },
   { "!="  , "!="  , 'B', 'r',  7, 'k',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, "Þ" ,   "relational inequality"                },
   { "©="  , "©="  , 'g', 'r',  7, 'k',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "relational string equality"           },
   { "©!"  , "©!"  , 'g', 'r',  7, 'k',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "relational string inequality"         },
   /*-bitwise------- who real off prec    --dir--  ar -comb-  -post-    -pre-- -suf- pretty   --comment-----------------------------*/
   { "&"   , "&"   , 'B', 'r',  8, 'l',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "bitwise AND"                          },
   { "^"   , "^"   , 'B', 'r',  9, 'm',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "bitwise XOR"                          },
   { "|"   , "|"   , 'B', 'r', 10, 'n',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "bitwise OR"                           },
   /*-logical------- who real off prec    --dir--  ar -comb-  -post-    -pre-- -suf- pretty   --comment-----------------------------*/
   { "&&"  , "&&"  , 'B', 'r', 11, 'o',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, "Ð" ,   "logical AND"                          },
   { "!&"  , "!&"  , 'B', 'r', 11, 'o',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, "Ô" ,   "NAND (at least one is false)"         },
   { "||"  , "||"  , 'B', 'r', 12, 'p',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, "Ñ" ,   "logical OR"                           },
   { "!|"  , "!|"  , 'B', 'r', 12, 'p',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, "Õ" ,   "NOR (neither/nor)"                    },
   { "&|"  , "&|"  , 'B', 'r', 12, 'p',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, "Ò" ,   "XOR (one and only one)"               },
   { "|&"  , "|&"  , 'B', 'r', 12, 'p',    S_LEFT , 2, S_YES, S_NO ,    S_YES, S_YES, "Ó" ,   "NXOR (both the same)"                 },
   /*-conditional--- who real off prec    --dir--  ar -comb-  -post-    -pre-- -suf- pretty   --comment-----------------------------*/
   { "?"   , "?"   , 'c', 'r', 13, 'q',    S_RIGHT, 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "trinary conditional"                  },
   { ":"   , ":"   , 'c', 'r', 13, 'q',    S_RIGHT, 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "trinary conditional"                  },
   /*-assignment---- who real off prec    --dir--  ar -comb-  -post-    -pre-- -suf- pretty   --comment-----------------------------*/
   { "="   , "="   , 'B', 'r', 14, 'r',    S_RIGHT, 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "direct numeric assignment"            },
   { "+="  , "+="  , 'c', 'r', 14, 'r',    S_RIGHT, 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "compound assignment (addition)"       },
   { "-="  , "-="  , 'c', 'r', 14, 'r',    S_RIGHT, 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "compound assignment (subtract)"       },
   { "*="  , "*="  , 'c', 'r', 14, 'r',    S_RIGHT, 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "compound assignment (multiply)"       },
   { "/="  , "/="  , 'c', 'r', 14, 'r',    S_RIGHT, 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "compound assignment (divide)"         },
   { "%="  , "%="  , 'c', 'r', 14, 'r',    S_RIGHT, 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "compound assignment (modulus)"        },
   { "<<=" , "<<=" , 'c', 'r', 14, 'r',    S_RIGHT, 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "compound assignment (bitwise left)"   },
   { ">>=" , ">>=" , 'c', 'r', 14, 'r',    S_RIGHT, 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "compound assignment (bitwise right)"  },
   { "&="  , "&="  , 'c', 'r', 14, 'r',    S_RIGHT, 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "compound assignment (bitwise AND)"    },
   { "^="  , "^="  , 'c', 'r', 14, 'r',    S_RIGHT, 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "compound assignment (bitwise XOR)"    },
   { "|="  , "|="  , 'c', 'r', 14, 'r',    S_RIGHT, 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "compound assignment (bitwise OR)"     },
   { "#"   , "#"   , 'B', 'r', 14, 'r',    S_RIGHT, 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "direct string assignment"             },
   { "©#"  , "©#"  , 'c', 'r', 14, 'r',    S_RIGHT, 2, S_YES, S_NO ,    S_YES, S_YES, ""  ,   "concatinate and assign"               },
   /*-comma-family-- who real off prec    --dir--  ar -comb-  -post-    -pre-- -suf- pretty   --comment-----------------------------*/
   { ",;"  , ";"   , 'c', 'r', 15, 'u',    S_LEFT , 1, S_YES, S_NO ,    S_NO , S_YES, ""  ,   "sequence mega-separator"              },
   { ","   , ","   , 'B', 'r', 15, 's',    S_LEFT , 2, S_YES, S_NO ,    S_NO , S_YES, ""  ,   "sequence separator"                   },
   /*-parens-------- who real off prec    --dir--  ar -comb-  -post-    -pre-- -suf- pretty   --comment-----------------------------*/
   { "("   , "("   , 'B', 'r', 16, 't',    S_LEFT , 1, S_NO , S_NO ,    S_YES, S_NO , ""  ,   "sequence openning"                    },
   { ")"   , ")"   , 'B', 'r', 16, 't',    S_LEFT , 1, S_NO , S_NO ,    S_NO , S_YES, ""  ,   "sequence closing"                     },
   { "¸"   , "¸"   , 'B', 'r', 16, 't',    S_LEFT , 1, S_NO , S_NO ,    S_YES, S_NO , ""  ,   "math sequence openning"               },
   { "¹"   , "¹"   , 'B', 'r', 16, 't',    S_LEFT , 1, S_NO , S_NO ,    S_NO , S_YES, ""  ,   "math sequence closing"                },
   { "(>"  , "("   , 'c', 'I', 16, 't',    S_LEFT , 1, S_YES, S_NO ,    S_YES, S_NO , "¼" ,   "function opening"                     },
   { "(:"  , "("   , 'c', 'I', 16, 't',    S_LEFT , 1, S_YES, S_NO ,    S_YES, S_NO , ""  ,   "casting opening"                      },
   { "):"  , ")"   , 'c', 'I', 16, 't',    S_LEFT , 1, S_YES, S_NO ,    S_NO , S_YES, ""  ,   "casting closing"                      },
   /*-semicolon----- who real off prec    --dir--  ar -comb-  -post-    -pre-- -suf- pretty   --comment-----------------------------*/
   { ";"   , ";"   , 'c', 'r', 17, 'u',    S_LEFT , 1, S_NO , S_NO ,    S_NO , S_YES, ""  ,   "statement separator"                  },
   /*-braces-------- who real off prec    --dir--  ar -comb-  -post-    -pre-- -suf- pretty   --comment-----------------------------*/
   { "{"   , "{"   , 'c', 'r', 18, 'v',    S_LEFT , 1, S_NO , S_NO ,    S_YES, S_YES, ""  ,   "function openning"                    },
   { "}"   , "}"   , 'c', 'r', 18, 'v',    S_LEFT , 1, S_NO , S_NO ,    S_YES, S_YES, ""  ,   "function closing"                     },
   /*-constants----- who real off prec    --dir--  ar -comb-  -post-    -pre-- -suf- pretty   --comment-----------------------------*/
   /*> { "Û"   , "Û"   , 'g', 'r', 19, 'c',    S_LEFT , 0, S_NO , S_NO ,    S_YES, S_YES, ""  ,   "true"                                 },   <*/
   /*> { "Ú"   , "Ú"   , 'g', 'r', 19, 'c',    S_LEFT , 0, S_NO , S_NO ,    S_YES, S_YES, ""  ,   "false"                                },   <*/
   /*-sentinel------ who real off prec    --dir--  ar -comb-  -post-    -pre-- -suf- pretty   --comment-----------------------------*/
   { ""    , ""    , '-', '-',  0, '-',    '-'    , 0, S_NO , S_NO ,    S_NO , S_NO , ""  ,   ""                                     },
   /*-done---------- who real off prec    --dir--  ar -comb-  -post-    -pre-- -suf- pretty --comment-----------------------------*/
};

char s_one      [LEN_HUND] = "";
char s_two      [LEN_HUND] = "";
char s_thr      [LEN_HUND] = "";

char
yrpn_oper_init          (void)
{
   int         i           =    0;     /* iterator for keywords               */
   int         l           =    0;
   char        t           [LEN_SHORT] = "";
   char        c           =  '-';
   char       *p           = NULL;
   DEBUG_YRPN     yLOG_enter   (__FUNCTION__);
   strlcpy (s_one, "", LEN_HUND);
   strlcpy (s_two, "", LEN_HUND);
   strlcpy (s_thr, "", LEN_HUND);
   for (i = 0; i < MAX_OPER; ++i) {
      if (s_opers [i].name [0] == '\0')                            break;
      if (s_opers [i].real     != 'r' )                            continue;
      l = strlen (s_opers [i].orig);;
      if (l == 1 && strchr ("()¸¹[]{},;", s_opers [i].orig [0]) != NULL)  continue;
      /*---(original)--------------------*/
      c = s_opers [i].orig [0];
      if (c != '\0') {
         sprintf (t, "%c", c);
         p = strchr (s_one, c);
         if (p == NULL)  strlcat (s_one, t, LEN_HUND);
      }
      c = s_opers [i].orig [1];
      if (c != '\0') {
         sprintf (t, "%c", c);
         p = strchr (s_two, c);
         if (p == NULL)  strlcat (s_two, t, LEN_HUND);
      }
      c = s_opers [i].orig [2];
      if (c != '\0') {
         sprintf (t, "%c", c);
         p = strchr (s_thr, c);
         if (p == NULL)  strlcat (s_thr, t, LEN_HUND);
      }
      /*---(pretty)----------------------*/
      c = s_opers [i].pretty [0];
      if (c != '\0') {
         sprintf (t, "%c", c);
         p = strchr (s_one, c);
         strlcat (s_one, t, LEN_HUND);
      }
      /*---(done)------------------------*/
   }
   DEBUG_YRPN     yLOG_info    ("s_one"     , s_one);
   DEBUG_YRPN     yLOG_info    ("s_two"     , s_two);
   DEBUG_YRPN     yLOG_info    ("s_thr"     , s_thr);
   DEBUG_YRPN     yLOG_exit    (__FUNCTION__);
   return 0;
}

char
yrpn_oper_wrap          (void)
{
   strlcpy (s_one, "", LEN_HUND);
   strlcpy (s_two, "", LEN_HUND);
   strlcpy (s_thr, "", LEN_HUND);
   return 0;
}

short
yrpn_oper__by_name      (char l, char a_name [LEN_SHORT])
{
   int         i           =    0;     /* iterator for keywords               */
   for (i = 0; i < MAX_OPER; ++i) {
      if  (s_opers [i].name [0] == '\0')              return -1;
      if  (s_opers [i].real     == '-' )              continue;
      /*---(original)--------------------*/
      if (l <= 0) {
         if (strcmp  (a_name, s_opers [i].name)       == 0)   return i;
         if (strcmp  (a_name, s_opers [i].pretty)     == 0)   return i;
      } else {
         if (strncmp (a_name, s_opers [i].name  , l)  == 0)   return i;
         if (strcmp  (a_name, s_opers [i].pretty)     == 0)   return i;
      }
      /*---(done)------------------------*/
   }
   return -1;
}

char       /* ---- : identify the symbol spacing -----------------------------*/
yrpn_oper_spacing       (char  *a_name, char *r_pre, char *r_suf, char r_pretty [LEN_SHORT])
{
   /*---(locals)-----------+-----+-----+-*/
   char        rce         =  -10;
   int         i           =    0;
   /*---(defaults)----------------*/
   if (r_pre    != NULL)  *r_pre = S_YES;
   if (r_suf    != NULL)  *r_suf = S_YES;
   if (r_pretty != NULL)  strlcpy (r_pretty, "", LEN_SHORT);
   --rce;  if (a_name == NULL)  return rce;
   /*---(find symbol)-------------*/
   i = yrpn_oper__by_name (-1, a_name);
   --rce;  if (i < 0)  return rce;
   /*---(save-back)---------------*/
   if (r_pre    != NULL)  *r_pre = s_opers [i].pre;
   if (r_suf    != NULL)  *r_suf = s_opers [i].suf;
   if (r_pretty != NULL)  strlcpy (r_pretty, s_opers [i].pretty, LEN_SHORT);
   /*---(complete)----------------*/
   return 0;
}



/*====================------------------------------------====================*/
/*===----                  single character validation                 ----===*/
/*====================------------------------------------====================*/
static void        o___SINGLES_________________o (void) {;}

char
yrpn_oper_one           (short l, char f, char p, char c, char a_test)
{
   char        rc          =    0;
   char        t           [LEN_SHORT] = "";
   DEBUG_YRPN_M  yLOG_snote   ("OPERATOR");
   if (c == 0)                                        return 0;
   if (l == 0 && strchr (s_one, c) == NULL)           return 0;
   if (l == 1) {
      if (strchr (s_two, c) == NULL)                  return 0;
      sprintf (t, "%s%c", myRPN.t_name, c);
      rc = yrpn_oper__by_name (l + 1, t);
      if (a_test == '-' && rc < 0)                    return 0;
   }
   if (l == 2) {
      if (strchr (s_thr, c) == NULL)                  return 0;
      sprintf (t, "%s%c", myRPN.t_name, c);
      rc = yrpn_oper__by_name (l + 1, t);
      if (a_test == '-' && rc < 0)                    return 0;
   }
   if (l >= 3)                                        return 0;
   return 1;
}



/*====================------------------------------------====================*/
/*===----                          token handling                      ----===*/
/*====================------------------------------------====================*/
static void        o___TOKENS__________________o (void) {;}

char
yrpn_oper__mathy_one    (short a_pos)
{
   DEBUG_YRPN     yLOG_note    ("must insert a multiplier");
   yrpn_stack_push_oper (YRPN_OPER, S_LEFT, 'g', "´", a_pos);
   myRPN.left_oper = S_OPER_CLEAR;
   myRPN.combined  = S_NO;
   return 0;
}

char
yrpn_oper_mathy         (short a_pos)
{
   char        rc          =    0;
   char        x_type      =  '-';
   char        x_name      [LEN_FULL]  = "";
   int         l           =    0;
   if (myRPN.math != 'y')  return 0;
   DEBUG_YRPN     yLOG_note    ("mathy multiplier check");
   rc = yrpn_output_peek (&x_type, NULL, x_name, NULL, NULL);
   DEBUG_YRPN     yLOG_char    ("x_type"    , x_type);
   DEBUG_YRPN     yLOG_info    ("x_name"    , x_name);
   l = strlen (x_name);
   DEBUG_YRPN     yLOG_value   ("l"         , l);
   if (strchr (YRPN_MATHERS, x_type) != NULL) {
      yrpn_oper__mathy_one (a_pos);
   } else if (x_type == YRPN_OPER && strchr ("ÆÇÈÉÊÎÍÌ", x_name [0]) != NULL) {
      yrpn_oper__mathy_one (a_pos);
   } else if (x_type == YRPN_GROUP && l == 1 && strchr (")¹", x_name [0]) != NULL) {
      yrpn_oper__mathy_one (a_pos);
   } else {
      DEBUG_YRPN     yLOG_note    ("already has an operator");
   }
   return 0;
}


int          /*--> check for operators -------------------[--------[--------]-*/
yrpn_oper__splat        (int  a_pos)
{  /*---(design notes)--------------------------------------------------------*/
   char  rc = 0;
   DEBUG_YRPN     yLOG_note    ("special pointer handling");
   DEBUG_YRPN     yLOG_char    ("line_type" , myRPN.line_type);
   rc = yRPN_stack_peek_OLD ();
   DEBUG_YRPN     yLOG_info    ("p_name"    , myRPN.p_name);
   DEBUG_YRPN     yLOG_char    ("p_type"    , myRPN.p_type);
   if (rc >= 0 && myRPN.p_type == YRPN_CAST   ) {
      DEBUG_YRPN     yLOG_note    ("working in * casting mode");
      strlcpy (myRPN.t_name, "(*)", LEN_LABEL);
      myRPN.t_type = YRPN_CAST   ;
      myRPN.t_prec = S_PREC_FUNC;
      myRPN.t_dir  = S_RIGHT;
      DEBUG_YRPN     yLOG_info    ("t_name"    , myRPN.t_name);
      DEBUG_YRPN     yLOG_char    ("t_type"    , myRPN.t_type);
      yrpn_stack_push   (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
      myRPN.left_oper  = S_OPER_CLEAR;
   } else if (rc >= 0 && (myRPN.line_type == S_LINE_DEF_FPTR || myRPN.line_type == S_LINE_DEF_FUN || myRPN.line_type == S_LINE_DEF_PRO)) {
      DEBUG_YRPN     yLOG_note    ("working in * type modifier mode");
      strlcpy (myRPN.t_name, "(*)", LEN_LABEL);
      myRPN.t_type = YRPN_PTYPE;
      DEBUG_YRPN     yLOG_info    ("t_name"    , myRPN.t_name);
      DEBUG_YRPN     yLOG_char    ("t_type"    , myRPN.t_type);
      yrpn_output_rpn   (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
      myRPN.left_oper  = S_OPER_CLEAR;
   } else if (myRPN.line_type == S_LINE_DEF_FPTR) {
      DEBUG_YRPN     yLOG_note    ("working in function pointer mode");
      strlcpy (myRPN.t_name, "(>", LEN_LABEL);
      myRPN.t_type = YRPN_FPTR   ;
      DEBUG_YRPN     yLOG_info    ("t_name"    , myRPN.t_name);
      DEBUG_YRPN     yLOG_char    ("t_type"    , myRPN.t_type);
      yrpn_output_rpn   (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
      myRPN.left_oper  = S_OPER_CLEAR;
   } else if ((myRPN.line_type == S_LINE_DEF || myRPN.line_type == S_LINE_DEF_VAR) && myRPN.line_sect != '=') {
      DEBUG_YRPN     yLOG_note    ("working in * type modifier mode");
      strlcpy (myRPN.t_name, "(*)", LEN_LABEL);
      myRPN.t_type = YRPN_TYPE   ;
      DEBUG_YRPN     yLOG_info    ("t_name"    , myRPN.t_name);
      DEBUG_YRPN     yLOG_char    ("t_type"    , myRPN.t_type);
      yrpn_output_rpn   (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
      myRPN.left_oper  = S_OPER_LEFT;
   } else {
      DEBUG_YRPN     yLOG_note    ("working in * dereference mode");
      DEBUG_YRPN     yLOG_info    ("t_name"    , myRPN.t_name);
      DEBUG_YRPN     yLOG_char    ("t_type"    , myRPN.t_type);
      yrpn_stack_push_oper  (myRPN.t_type, myRPN.t_dir, myRPN.t_prec, myRPN.t_name, a_pos);
      myRPN.left_oper  = S_OPER_LEFT;  /* an oper after an oper must be right-only */
   }
   return 0;
}

int          /*--> check for operators -------------------[--------[--------]-*/
yrpn_oper_any           (int  a_pos)
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
   yrpn_token_error  ();
   DEBUG_YRPN     yLOG_value   ("a_pos"     , a_pos);
   --rce;  if (a_pos <  0) {
      DEBUG_YRPN     yLOG_note    ("start can not be negative");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(accumulate characters)------------*/
   DEBUG_YRPN     yLOG_note    ("accumulate characters");
   myRPN.t_type   = YRPN_OPER   ;
   x_pos        = a_pos;  /* starting point */
   while (yrpn_token_accum (&x_pos) == 0);
   DEBUG_YRPN     yLOG_info    ("myRPN.t_name", myRPN.t_name);
   /*---(try to match normal operators)----*/
   x_found = i = yrpn_oper__by_name (-1, myRPN.t_name);
   /*---(handle misses)--------------------*/
   --rce;  if (x_found < 0) {
      yrpn_token_error  ();
      DEBUG_YRPN     yLOG_note    ("operator not found");
      DEBUG_YRPN     yLOG_exitr   (__FUNCTION__, rce);
      return rce;
   }
   /*---(save original)--------------------*/
   strlcpy (myRPN.t_name , s_opers [i].name, LEN_FULL);
   strlcpy (myRPN.t_token, s_opers [i].orig, LEN_FULL);
   /*---(constants)------------------------*/
   /*> if (strchr ("ÛÚ", myRPN.t_name) != NULL) {                                     <* 
    *> }                                                                              <*/
   /*---(check for unary)------------------*/
   if (myRPN.left_oper  == S_OPER_LEFT) {
      DEBUG_YRPN     yLOG_note    ("check for left operators in right only mode");
      if      (strcmp (myRPN.t_name, "+" ) == 0)  { strcpy (myRPN.t_name, "+:"); myRPN.t_len  = 2; }
      else if (strcmp (myRPN.t_name, "-" ) == 0)  { strcpy (myRPN.t_name, "-:"); myRPN.t_len  = 2; }
      else if (strcmp (myRPN.t_name, "*" ) == 0)  { strcpy (myRPN.t_name, "á" ); myRPN.t_len  = 2; }
      else if (strcmp (myRPN.t_name, "&" ) == 0)  { strcpy (myRPN.t_name, "&:"); myRPN.t_len  = 2; }
   } else {
      DEBUG_YRPN     yLOG_note    ("check for right operators in left only mode");
      if      (strcmp (myRPN.t_name, "++") == 0)  { strcpy (myRPN.t_name, ":+"); myRPN.t_len  = 2; }
      else if (strcmp (myRPN.t_name, "--") == 0)  { strcpy (myRPN.t_name, ":-"); myRPN.t_len  = 2; }
   }
   /*---(handle it)------------------------*/
   yrpn_oper_prec ();
   yrpn_output_infix (myRPN.t_type, myRPN.t_prec, myRPN.t_name, myRPN.t_token, a_pos);
   if        (myRPN.pproc == S_PPROC_YES) {
      DEBUG_YRPN     yLOG_note    ("pre-processor directive");
      yrpn_output_rpn   (myRPN.t_type, myRPN.t_prec, myRPN.t_name, a_pos);
      myRPN.left_oper  = S_OPER_LEFT;  /* an oper after an oper must be right-only */
   } else if (strcmp (myRPN.t_name, "á") == 0) {
      DEBUG_YRPN     yLOG_note    ("working with a pointer");
      yrpn_oper__splat  (a_pos);
   } else {
      DEBUG_YRPN     yLOG_note    ("working with normal operators");
      yrpn_stack_push_oper  (myRPN.t_type, myRPN.t_dir, myRPN.t_prec, myRPN.t_name, a_pos);
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

char       /* ---- : identify the symbol precedence --------------------------*/
yrpn_oper_prec    (void)
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

char       /* ---- : identify the symbol arity -------------------------------*/
yRPN_arity         (char *a_name, char *r_dir)
{
   int       i         = 0;
   i = yrpn_oper__by_name (-1, a_name);
   if (i < 0)  return -1;
   if (r_dir != NULL)  *r_dir = s_opers [i].dir;
   return s_opers [i].arity;
}

char
yrpn_oper_combining     (void)
{
   DEBUG_YRPN     yLOG_char    ("combined"  , myRPN.combined);
   return 1;
   if (myRPN.combined == S_YES)  return 1;
   return 0;
}


