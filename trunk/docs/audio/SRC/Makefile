CXX_LDFLAGS += -lmp3lame -lmpg123 -lsndfile


all: main


main: main.o mwsConfigReader.o sndconv.o
	${CXX} ${CXX_LDFLAGS} $^ -o $@


test_sndconv: test_sndconv.o sndconv.o
	${CXX} ${CXX_LDFLAGS} $^ -o $@


%.o:%.cpp
	${CXX} ${CXX_CFLAGS} -c $< -o $@

clean:
	$(RM) *.o main test_sndconv
