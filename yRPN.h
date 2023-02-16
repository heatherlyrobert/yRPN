/*============================----beg-of-source---============================*/
/*===[[ SUMMARY ]]============================================================*

 *   yRPN is a simple, clean, reliable, and maintainable set of functions to
 *   convert user-understandable infix notation into computer-efficient reverse
 *   polish notation.
 *
 */
/*===[[ WHY ]]================================================================*

 *   why would anyone use RPN ?
 *
 *   wiki : interpreters of RPN are often stack-based; that is, operands are
 *   pushed onto a stack, and when an operation is performed, its operands are
 *   popped from a staok and its result is pushed back on.  So the value of a
 *   postfix expression is on the top of the stack.  Stacks, and therefore RPN,
 *   have the advantage of being easy to implement and very fast.
 *
 *   RPN can handle arbitrary complexity with no limit
 *
 *   RPN requires no parentheses
 *
 *   RPN requires no precedence rules as operations are done as encountered
 *
 *
 *
 *
 */
/*===[[ PURPOSE ]]============================================================*

 *   yRPN is a set of shared functions providing simple, clean, reliable, and
 *   consistent translation from infix to postfix notation.  its purpuse is to
 *   take a painful, repetitous, but useful capability and make it reliable,
 *   testable, maintainable, shared, and documented.
 *
 *   it became obvious that this shared library made sense when i needed to
 *   bring the conversion code from out spreadsheet s over to our c compiler
 *   ananke, and potentially other places as well.  fixing, updating, testing,
 *   and then backporting would be a nightmare.
 *
 *   refactoring "DRY: don't repeat yourself" each piece of knowledge must have
 *   a single, unambiguous, authoritative representation in the system.  the
 *   process of consolidation, review, adaptation, and strengthening also
 *   automatically adds a great deal of quality changes and bombproofing.
 *
 *   above all, this design has been overcome by greater minds than ours.  why
 *   would we develop new and interesting ways to solve this -- unique for every
 *   application.  no way.  so, we solve it once and improve the shared solution
 *   as we learn.
 *
 *   yRPN will focus on...
 *      - use of dijkstra's shunting-yard algorithm (not a tree-based appoach)
 *      - c style operators, identifiers, and variables
 *      - fullsome unit testing to ensure proper functioning
 *      - highly transparent operations for clean debugging
 *      - keeping this functionality isolated for clear and complete testing
 *
 *   ySCHED will not...
 *      - understand anything about what the content
 *      - do anything with a resulting translation, this is just the library
 *      - provide any output or visual feedback other than logging
 *
 *   above all, yRPN is meant to enable technical power users access to the
 *   best conversion that we can provide, not to make it easy for novices.
 *
 *   as always, there are many, stable, accepted, existing programs and
 *   utilities built by better programmers that are likely superior in speed,
 *   size, capability, and reliability; BUT, i would not have learned nearly as
 *   much using them, so i follow the adage...
 *
 *   TO TRULY LEARN> do not seek to follow in the footsteps of the men of old;
 *   seek what they sought ~ Matsuo Basho
 *
 *   the underlying idea is to use this build process to learn new programming
 *   capabilities and put additional pressure on me to improve my standards,
 *   maintenance, and coding environment.  just using programs and ideas does
 *   not lead to understanding -- so get messy ;)
 *
 *   any one who wishes to become a good writer [programmer] should endeavour,
 *   before he allows himself to be tempted by the more showy qualities, to be
 *   direct, simple, brief, vigourous, and lucid -- henry watson fowler (1908)
 *
 *   simplicity is prerequisite for reliability and security
 *
 */
/*===[[ STANDARDS ]]==========================================================*
 *
 *   v   = symbols/variables
 *   o   = operator
 *   k   = keyword
 *   t   = type
 *   p   = procedure/function
 *
 *   C   = character literal
 *   S   = string literal
 *   I   = integer literal
 *   F   = float literal
 *   B   = binary literal
 *   X   = hex literal
 *   O   = octal literal
 *
 *   n   = named constants   (spreadsheet mode)
 *   @   = cell addresses    (spreadsheet mode)
 *
 * 123456789   integer/pointer paramaters
 * abcdefghi   float parameters
 *
 *   a   = argument assignment
 *   S,1,"testing"
 *   F,a,3.1415927
 *   I,2,20
 *   I,_,
 *
 *
 */
/*============================================================================*/



/*===[[ HEADER GUARD ]]=======================================================*/
#ifndef   YRPN_PUBLIC  
#define   YRPN_PUBLIC        loaded


#include <ySTR_solo.h>

/*===[[ CONSTANTS ]]======================================*/
#define   YRPN_TOKEN_NULL    "((null))"


#define     YRPN_TOKENS        't'
#define     YRPN_PARSED        'p'
#define     YRPN_PRETTY        'P'
#define     YRPN_MATHY         'M'
#define     YRPN_EXACT         'E'
#define     YRPN_SHUNTED       's'
#define     YRPN_DETAIL        'd'
#define     YRPN_DEBUG         'n'


/*---(overall)------------------------*/
#define     YRPN_IGNORE        '-'    /* nothing to do                      */
/*---(requires)-----------------------*/
#define     YRPN_RNONE         'n'    /* no cell adjustments                */
#define     YRPN_RREL          'r'    /* adjust all relative references     */
#define     YRPN_RINNER        'i'    /* adjust all (rel/abs) inner refs    */
#define     YRPN_RBOTH         'b'    /* adjust both inner and relative     */
#define     YRPN_REVERY        'e'    /* adjust all inner, rel, and abs     */
#define     YRPN_REQS          "nribe"
#define     YRPN_RINSIDE       "ibe"
/*---(provides)-----------------------*/
#define     YRPN_PNONE         'N'    /* no provider adjustments            */
#define     YRPN_PREL          'R'    /* adjust rel provider refs in source */
#define     YRPN_PPART         'P'    /* adjust rel parts of provider ref   */
#define     YRPN_PALL          'A'    /* adjust all provider refs in source */
#define     YRPN_PSPLIT        'S'    /* split provider refs in source      */
#define     YRPN_PROS          "NRPAS"
/*---(compound)-----------------------*/
#define     YRPN_ALL           "nribeNRPAS"
#define     YRPN_NONES         "-nN"


/*---(language support)---------------*/
#define      YRPN_CBANG        'c'
#define      YRPN_GYGES        'g'
#define      YRPN_BOTH         'B'

/*---(lower)-----------*/
#define      YRPN_KEYW          'k'    /* c language keyword                  */
#define      YRPN_TYPE          't'    /* c language variable type            */
#define      YRPN_ERROR         'e'    /* symbol is not understood            */
#define      YRPN_CONST         'n'    /* built-in constant                   */
#define      YRPN_OPER          'o'    /* built-in operator                   */
#define      YRPN_FUNC          'f'    /* user-defined function               */
#define      YRPN_ADDR          '@'    /* gyges addresss                      */
#define      YRPN_VARS          'v'    /* user-defined variable               */
#define      YRPN_LOCAL         'l'    /* agrios local variable               */
#define      YRPN_MEMB          'm'    /* structure member                    */
#define      YRPN_CAST          'c'    /* casting                             */
#define      YRPN_FPTR          '*'    /* function pointer                    */
#define      YRPN_PTYPE         'p'    /* variable type pointer               */
#define      YRPN_GROUP         '('    /* grouping operator                   */
/*---(upper)-----------*/
#define      YRPN_CHAR          'C'
#define      YRPN_STR           'S'
#define      YRPN_HEX           'X'
#define      YRPN_BIN           'B'
#define      YRPN_OCT           'O'
#define      YRPN_MONGO         'Z'
#define      YRPN_INT           'I'
#define      YRPN_FLOAT         'F'
/*---(combo)-----------*/
#define      YRPN_NONE          '-'
#define      YRPN_TYPES         "ktenof@vlmc*p(CSXBOZIF"
#define      YRPN_MATHERS       "n@vlmXBOZIF"
/*---(done)------------*/

typedef     const char     cchar;
typedef     const int      cint;
typedef     unsigned char  uchar;
typedef     unsigned short ushort;


/*===[[ PUBLIC FUNCTIONS ]]===============================*/

/*345678901-12345678901-12345678901-12345678901-12345678901-12345678901-123456*/
char*       yRPN_version            (void);
char        yRPN_init               (cchar a_mode);
int         yRPN_errorpos           (void);

char        yRPN_get                (uchar a_type, uchar r_rpn [LEN_RECD], uchar *r_nrpn);
char        yRPN_formula            (uchar *a_src, uchar a_type, uchar r_rpn [LEN_RECD], uchar *r_nrpn);
char        yRPN_math               (uchar *a_src, uchar a_type, uchar r_rpn [LEN_RECD], uchar *r_nrpn);


char        yRPN_compiler           (char *a_src, short a_tab, char **a_rpn, int a_nrpn, int a_max);
char        yRPN_gyges              (char *a_src, char **r_rpn, int *r_nrpn, int a_max, int a_def);

char        yRPN_interpret          (char *a_src, char **r_rpn, int *r_nrpn, int a_max, int a_def);
char        yRPN_normal             (char *a_src, char **r_rpn, int *r_nrpn, int a_max);
char        yRPN_detail             (char *a_src, char **r_rpn, int *r_nrpn, int a_max);
char        yRPN_techtoken          (char *a_src, char **r_rpn, int *r_nrpn, int a_max);
char        yRPN_parsed             (char *a_src, char **r_rpn, int *r_nrpn, int a_max);
char        yRPN_tokens             (char *a_src, char **r_rpn, int *r_nrpn, int a_max);
char        yRPN_pretty             (char *a_src, char **r_rpn, int *r_nrpn, int a_max);



/*===[ YRPN_ADJ.C ]]==========================================================*/
char        yRPN_addr_config        (void *a_breaker, void *a_maker, void *a_prettier, void *a_adjuster, void *a_insider);
char        yRPN_addr_normal        (cchar *a_src, cint b, cint x, cint y, cint z, cint a_max, char *a_out);
char        yRPN_addr_require       (cchar *a_src, cchar a_scope, cint b, cint x, cint y, cint z, cint a_max, char *a_out);
char        yRPN_addr_provide       (cchar *a_src, cchar a_scope, cchar *a_target, cint b, cint x, cint y, cint z, cint a_max, char *a_out);





char        yRPN_symbols            (void);
char        yRPN_arity              (char *a_name, char *r_dir);


#endif
/*===[[ END ]]================================================================*/
