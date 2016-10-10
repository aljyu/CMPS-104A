#
# Partner: Alice Yu (aljyu)
# Partner: Paula Espiritu (mespirit)
#

GCC        = g++ -g -O0 -Wall -Wextra -std=gnu++14
MKDEP      = g++ -MM -std=gnu++14

MKFILE     = Makefile
DEPFILE    = Makefile.dep
SOURCES    = main.cpp auxlib.cpp stringset.cpp
HEADERS    = auxlib.h stringset.h
OBJECTS    = ${SOURCES:.cpp=.o}
EXECBIN    = oc
SRCFILES   = ${HEADERS} ${SOURCES} ${MKFILE}
GITHUB     = https://github.com
REPO       = /aljyu/CMPS-104A.git

all : ${EXECBIN}

${EXECBIN} : ${OBJECTS}
	${GCC} -o ${EXECBIN} ${OBJECTS}
	${MKDEP} ${SOURCES} >${DEPFILE}

%.o : %.cpp
	${GCC} -c $<

ci :
	git remote set-url origin ${USER}${REPO}
	git push --set-upstream ${GITHUB}${REPO} master
	git add ${SRCFILES} README PARTNER
	git commit
	git push ${GITHUB}${REPO}
    
clean :
	- rm ${OBJECTS} ${DEPFILE}

spotless : clean
	- rm ${EXECBIN}


deps :
	${MAKE} --no-print-directory ${DEPFILE}

again :
	${MAKE} spotless dep all
