CC := g++
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./


all: mwsShObj.o mwsBuilder.o


mwsShObj.o: mwsShObj.cpp mwsShObj.h
	${CC} -g -I./ -Wall -c -o mwsShObj.o mwsShObj.cpp

mwsBuilder.o: mwsBuilder.cpp mwsBuilder.h
	${CC} -g -Wall -c mwsBuilder.cpp -o mwsBuilder.o





test_mwsShObj: libtest_ShObj.so test_mwsShObj.o
	${CC} -g -I./ -Wall -ldl test_mwsShObj.o -o $@

test_mwsShObj.o: test_mwsShObj.cpp
	${CC} -g -I./ -Wall -c -o $@ $<

libtest_ShObj.so: test_ShObj.o
	${CC} -g -shared -Wall,-soname,libtest_ShObj.so.1 -o libtest_ShObj.so.1.0.0 test_ShObj.o
	ln -sf libtest_ShObj.so.1.0.0 libtest_ShObj.so

test_ShObj.o: test_ShObj.cpp
	${CC} -g -I./ -c -Wall -fPIC -o $@ $<	



clean:
	rm -rf *.o *.o *.so* test_mwsShObj