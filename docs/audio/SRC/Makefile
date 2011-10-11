CXX_LDFLAGS += -lmp3lame -lmpg123 -lsndfile -lfft -lfftw3
CXX_CFLAGS += -ggdb -Wall

all: audioenh


audioenh: main.o mwsConfigReader.o sndconv.o matrices.o voicebox.o
	${CXX} ${CXX_LDFLAGS} $^ -o $@


test_sndconv: test_sndconv.o sndconv.o
	${CXX} ${CXX_LDFLAGS} $^ -o $@

test_adaptivefilter: test_adaptivefilter.o adaptivefilter.o matrices.o sndconv.o voicebox.o
	${CXX} ${CXX_LDFLAGS} $^ -o $@

test_stft: test_stft.o stft.o matrices.o istft.o
	${CXX} ${CXX_LDFLAGS} $^ -o $@

test_lpc: test_lpc.o lpc.o matrices.o
	${CXX} ${CXX_LDFLAGS} $^ -o $@

test_specsub: test_specsub.o sndconv.o matrices.o voicebox.o
	${CXX} ${CXX_LDFLAGS} $^ -o $@

test_matrices: test_matrices.o matrices.o
	${CXX} ${CXX_LDFLAGS} $^ -o $@

%.o:%.cpp
	${CXX} ${CXX_CFLAGS} -c $< -o $@

clean:
	$(RM) *.o audioenh test_adaptivefilter test_matrices test_sndconv test_voicebox test_stft test_specsub out.??? 
