CXX=g++
CXXFLAGS=-std=c++14 -g -MMD -w
OBJECTS=main.o tree.o wlp4gen.o
DEPENDS=${OBJECTS:.o=.d}
EXEC=generator

${EXEC}: ${OBJECTS}
	${CXX} ${OBJECTS} -o ${EXEC}

-include ${DEPENDS}

.PHONY: clean

clean:
	rm ${OBJECTS} ${DEPENDS} ${EXEC}