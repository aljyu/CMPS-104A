GCC        = g++ -g -O0 -Wall -Wextra -std=gnu++14
MKDEP      = g++ -MM -std=gnu++14

MKFILE     = Makefile
DEPFILE    = Makefile.dep
SOURCES    = main.cpp auxlib.cpp stringset.cpp astree.cpp lyutils.cpp \
             symtable.cpp symstack.cpp tablemanager.cpp emit.cpp
LSOURCES   = scanner.l
YSOURCES   = parser.y
CLGEN      = yylex.cpp
HYGEN      = yyparse.h
CYGEN      = yyparse.cpp
LREPORT    = yylex.output
YREPORT    = yyparse.output

HEADERS    = auxlib.h stringset.h astree.h lyutils.h yyparse.h \
             symtable.h symstack.h tablemanager.h emit.h
OBJECTS    = ${SOURCES:.cpp=.o} yyparse.o yylex.o
EXECBIN    = oc
SRCFILES   = ${HEADERS} ${SOURCES} ${MKFILE} ${LSOURCES} ${YSOURCES}
GITHUB     = https://github.com
REPO       = /aljyu/CMPS-104A.git

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${GCC} -o ${EXECBIN} ${OBJECTS}
	${MKDEP} ${SOURCES} >${DEPFILE}

${CLGEN} : ${LSOURCES}
	flex --outfile=${CLGEN} ${LSOURCES} 2>${LREPORT}
	- grep -v '^ ' ${LREPORT}

${CYGEN} ${HYGEN} : ${YSOURCES}
	bison --defines=${HYGEN} --output=${CYGEN} ${YSOURCES} \
                                          2>${YREPORT}

yylex.o : yylex.cpp
	# Suppress warning message from flex compilation.
	${GCC} -Wno-sign-compare -c $<

%.o : %.cpp
	${GCC} -c $<

ci :
	git remote set-url origin ${USER}${REPO}
	git push --set-upstream ${GITHUB}${REPO} master
	git add ${SRCFILES} README PARTNER
	git commit
	git push ${GITHUB}${REPO}
    
clean :
	- rm ${OBJECTS} ${DEPFILE} ${CLGEN} ${LREPORT} ${YREPORT} \
             ${CYGEN} ${HYGEN}

spotless : clean
	- rm ${EXECBIN}

deps : ${SOURCES} ${CLGEN} ${CYGEN}
	@ echo "# ${DEPFILE} created `date` by ${MAKE}" >${DEPFILE}
	${MKDEP} ${SOURCES} ${CLGEN} ${CYGEN} >>${DEPFILE}

again :
	${MAKE} spotless dep all
