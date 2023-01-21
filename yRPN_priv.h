/*===[[ HEADER GUARD ]]=======================================================*/
#ifndef zRPN_PRIVATE  
#define zRPN_PRIVATE         loaded



/*===[[ HEADER ]]=============================================================*

 *   focus         : (PS) programming support
 *   niche         : (rp) reverse-polish notation conversion
 *   purpose       : simple, clean infix to reverse polish notation conversion
 *
 *   base_system   : gnu/linux   (powerful, ubiquitous, technical, and hackable)
 *   lang_name     : ansi-c      (wicked, limitless, universal, and everlasting)
 *   dependencies  : ((none))
 *   size goal     : tiny (approximately 500 slocL)
 *
 *   author        : the_heatherlys
 *   created       : 2011-08
 *   priorities    : direct, simple, brief, vigorous, and lucid (h.w. fowler)
 *   end goal      : loosely coupled, strict interface, maintainable, portable
 * 
 */

/*===[[ BEG_HEADER ]]=========================================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-12345678901-12345678901-*/
/*===[[ BEG_HEADER ]]=========================================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-12345678901-12345678901-*/


/*===[[ ONE_LINERS ]]=========================================================*/

#define     P_BASENAME  "yRPN"

#define     P_FOCUS     "PS (programming support)"
#define     P_NICHE     "rp (reverse-polish notation)"
#define     P_SUBJECT   "machine efficient formulas"
#define     P_PURPOSE   "simple, clean infix to reverse polish notation conversion"

#define     P_NAMESAKE  "terpsikhore-seirenes (delight in dance)
#define     P_HERITAGE  "terpsikhore is the greek muse of music, choral song, and dance"
#define     P_IMAGERY   "beaufiful young woman sitting and playing a lyre"
#define     P_REASON    "choral song is about coordination as is rpn"

#define     P_ONELINE   P_NAMESAKE " " P_SUBJECT

#define     P_HOMEDIR   "/home/system/yRPN.postfix_notation_conversion"
#define     P_FULLPATH  "/usr/local/lib64/libyRPN.so"
#define     P_SUFFIX    "n/a"
#define     P_CONTENT   "n/a"

#define     P_SYSTEM    "gnu/linux   (powerful, ubiquitous, technical, and hackable)"
#define     P_LANGUAGE  "ansi-c      (wicked, limitless, universal, and everlasting)"
#define     P_CODESIZE  "large       (appoximately 10,000 slocl)"
#define     P_DEPENDS   "none"

#define     P_AUTHOR    "heatherlyrobert"
#define     P_CREATED   "2011-08"

#define     P_VERMAJOR  "1.--, production use, working out issues"
#define     P_VERMINOR  "1.1-, first full production version"
#define     P_VERNUM    "1.1l"
#define     P_VERTXT    "all unit tests updated and passed"

/*===[[ END_HEADER ]]=========================================================*/




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


extern char s_divider [5];
extern char s_divtech [5];

extern char     *v_alphanum;
extern char     *v_alpha;
extern char     *v_float;
extern char     *v_sequence;
extern char     *v_enders;
extern char     *v_preproc;

extern char      zRPN_olddebug;

/*---(debugging)----------------------*/
#define      S_DEBUG_NO         'n'
#define      S_DEBUG_YES        'y'
/*---(token types)--------------------*/
#define      S_TTYPE_NONE       '-'
/*---(grouping)--------*/
#define      S_TTYPE_GROUP      '('
/*---(lower)-----------*/
#define      S_TTYPE_KEYW       'k'
#define      S_TTYPE_TYPE       't'
#define      S_TTYPE_PTYPE      'p'
#define      S_TTYPE_ERROR      'e'
#define      S_TTYPE_CONST      'n'
#define      S_TTYPE_OPER       'o'
#define      S_TTYPE_FUNC       'f'
#define      S_TTYPE_ADDR       '@'
#define      S_TTYPE_VARS       'v'
#define      S_TTYPE_LOCAL      'l'
#define      S_TTYPE_MEMB       'm'
#define      S_TTYPE_CAST       'c'
#define      S_TTYPE_FPTR       '*'
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
/*---(line types)---------------------*/
#define      S_LINE_NORMAL      's'
#define      S_LINE_PREPROC     '#'
#define      S_LINE_EXTERN      'e'
#define      S_LINE_DEF         'D'
#define      S_LINE_DEF_VAR     'v'
#define      S_LINE_DEF_PRO     'p'
#define      S_LINE_DEF_FUN     'f'
#define      S_LINE_DEF_FPTR    '*'
/*---(line complete)------------------*/
#define      S_LINE_OPEN        '-'
#define      S_LINE_DONE        'y'
/*---(characters)---------------------*/
#define      S_CHAR_WORM        '@'
#define      S_CHAR_GREED       '$'
#define      S_CHAR_ZERO        '0'

#define      S_YES              'y'
#define      S_NO               '-'


extern char      g_type_counts    [LEN_LABEL];

/*> typedef  struct cRPN_DEBUG   tRPN_DEBUG;                                           <* 
 *> struct cRPN_DEBUG {                                                                <* 
 *>    char        tops;                   /+ t) broad structure and context      +/   <* 
 *>    char        stack;                  /+ stack                               +/   <* 
 *>    char        cell;                   /+ cell/address                        +/   <* 
 *>    char        oper;                   /+ operators                           +/   <* 
 *>    char        keys;                   /+ keywords                            +/   <* 
 *> };                                                                                 <* 
 *> tRPN_DEBUG  zRPN_debug;                                                            <*/

#define   zRPN_DEBUG       if ('n' == 'y')

#define PRIV      static


typedef   struct cRPN  tRPN;
struct  cRPN {
   /*---(infix format)-------------------*/
   char        source      [LEN_RECD];  /* source infix string (const)    */
   int         l_source;                    /* length of source infix string  */
   /*---(working areas)------------------*/
   char        working     [LEN_RECD];  /* copy of source for parsing     */
   int         l_working;                   /* position in working string     */
   int         pos;
   /*---(overall working)----------------*/
   char        line_type;                   /* source line type               */
   char        line_sect;                   /* source section pre or post =   */
   char        line_done;                   /* source line complete           */
   char        paren_lvl;                   /* how deep in parenthesis (any)  */
   /*---(token working)------------------*/
   char        t_token     [LEN_FULL];   /* current token (full)           */
   char        t_type;                      /* current token type             */
   char        t_name      [LEN_FULL];   /* current token name             */
   int         t_len;                       /* current token length           */
   char        t_prec;                      /* current token precidence       */
   char        t_dir;                       /* current token dir of eval      */
   char        t_arity;                     /* current token unary, binary,.. */
   char        t_post;                      /* current token postfix-unary    */
   char        t_comb;                      /* current token combining        */
   char        left_oper;                   /* if oper next, must be left type*/
   char        combined;                    /* once hit a combining operator  */
   char        pproc;                       /* pre-processor modes            */
   /*---(stack)--------------------------*/
   char        p_name      [LEN_FULL];   /* peek token from stack          */
   char        p_type;                      /* peek token from stack          */
   char        p_prec;                      /* peek token from stack          */
   /*---(shuntd)-------------------------*/
   char        l_name      [LEN_FULL];   /* last token touched             */
   char        l_type;                      /* last token touched             */
   char        l_prec;                      /* last token touched             */
   /*---(shuntd)-------------------------*/
   char        s_name      [LEN_FULL];   /* last token shuntd              */
   char        s_type;                      /* last token shuntd              */
   char        s_prec;                      /* last token shuntd              */
   /*---(infix output)-------------------*/
   char        parsed      [LEN_RECD];
   char        tokens      [LEN_RECD];
   char        normal      [LEN_RECD];
   int         l_normal;
   int         n_tokens; 
   /*---(postfix output)-----------------*/
   char        shuntd      [LEN_RECD];
   char        detail      [LEN_RECD];
   int         l_shuntd;
   int         n_shuntd;
   /*---(pretty output)------------------*/
   char        pretty      [LEN_RECD];
   /*---(MAYBE GONE)---------------------*/
   char        about       [500];
};
extern  tRPN      myRPN;


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
#define   MODE_GYGES    if (zRPN_lang == YRPN_GYGES)
#define   MODE_C        if (zRPN_lang == YRPN_CBANG)


extern  char      zRPN_divider [5];

char         /*--> convert spreadsheet infix to rpn ------[ ------ [ ------ ]-*/
yRPN__driver       (char *a_src, char a_type, char **a_rpn, int *a_nrpn, int a_max);

char       /* ---- : identify the symbol precedence --------------------------*/
yRPN__prec   (void);

int        /* ---- : prepare variables for postfix conversion ----------------*/
yRPN__load         (char *a_source);

char         /*--> convert complier infix to rpn ---------[--------[--------]-*/
yRPN__convert      (char *a_source);

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
yRPN__sequence     (int  a_pos);

int          /*--> check for bad addresses ---------------[--------[--------]-*/
yRPN__badaddr      (int  a_pos);

int        /* ---- : save off cell addresses ---------------------------------*/
yRPN__addresses    (int  a_pos, short a_ctab);

/*> int        /+ ---- : save off constants --------------------------------------+/   <* 
 *> yRPN__constants    (int  a_pos);                                                   <*/

int        /* ---- : save off keywords ---------------------------------------*/
yRPN__keywords     (int  a_pos);

int        /* ---- : save off type declarations ------------------------------*/
yRPN__types        (int  a_pos);

int          /*--> check for statement enders ------------[--------[--------]-*/
yRPN__enders         (int  a_pos);

char       /* ---- : convert normal infix notation to postfix/rpn ------------*/
yRPN__output_done    (void);



char*      /* ---- : provide gray-box information to unit testing ------------*/
yRPN_accessor      (char*, int);

char       /*----: set up programgents/debugging -----------------------------*/
yRPN__unit_quiet    (void);

char       /*----: set up programgents/debugging -----------------------------*/
yRPN__unit_loud     (void);

char       /*----: set up programgents/debugging -----------------------------*/
yRPN__unit_end      (void);


extern char unit_answer [ LEN_RECD ];






/*===[[ yRPN_stack.c ]]========================================*/
/*345678901-12345678901234567890->-----------------------------*/
/*---(program)-----------------*/
char        yRPN_stack_init      (void);
/*---(stack on)----------------*/
char        yRPN_stack_push      (int a_pos);
char        yRPN_stack_update    (void);
/*---(stack off)---------------*/
char        yRPN_stack_peek      (void);
char        yRPN_stack_pops      (void);
char        yRPN_stack_toss      (void);
/*---(specialty)---------------*/
char        yRPN_stack_oper      (int a_pos);
char        yRPN_stack_paren     (int a_pos);
char        yRPN_stack_comma     (int a_pos);
/*---(output)------------------*/
char        yRPN_stack_shuntd    (void);
char        yRPN_stack_normal    (int a_pos);
char        yRPN_stack_tokens     (void);
/*---(unittest)----------------*/
char*       yRPN_stack_unit      (char *a_question, int a_item);
/*---(done)--------------------*/


/*===[[ yRPN_cell.c ]]=========================================*/
/*345678901-12345678901234567890->-----------------------------*/
/*---(parsing)-----------------*/
char        yRPN_cell_init       (char *a_label, short *a_pos, short *a_tab, short *a_col, short *a_row, char *a_abs, char *a_max);
char        yRPN_cell_tab        (char *a_label, short *a_pos, short *a_tab, char *a_abs, char  a_max, short a_ctab);
char        yRPN_cell_col        (char *a_label, short *a_pos, short *a_col, char *a_abs, char  a_max);
char        yRPN_cell_row        (char *a_label, short *a_pos, short *a_row, char *a_abs, char  a_max);
char        yRPN_cell_pretty     (short a_tab, short a_col, short a_row, char a_abs, char *a_pretty);
char        yRPN_cell            (char *a_label, short a_ctab);
char        yRPN__cells          (char *a_label, int *a_tab, int *a_col, int *a_row, char *a_sign);


char        yRPN_space           (char  *a_token, char *a_pre, char *a_suf, char *a_new);
char        yrpn_ready              (void);

char        yrpn_addr__init         (void);
char*       yrpn_addr__unit      (char *a_question, int a_item);

char        yrpn_addr_insider_fake  (int b, int x, int y, int z);

char        yrpn__token_unit_prep   (char a_type, char *a_working, int *a_pos, int a_start);
char        yrpn_token_numtype      (int a_pos);
char        yRPN__token_paren       (int a_pos);
char        yrpn_token_add          (int *a_pos);

#endif
/*===[[ END ]]================================================================*/
