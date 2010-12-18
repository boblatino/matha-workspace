CC := g++
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./


all: mwsShObj.o mwsBuilder.o


mwsShObj.o: mwsShObj.cpp mwsShObj.h
	${CC} -I./ -Wall -c -o mwsShObj.o mwsShObj.cpp

mwsBuilder.o: mwsBuilder.cpp mwsBuilder.h
	${CC} -Wall -c mwsBuilder.cpp -o mwsBuilder.o



test_mwsShObj: mwsShObj.o libtest_ShObj.so test_mwsShObj.o
	${CC} -I./ -Wall -L./ -ltest_ShObj -ldl test_mwsShObj.o mwsShObj.o -o $@


libtest_ShObj.so: test_ShObj.o
	${CC} -shared -Wall,-soname,libtest_ShObj.so.1 -o libtest_ShObj.so.1.0.0 test_ShObj.o
	ln -sf libtest_ShObj.so.1.0.0 libtest_ShObj.so

test_ShObj.o: test_ShObj.cpp
	${CC} -I./ -c -Wall -fPIC -o $@ $<	



clean:
	rm -rf *.o *.o *.so* test_mwsShObj
