CC = clang++
CFLAGS = -W -Wall -g  \
			 -I/usr/local/opt/opencv/include \
			 -I/usr/local/opt/boost/include \
			 -I/usr/local/opt/fftw/include \
			 -I../lib/libgnomonic/src -I../lib/libgnomonic/lib/libinter/src \
			 -I../lib/libbms/src \
			 -I../lib/libgbvs/src \
			 -I../lib/libhmd/src \
			 -I../lib/liblinper/src \
			 -I../lib/libmeanshift/src \
			 -I../libgbvs360 

LDFLAGS = -L/usr/local/opt/boost/lib \
		  -L/usr/local/opt/opencv/lib \
		  -L/usr/local/opt/fftw/lib \
		  -I/usr/local/opt/fftw/include \
		  -L../lib/libgnomonic/bin -L../lib/libgnomonic/lib/libinter/bin \
		  -L../lib/libbms/bin \
		  -L../lib/libgbvs/bin \
		  -L../lib/liblinper/bin \
		  -L../lib/libhmd/bin \
		  -L../lib/libmeanshift/bin \
		  -lopencv_core -lopencv_imgproc -lopencv_objdetect -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio \
		  -lboost_program_options -lboost_exception -lboost_thread${BOOST_MT} -lboost_system -lboost_regex${BOOST_MT} \
		  -lgnomonic -linter \
		  -lbms \
		  -lgbvs \
		  -lfftw3 \
		  -llinper \
		  -lmean_shift \
		  -lhmd


SRC = $(wildcard ../libgbvs360/*.cpp ../libgbvs360/*.cc src/*.cpp)
OBJS = $(SRC:.cpp=.o)
AOUT = bin/baseline



all : $(AOUT) $(PRIOR)

bin/baseline : $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@ 
%.o : %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
clean :
	@rm src/*.o 
cleaner : clean
	@rm $(AOUT)

