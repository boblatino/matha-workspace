CC := g++

export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:./


all: bin lib test

test: test_mwsShObj test_mwsShObj.o

lib:

bin: mathaws

MWSOBJS := mwsMain.o mwsConfigReader.o mwsWorkspace.o mwsFileBuilder.o \
	mwsGraphicAdapter.o mwsDrawable.o

INCDIR := .

CPPFLAGS := -g -I${INCDIR} -c -Wall -fPIC

LDFLAGS := -lglut -lGLU -lX11

mathaws: ${MWSOBJS}
	${CC} ${LDFLAGS} -o $@ $^

%.o: %.cpp
	${CC} ${CPPFLAGS} -o $@ $<

test_mwsShObj: libtest_ShObj.so test_mwsShObj.o
	${CC} -g -I./ -Wall -ldl test_mwsShObj.o -o $@

libtest_ShObj.so: test_ShObj.o
	${CC} -g -shared -Wall,-soname,libtest_ShObj.so.1 -o libtest_ShObj.so.1.0.0 test_ShObj.o
	ln -sf libtest_ShObj.so.1.0.0 libtest_ShObj.so

clean:
	rm -rf *.o *.o *.so* test_mwsShObj
