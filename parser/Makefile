CXX=g++
CXXFLAGS=-std=c++14 -g -MMD
OBJECTS=main.o wlp4parse.o
DEPENDS=${OBJECTS:.o=.d}
EXEC=parser

${EXEC}: ${OBJECTS}
	${CXX} ${OBJECTS} -o ${EXEC}

-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${DEPENDS} ${EXEC}