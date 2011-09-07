CXX_LDFLAGS += -lmp3lame -lmpg123 -lsndfile -lfft -lfftw3
CXX_CFLAGS += -ggdb -Wall

all: audioenh


audioenh: main.o mwsConfigReader.o sndconv.o matrices.o voicebox.o
	${CXX} ${CXX_LDFLAGS} $^ -o $@


test_voicebox: test_voicebox.o voicebox.o matrices.o
	${CXX} ${CXX_LDFLAGS} $^ -o $@

test_sndconv: test_sndconv.o sndconv.o
	${CXX} ${CXX_LDFLAGS} $^ -o $@

test_specsub: test_specsub.o sndconv.o matrices.o voicebox.o
	${CXX} ${CXX_LDFLAGS} $^ -o $@

test_matrices: test_matrices.o matrices.o
	${CXX} ${CXX_LDFLAGS} $^ -o $@

%.o:%.cpp
	${CXX} ${CXX_CFLAGS} -c $< -o $@

clean:
	$(RM) *.o audioenh test_matrices test_sndconv test_voicebox test_specsub out.??? 
