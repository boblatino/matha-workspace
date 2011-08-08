CXX_LDFLAGS += -lmp3lame -lmpg123 -lsndfile


all: audioenh


audioenh: main.o mwsConfigReader.o sndconv.o
	${CXX} ${CXX_LDFLAGS} $^ -o $@


test_sndconv: test_sndconv.o sndconv.o
	${CXX} ${CXX_LDFLAGS} $^ -o $@


%.o:%.cpp
	${CXX} ${CXX_CFLAGS} -c $< -o $@

clean:
	$(RM) *.o audioenh test_sndconv out.{wav,ogg,mp3}
