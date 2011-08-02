
all: main

main: main.o mwsConfigReader.o mwsConfigReader.h 
	${CXX} main.o mwsConfigReader.o -o main

%.o:%.cpp
	${CXX} ${CXX_CFLAGS} -c $< -o $@

clean:
	$(RM) *.o main
