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

/*
 *  TODO/WANT (exact output which adds parens for max predantic-ness)
 *      input   : a + b * c - d / e * f
 *      exact   : (a+(b¥c))-((d/e)¥f)
 *      > use mathy spacing and pretty symbols
 *      > likely recursion from shunted based on arity and direction
 *      > leave off furthest outside parens (they are obvious)
 *
 *      a) start at beg each time
 *      b) find next operator (skipping
 *      c) get arity, and direction for arity=1
 *
 *      11   a b c * + d e / f * -
 *                 ‘ 2 ∑ (b¥c)
 *
 *       9   a (b¥c) + d e / f * -
 *                   ‘ 2 ∑ (a+(b¥c))
 *
 *       7   (a+(b¥c)) d e / f * -
 *                         ‘ 2 ∑ (d/e)
 *
 *       5   (a+(b¥c)) (d/e) f * -
 *                             ‘ 2 ∑ ((d/e)*f)
 *
 *       3   (a+(b¥c)) ((d/e)¥f) -
 *                               ‘ 2 ∑ (a+(b¥c))-((d/e)¥f)
 *
 *       1   ((a+(b¥c))-((d/e)¥f))
 *                              
 *       finally, peel off outermost parens (just too much)
 *       0   (a+(b¥c))-((d/e)¥f)
 *                              
 *
 *
 */





/*===[[ BEG_HEADER ]]=========================================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-12345678901-12345678901-*/
/*===[[ BEG_HEADER ]]=========================================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-12345678901-12345678901-*/

/*
 *
 * example    = ∏1-tπ«x¿ + 3t∏1-tπ∆x¡ + 3t∆∏1-tπx¬ + t«x√
 * 
 *     3t     ÷  3¥t
 *      t∏     ÷  t¥(
 *       3t∏    ÷  3¥t¥(
 *
 *    πx      ÷ )¥x
 *     π∆x     ÷ )∆¥x
 *
 *    x∆3y    ÷ x∆¥3¥y
 *
 */

/*===[[ ONE_LINERS ]]=========================================================*/

#define     P_BASENAME  "yRPN"

#define     P_FOCUS     "PS (programming support)"
#define     P_NICHE     "rp (reverse-polish notation)"
#define     P_SUBJECT   "machine efficient formulas"
#define     P_PURPOSE   "simple, clean infix to reverse polish notation conversion"

#define     P_NAMESAKE  "terpsikhore-seirenes (delight in dance)"
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
#define     P_VERMINOR  "1.2-, first full production version"
#define     P_VERNUM    "1.2f"
#define     P_VERTXT    "minor updates to support library changes"

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

extern char     *v_enders;
extern char     *v_preproc;


/*---(debugging)----------------------*/
#define      S_DEBUG_NO         'n'
#define      S_DEBUG_YES        'y'
/*---(token types)--------------------*/
#define      S_TTYPE_NONE       '-'
/*---(grouping)--------*/
#define      S_TTYPE_GROUP      '('
/*---(lower)-----------*/
/*> #define      S_TTYPE_KEYW       'k'                                               <*/
/*> #define      S_TTYPE_TYPE       't'                                               <*/
/*> #define      S_TTYPE_PTYPE      'p'                                               <*/
/*> #define      S_TTYPE_ERROR      'e'                                               <*/
/*> #define      YRPN_CONST         'n'                                               <*/
/*> #define      S_TTYPE_OPER       'o'                                               <*/
/*> #define      S_TTYPE_FUNC       'f'                                               <*/
/*> #define      S_TTYPE_ADDR       '@'                                               <*/
/*> #define      S_TTYPE_VARS       'v'                                               <*/
/*> #define      S_TTYPE_LOCAL      'l'                                               <*/
/*> #define      S_TTYPE_MEMB       'm'                                               <*/
/*> #define      S_TTYPE_CAST       'c'                                               <*/
/*> #define      S_TTYPE_FPTR       '*'                                               <*/
/*---(upper)-----------*/
/*> #define      S_TTYPE_CHAR       'C'                                               <*/
/*> #define      S_TTYPE_STR        'S'                                               <*/
/*> #define      S_TTYPE_HEX        'X'                                               <*/
/*> #define      S_TTYPE_BIN        'B'                                               <*/
/*> #define      S_TTYPE_OCT        'O'                                               <*/
/*> #define      S_TTYPE_INT        'I'                                               <*/
/*> #define      S_TTYPE_FLOAT      'F'                                               <*/
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
   /*---(configuration)------------------*/
   char        lang;
   char        math;
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
   char        level;                       /* how deep in parenthesis (any)  */
   char        narg        [LEN_LABEL];     /* how many args by level         */
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
   /*---(shunted)------------------------*/
   char        l_type;                      /* last token type                */
   char        l_prec;                      /* last token precedence          */
   char        l_name      [LEN_FULL];      /* last token name                */
   char        l_token     [LEN_FULL];      /* last token token               */
   short       l_pos;                       /* last token pos         */
   /*---(shunted)------------------------*/
   char        s_name      [LEN_FULL];   /* last token shunted             */
   char        s_type;                      /* last token shunted             */
   char        s_prec;                      /* last token shunted             */
   /*---(infix output)-------------------*/
   char        parsed      [LEN_RECD];
   char        tokens      [LEN_RECD];
   int         l_tokens;
   int         n_tokens; 
   /*---(postfix output)-----------------*/
   char        shunted     [LEN_RECD];
   char        detail      [LEN_RECD];
   char        debug       [LEN_RECD];
   int         l_shunted;
   int         n_shunted;
   /*---(pretty output)------------------*/
   char        pretty      [LEN_RECD];
   char        mathy       [LEN_RECD];
   char        exact       [LEN_RECD];
   /*---(MAYBE GONE)---------------------*/
   char        about       [500];
};
extern  tRPN      myRPN;


extern char  s_one  [LEN_HUND];
extern char  s_two  [LEN_HUND];
extern char  s_thr  [LEN_HUND];


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


#define   MODE_GYGES    if (myRPN.lang == YRPN_GYGES)
#define   MODE_C        if (myRPN.lang == YRPN_CBANG)


extern  char      zRPN_divider [5];



/*===[[ STACK ]]==============================================================*/
#define   S_MAX_STACK         100
typedef     struct cSTACK    tSTACK;
struct cSTACK {
   uchar       type;
   uchar       prec;
   uchar       name        [LEN_FULL];
   short       pos;
};
extern tSTACK  g_stack     [S_MAX_STACK];
extern int     g_nstack;















/*===[[ yRPN_group.c ]]========================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char        yrpn_group_init         (void);
char        yrpn_group_open         (uchar a_type, uchar a_prec, uchar a_name [LEN_FULL], uchar a_token [LEN_FULL], short a_pos);
char        yrpn_group_close        (uchar a_type, uchar a_prec, uchar a_name [LEN_FULL], uchar a_token [LEN_FULL], short a_pos);
char        yrpn_group_open_mathy   (uchar a_type, uchar a_prec, uchar a_name [LEN_FULL], uchar a_token [LEN_FULL], short a_pos);
char        yrpn_group_close_mathy  (uchar a_type, uchar a_prec, uchar a_name [LEN_FULL], uchar a_token [LEN_FULL], short a_pos);





/*===[[ yRPN_stack.c ]]========================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
/*---(program)-----------------*/
char        yrpn_stack_init         (void);
char        yrpn_stack_show         (void);
/*---(stack on)----------------*/
char        yrpn_stack_push         (uchar a_type, uchar a_prec, uchar a_name [LEN_FULL], short a_pos);
char        yrpn_stack_update       (uchar a_type, uchar a_prec, uchar a_name [LEN_FULL]);
/*---(stack off)---------------*/
char        yrpn_stack_peek         (uchar *a_type, uchar *a_prec, uchar a_name [LEN_FULL], short *a_pos);
char        yRPN_stack_peek_OLD     (void);
char        yrpn_stack_pop          (void);
char        yrpn_stack__toss        (void);
/*---(specialty)---------------*/
char        yrpn_stack_push_oper    (uchar a_type, uchar a_dir, uchar a_prec, uchar a_name [LEN_FULL], short a_pos);
char        yrpn_stack_paren        (uchar a_type, uchar a_prec, uchar a_name [LEN_FULL], short a_pos);
char        yrpn_stack_comma        (uchar a_type, uchar a_prec, uchar a_name [LEN_FULL], short a_pos);
/*---(unittest)----------------*/
char*       yRPN_stack_unit      (char *a_question, int a_item);
/*---(done)--------------------*/


/*===[[ yRPN_out.c ]]==========================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
/*---(program)-----------------*/
char        yrpn_output_init        (void);
/*---(rpn)---------------------*/
char        yrpn_output__shunted    (uchar a_type, uchar a_name [LEN_FULL], short a_pos, uchar a_div [LEN_SHORT]);
char        yrpn_output__detail     (uchar a_type, uchar a_name [LEN_FULL], short a_pos, uchar a_div [LEN_SHORT]);
char        yrpn_output__debug      (uchar a_type, uchar a_name [LEN_FULL], short a_pos, uchar a_div [LEN_SHORT]);
char        yrpn_output_rpn         (uchar a_type, uchar a_prec, uchar a_name [LEN_FULL], short a_pos);
/*---(token)-------------------*/
char        yrpn_output__parsed     (uchar a_type, uchar a_name [LEN_FULL], uchar a_token [LEN_FULL], short a_pos, uchar a_div [LEN_SHORT]);
char        yrpn_output__tokens     (uchar a_type, uchar a_name [LEN_FULL], uchar a_token [LEN_FULL], short a_pos, uchar a_div [LEN_SHORT]);
char        yrpn_output__pretty     (uchar a_type, uchar a_name [LEN_FULL], uchar a_token [LEN_FULL], short a_pos, uchar a_div [LEN_SHORT]);
char        yrpn_output__linetype   (uchar a_type, uchar a_name [LEN_FULL], uchar a_token [LEN_FULL], short a_pos, uchar a_div [LEN_SHORT]);
char        yrpn_output_infix       (uchar a_type, uchar a_prec, uchar a_name [LEN_FULL], uchar a_token [LEN_FULL], short a_pos);
char        yrpn_output_peek        (uchar *r_type, uchar *r_prec, uchar r_name [LEN_FULL], uchar r_token [LEN_FULL], short *r_pos);
/*---(done)--------------------*/












/*===[[ yRPN_addr.c ]]=========================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
/*---(program)--------------*/
char        yrpn_addr_init          (void);
char        yRPN_addr_config        (void *a_breaker, void *a_maker, void *a_prettier, void *a_adjuster, void *a_insider);
char        yrpn_addr_one           (short l, char f, char p, char c);
/*---(modify)---------------*/
char        yrpn_addr__adj_check    (char *a_src, char a_scope, char *a_target);
char        yrpn_addr__adj_one      (char *a_old, char a_scope, char *a_new);
char        yrpn_addr__adj_main     (cchar *a_src, cchar a_scope, cchar *a_target, cint b, cint x, cint y, cint z, char *a_out);
char        yRPN_addr_normal        (cchar *a_src, cint b, cint x, cint y, cint z, cint a_max, char *a_out);
char        yRPN_addr_require       (cchar *a_src, cchar a_scope, cint b, cint x, cint y, cint z, cint a_max, char *a_out);
char        yRPN_addr_provide       (cchar *a_src, cchar a_scope, cchar *a_target, cint b, cint x, cint y, cint z, cint a_max, char *a_out);
/*---(tokens)---------------*/
int         yrpn_addr_badref        (int a_pos);
int         yrpn_addr               (int a_pos, short a_ctab);
/*---(unittest)-------------*/
char        yrpn_addr__uinsider     (int b, int x, int y, int z);
char*       yrpn_addr__unit         (char *a_question, int a_item);
/*---(done)-----------------*/




/*===[[ yRPN_syms.c ]]=========================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
/*---(overall)--------------*/
/*---(cbang)----------------*/
/*---(literal)--------------*/
/*---(symbol)---------------*/
int         yrpn_syms_funcvar       (int a_pos);
/*---(operator)-------------*/
/*---(grouping)-------------*/
int         yrpn_group_sequence     (int a_pos);
int         yrpn_syms_ender         (int a_pos);
/*---(done)-----------------*/



/*===[[ yRPN_base.c ]]=========================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
/*---(support)--------------*/
char*       yRPN_version            (void);
char        yRPN_init               (cchar a_mode);
int         yRPN_errorpos           (void);
/*---(external)-------------*/
char        yRPN_compiler           (char *a_src, short a_tab, char **a_rpn, int a_nrpn, int a_max);
char        yRPN_interpret          (char *a_src, char **r_rpn, int *r_nrpn, int a_max, int a_def);
char        yRPN_gyges              (char *a_src, char **r_rpn, int *r_nrpn, int a_max, int a_def);
char        yRPN_math               (uchar *a_src, uchar a_type, uchar r_rpn [LEN_RECD], uchar *r_nrpn);
char        yRPN_normal             (char *a_src, char **r_rpn, int *r_nrpn, int a_max);
char        yRPN_detail             (char *a_src, char **r_rpn, int *r_nrpn, int a_max);
char        yRPN_techtoken          (char *a_src, char **r_rpn, int *r_nrpn, int a_max);
char        yRPN_parsed             (char *a_src, char **r_rpn, int *r_nrpn, int a_max);
char        yRPN_tokens             (char *a_src, char **r_rpn, int *r_nrpn, int a_max);
char        yRPN_pretty             (char *a_src, char **r_rpn, int *r_nrpn, int a_max);
/*---(driver)---------------*/
int         yrpn_base__prepare      (char *a_source);
char        yrpn_base__driver       (char *a_src, char a_type, char **a_rpn, int *a_nrpn, int a_max);
char        yrpn_base__convert      (char *a_source);
/*---(done)-----------------*/



/*===[[ yRPN_token.c ]]========================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
/*---(support)--------------*/
char        yrpn_token_error        (void);
/*---(parse)----------------*/
char        yrpn_token_accum        (int *a_pos);
/*---(unittest)-------------*/
char        yrpn_token__unit_prep   (char a_type, char *a_working, int *a_pos, int a_start);
/*---(done)-----------------*/



/*===[[ yRPN_test.c ]]=========================================*/
extern char unit_answer [ LEN_RECD ];
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char        yrpn__unit_insider     (int b, int x, int y, int z);
char*       yrpn__unit             (char*, int);
char        yrpn__unit_quick       (void);
char        yrpn__unit_quiet       (void);
char        yrpn__unit_loud        (void);
char        yrpn__unit_end         (void);



/*===[[ yRPN_nums.c ]]=========================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char        yrpn_nums__typing       (int a_pos);
char        yrpn_nums_one_int       (short l, char f, char p, char c);
char        yrpn_nums_one_float     (short l, char f, char p, char c);
char        yrpn_nums_one_bin       (short l, char f, char p, char c);
char        yrpn_nums_one_oct       (short l, char f, char p, char c);
char        yrpn_nums_one_hex       (short l, char f, char p, char c);
char        yrpn_nums_one_mongo     (short l, char f, char p, char c);
short       yrpn_nums_any           (short a_pos);



/*===[[ yRPN_fixed.c ]]========================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char        yrpn_fixed_one_char     (short l, char f, char p, char c);
short       yrpn_fixed_char         (short a_pos);
char        yrpn_fixed_one_str      (short l, char f, char p, char c, char e);
short       yrpn_fixed_string       (short a_pos);



/*===[[ yRPN_cbang.c ]]========================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char        yrpn_cbang_one          (short l, char f, char p, char c);
short       yrpn_cbang_keyword      (short a_pos);
short       yrpn_cbang_type         (short a_pos);



/*===[[ yRPN_cbang.c ]]========================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
/*---(program)--------------*/
char        yrpn_oper_init          (void);
char        yrpn_oper_wrap          (void);
/*---(support)--------------*/
char        yrpn_oper_spacing       (char *a_name, char *r_pre, char *r_suf, char r_pretty [LEN_SHORT]);
char        yrpn_oper_prec          (void);
char        yrpn_oper_combining     (void);
/*---(search)---------------*/
short       yrpn_oper__by_name      (char l, char a_name [LEN_SHORT]);
char        yrpn_oper_one           (short l, char f, char p, char c, char t);
/*---(drivers)--------------*/
char        yrpn_oper__mathy_one    (short a_pos);
char        yrpn_oper_mathy         (short a_pos);
int         yrpn_oper__splat        (int a_pos);
int         yrpn_oper_any           (int a_pos);
/*---(support)--------------*/
char        yRPN_symbols            (void);
char        yRPN_arity              (char *a_name, char *r_dir);
/*---(done)-----------------*/



/*===[[ yRPN_syms.c ]]=========================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char        yrpn_syms__classify     (short a_pos);
char        yrpn_syms_one           (short l, char f, char p, char c);


/*===[[ yRPN_exact.c ]]========================================*/
/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char        yrpn_exact__by_index    (char a_work [LEN_RECD], char a_index, char *r_type, char r_content [LEN_FULL], short *r_pos);
char        yrpn_exact__act_two     (char a_work [LEN_RECD], char a_index, char a_arity, char a_dir);
char        yrpn_exact__act_one     (char a_work [LEN_RECD], char a_index, char a_arity, char a_dir);




#endif
/*===[[ END ]]================================================================*/
