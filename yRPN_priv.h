/*===[[ HEADER GUARD ]]=======================================================*/
#ifndef zRPN_PRIVATE  
#define zRPN_PRIVATE         loaded



/*===[[ VERSION ]]========================================*/
/* rapidly evolving version number to aid with visual change confirmation     */
#define   zRPN_VER_NUM       "0.7h"
#define   zRPN_VER_TXT       "added all output and stack review to type and keyword units"



/*===[[ HEADERS ]]========================================*/
/*---(ansi-c standard)-------------------*/
#include    <stdio.h>        /* CLIBC   standard input/output                 */
#include    <stdlib.h>       /* CLIBC   standard general purpose              */
#include    <string.h>       /* CLIBC   standard string handling              */
#include    <ctype.h>

/*---(heatherly made)--------------------*/
#include    <yURG.h>         /* CUSTOM  heatherly urgent processing           */
#include    <yLOG.h>         /* CUSTOM  heatherly program logging             */
#include    <ySTR.h>         /* CUSTOM  heatherly string handling             */
#include    <yVAR.h>         /* CUSTOM  heatherly variable testing            */





#define   zRPN_MAX_LEN       2000
#define   S_MAX_STACK         100

extern char     *v_alphanum;
extern char     *v_alpha;
extern char     *v_lower;
extern char     *v_upnum;
extern char     *v_number;
extern char     *v_paren;
extern char     *v_operator;
extern char     *v_address;

extern char      zRPN_olddebug;

/*---(debugging)----------------------*/
#define      S_DEBUG_NO         'n'
#define      S_DEBUG_YES        'y'
/*---(lengths)------------------------*/
#define      S_LEN_OUTPUT     2000
#define      S_LEN_TOKEN       200
#define      S_LEN_DESC        100
#define      S_LEN_LABEL        20
/*---(token types)--------------------*/
/*---(grouping)--------*/
#define      S_TTYPE_GROUP      '('
/*---(lower)-----------*/
#define      S_TTYPE_KEYW       'k'
#define      S_TTYPE_TYPE       't'
#define      S_TTYPE_ERROR      'e'
#define      S_TTYPE_CONST      'n'
#define      S_TTYPE_OPER       'o'
#define      S_TTYPE_FUNC       'f'
#define      S_TTYPE_ADDR       '@'
#define      S_TTYPE_VARS       'v'
/*---(upper)-----------*/
#define      S_TTYPE_CHAR       'C'
#define      S_TTYPE_STR        'S'
#define      S_TTYPE_HEX        'X'
#define      S_TTYPE_BIN        'B'
#define      S_TTYPE_BINHEX     "BX"
#define      S_TTYPE_OCT        'O'
#define      S_TTYPE_INT        'I'
#define      S_TTYPE_FLOAT      'F'
/*---(precidence)---------------------*/
#define      S_PREC_NONE        '-'
#define      S_PREC_FAIL        '?'
#define      S_PREC_FUNC        'a'
/*---(language support)---------------*/
#define      S_LANG_C           'c'
#define      S_LANG_GYGES       's'
/*---(evalulation direction)----------*/
#define      S_LEFT             'l'
#define      S_RIGHT            'r'
/*---(operator type)------------------*/
#define      S_RIGHT_ONLY       'y'
#define      S_LEFT_ONLY        '-'
/*---(left_oper options)--------------*/
#define      S_OPER_LEFT        'y'
#define      S_OPER_CLEAR       '-'
/*---(preprocessor)-------------------*/
#define      S_PPROC_NO         '-'
#define      S_PPROC_YES        'y'
#define      S_PPROC_INCL       'i'
#define      S_PPROC_OTHER      'o'



typedef  struct cRPN_DEBUG   tRPN_DEBUG;
struct cRPN_DEBUG {
   char        tops;                   /* t) broad structure and context      */
   char        stack;                  /* stack                               */
   char        cell;                   /* cell/address                        */
   char        oper;                   /* operators                           */
   char        keys;                   /* keywords                            */
};
tRPN_DEBUG  zRPN_debug;

#define   zRPN_DEBUG       if ('n' == 'y')
#define   DEBUG_OPER       if (zRPN_debug.oper == 'y')

#define PRIV      static


typedef   struct cRPN  tRPN;
struct  cRPN {
   /*---(infix format)-------------------*/
   char        source      [S_LEN_OUTPUT];  /* source infix string (const)    */
   int         l_source;                    /* length of source infix string  */
   /*---(working areas)------------------*/
   char        working     [S_LEN_OUTPUT];  /* copy of source for parsing     */
   int         l_working;                   /* position in working string     */
   /*---(working areas)------------------*/
   char        t_token     [S_LEN_TOKEN];   /* current token (full)           */
   char        t_type;                      /* current token type             */
   char        t_name      [S_LEN_TOKEN];   /* current token name             */
   int         t_len;                       /* current token length           */
   char        t_prec;                      /* current token precidence       */
   char        t_dir;                       /* current token dir of eval      */
   char        t_arity;                     /* current token unary, binary,.. */
   char        left_oper;                   /* if oper next, must be left type*/
   /*---(stack)--------------------------*/
   char        stack       [S_MAX_STACK][S_LEN_TOKEN];
   int         n_stack;
   char        p_type;
   char        p_prec;
   char        pproc;                       /* pre-processor modes           */
   /*---(infix output)-------------------*/
   char        tokens      [S_LEN_OUTPUT];
   char        normal      [S_LEN_OUTPUT];
   int         n_tokens; 
   /*---(postfix output)-----------------*/
   char        shuntd      [S_LEN_OUTPUT];
   char        detail      [S_LEN_OUTPUT];
   int         l_shuntd;
   int         l_normal;
   int         n_shuntd;
   /*---(MAYBE GONE)---------------------*/
   int         depth;  
   char        about       [500];
};
extern  tRPN      rpn;

#define   zRPN_END       '~'


#define   zRPN_ERR_INPUT_NOT_AVAIL        -100
/*---(literals)------------------*/
#define   zRPN_ERR_NO_STARTING_QUOTE      -101
#define   zRPN_ERR_NO_ENDING_QUOTE        -102
#define   zRPN_ERR_LITERAL_TOO_SHORT      -103

#define   zRPN_ERR_NOT_START_ALPHA        -104
#define   zRPN_ERR_NOT_START_NUMBER       -105
#define   zRPN_ERR_NOT_START_OPERATOR     -106
#define   zRPN_ERR_ZERO_LEN_TOKEN         -107
#define   zRPN_ERR_OPERATOR_TOO_LONG      -108
#define   zRPN_ERR_NOT_GROUPING           -109

#define   zRPN_ERR_EMPTY_STACK            -110
#define   zRPN_ERR_UNBALANCED_PARENS      -111

#define   zRPN_ERR_NOT_SPREADSHEET        -112
#define   zRPN_ERR_NOT_ADDRESS_CHAR       -113
#define   zRPN_ERR_BAD_ADDRESS            -114

#define   zRPN_ERR_MULTIPLE_DECIMALS      -115
#define   zRPN_ERR_UNKNOWN_CONSTANT       -116
#define   zRPN_ERR_UNKNOWN_TYPE           -117
#define   zRPN_ERR_DECIMALS_NOT_ALLOWED   -118


extern  char      zRPN_lang;
#define   MODE_GYGES    if (zRPN_lang == S_LANG_GYGES)
#define   MODE_C        if (zRPN_lang == S_LANG_C    )


extern  char      zRPN_divider [5];


char*      /* ---- : provide gray-box information to unit testing ------------*/
yRPN_accessor      (char*, int);

char       /* ---- : identify the symbol precedence --------------------------*/
yRPN__precedence   (void);

int        /* ---- : prepare variables for postfix conversion ----------------*/
yRPN__load         (char *a_source);


int        /* ---- : save off character literals -----------------------------*/
yRPN__chars        (int   a_pos);

int        /* ---- : save off string literals --------------------------------*/
yRPN__strings      (int   a_pos);

int        /* ---- : save off symbol names -----------------------------------*/
yRPN__funcvar      (int   a_pos);

int        /* ---- : save off numbers ----------------------------------------*/
yRPN__numbers      (int   a_pos);

int        /* ---- : process operators ---------------------------------------*/
yRPN__operators    (int  a_pos);

int        /* ---- : process grouping ----------------------------------------*/
yRPN__grouping     (int  a_pos);

char       /* ---- : interpret cell address ----------------------------------*/
yRPN__cells       (char *a_label, int *a_tab, int *a_col, int *a_row, char *a_sign);

int        /* ---- : save off cell addresses ---------------------------------*/
yRPN__addresses    (int  a_pos);

int        /* ---- : save off constants --------------------------------------*/
yRPN__constants    (int  a_pos);

int        /* ---- : save off keywords ---------------------------------------*/
yRPN__keywords     (int  a_pos);

int        /* ---- : save off type declarations ------------------------------*/
yRPN__types        (int  a_pos);

char       /* ---- : set spreadsheet human readable --------------------------*/
yRPN__shuman       (int *a_ntoken);

char       /* ---- : convert normal infix notation to postfix/rpn ------------*/
yRPN__output_done    (void);

#endif
/*===[[ END ]]================================================================*/
