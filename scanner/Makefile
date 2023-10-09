CXX=g++
CXXFLAGS=-std=c++14 -g -MMD
OBJECTS=wlp4scan.o main.o
DEPENDS=${OBJECTS:.o=.d}
EXEC=scanner

${EXEC}: ${OBJECTS}
	${CXX} ${OBJECTS} -o ${EXEC}

-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${DEPENDS} ${EXEC}