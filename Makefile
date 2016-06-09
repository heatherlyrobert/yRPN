#*============================----beg-of-source---============================*#

BASE    = yRPN
TEST    = ${BASE}_unit

#*---(standard variables)-------------*#
COMP    = gcc -ansi -Wall -Wextra -fPIC
#COMP    = tcc -g -Wall -Wunsupported -bench
#AS_LIB  = -shared -soname lib${BASE}.so.1  -o lib${BASE}.so.1.0
AS_LIB  = -shared -Wl,-soname,lib${BASE}.so.1  -o lib${BASE}.so.1.0
INCS    = 

LINK    = gcc
LIBS    = -L/usr/local/lib -lyLOG

COPY    = cp -f
CLEAN   = rm -f
PRINT   = @printf



#*---(main)---------------------------*#
all                : gcc test



#*---(parts)--------------------------*#
gcc                : ${BASE}.h ${BASE}.c
	${PRINT}  "\n--------------------------------------\n"
	${PRINT}  "compile ${BASE} using tcc\n"
	${COMP}   ${AS_LIB}  ${LIBS}  ${BASE}.c

test               : ${BASE}.h ${BASE}.c ${BASE}.unit
	koios    ${BASE}
	${COMP}   -o ${BASE}_unit  ${BASE}.c ${BASE}_unit.c  ${INCS} ${LIBS} -lyVAR -lyUNIT



#*---(utilities)----------------------*#
clean              :
	${PRINT}  "\n--------------------------------------\n"
	${PRINT}  "cleaning out local object, backup, and temp files\n"
	${CLEAN}  *.o
	${CLEAN}  *~
	${CLEAN}  temp*
	${CLEAN}  lib${BASE}*

bigclean           :
	${PRINT}  "\n--------------------------------------\n"
	${PRINT}  "clean out local swap files\n"
	${CLEAN}  '.'*.sw?

install            :
	${PRINT}  "\n--------------------------------------\n"
	${PRINT}  "install ${BASE} into production\n"
	sha1sum   lib${BASE}.so.1.0
	_lib      -s ${BASE}
	ldconfig
	rm -f     /usr/share/man/man3/${BASE}.3.bz2
	cp -f     ${BASE}.3    /usr/share/man/man3/
	bzip2     /usr/share/man/man3/${BASE}.3
	chmod     0644  /usr/share/man/man3/${BASE}.3.bz2

remove             :
	${PRINT}  "\n--------------------------------------\n"
	${PRINT}  "remove ${BASE} from production\n"
	_lib      -d  ${BASE}
	ldconfig

#*============================----end-of-source---============================*#
