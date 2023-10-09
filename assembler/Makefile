CXX=g++
CXXFLAGS=-std=c++14 -g -MMD
OBJECTS=asm.o scanner.o main.o utility.o
DEPENDS=${OBJECTS:.o=.d}
EXEC=asm

${EXEC}: ${OBJECTS}
	${CXX} ${OBJECTS} -o ${EXEC}

-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${DEPENDS} ${EXEC}