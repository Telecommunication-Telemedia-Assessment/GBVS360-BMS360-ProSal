CC = clang++
CL = ar
CLFLAGS = rvs

CFLAGS = -W -Wall -g \
			 -I/usr/local/opt/opencv3/include \
			 -I/usr/local/opt/boost/include \
			 -I/usr/local/opt/fftw/include \
			 -I../liblinper/src \
			 -I../libmeanshift/src 

LDFLAGS = -L/usr/local/opt/boost/lib \
		  -L/usr/local/opt/opencv3/lib \
		  -L/usr/local/opt/fftw/lib \
          -I/usr/local/opt/fftw/include \
          -L../liblinper/bin \
		  -L../libmeanshift/bin \
		  -lopencv_core -lopencv_imgproc -lopencv_objdetect -lopencv_highgui -lopencv_imgcodecs \
		  -lboost_program_options -lboost_exception -lboost_thread-mt -lboost_system \
		  -lfftw3 \
		  -llinper \
		  -lmean_shift

SRC = $(wildcard src/*.cpp src/*.cc)
OBJS = $(SRC:.cpp=.o)
AOUT = bin/gbvs

TARGET_LIB = bin/libgbvs.a

all : $(AOUT)  $(TARGET_LIB)

lib : $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	$(CL) ${CLFLAGS} $@ src/GBVS.o

bin/gbvs : $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
%.o : %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
clean :
	@rm src/*.o 
cleaner : clean
	@rm $(AOUT)

