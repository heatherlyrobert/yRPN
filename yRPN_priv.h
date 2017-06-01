/*===[[ HEADER GUARD ]]=======================================================*/
#ifndef zRPN_PRIVATE  
#define zRPN_PRIVATE         loaded

#include    <stdio.h>
#include    <string.h>
#include    <stdlib.h>
#include    <ctype.h>
#include    <yLOG.h>         /* CUSTOM : heatherly program logging            */


/*===[[ VERSION ]]========================================*/
/* rapidly evolving version number to aid with visual change confirmation     */
#define   zRPN_VER_NUM       "0.7d"
#define   zRPN_VER_TXT       "added spreadsheet string logical comparison operators"


#define   zRPN_MAX_LEN       2000

extern char     *v_alphanum;
extern char     *v_alpha;
extern char     *v_lower;
extern char     *v_caps;
extern char     *v_number;
extern char     *v_paren;
extern char     *v_operator;
extern char     *v_address;

extern char      zRPN_olddebug;

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
#define   DEBUG_TOPS       if (zRPN_debug.tops == 'y')
#define   DEBUG_OPER       if (zRPN_debug.oper == 'y')

#define PRIV      static


typedef   struct cRPN  tRPN;
struct  cRPN {
   /*---(infix format)-----+-----------+-*/
   char        source      [zRPN_MAX_LEN];  /* source infix string (saved)    */
   int         nsource;                     /* source infix length (saved)    */
   /*---(working areas)----+-----------+-*/
   char        working     [zRPN_MAX_LEN];  /* copy of source for parsing     */
   int         nworking;                    /* position in working string     */
   /*---(working areas)----+-----------+-*/
   char        type;
   char        prec;
   char        dir;
   char        arity;
   char        token       [zRPN_MAX_LEN];
   int         ntoken;
   /*---(stack)------------+-----------+-*/
   char        stack       [zRPN_MAX_LEN][zRPN_MAX_LEN];
   int         nstack;
   int         depth;  
   char        t_type;
   char        t_prec;
   char        t_token     [zRPN_MAX_LEN];
   char        cdepth;
   char        mdepth;
   char        lops;
   /*---(postfix format)---+-----------+-*/
   char        output      [zRPN_MAX_LEN];
   char        detail      [zRPN_MAX_LEN];
   char        normal      [zRPN_MAX_LEN];
   char        tokens      [zRPN_MAX_LEN];
   char        micro       [zRPN_MAX_LEN];
   int         noutput;
   int         nnormal;
   int         count;
   /*---(other)------------+-----------+-*/
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
#define   MODE_GYGES    if (zRPN_lang == 's')
#define   MODE_HEPH     if (zRPN_lang == 'c')


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
yRPN__symbols      (int   a_pos);

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
yRPN__ckeywords    (int  a_pos);

int        /* ---- : save off type declarations ------------------------------*/
yRPN__types        (int  a_pos);

char       /* ---- : set spreadsheet human readable --------------------------*/
yRPN__shuman       (int *a_ntoken);


#endif
/*===[[ END ]]================================================================*/
