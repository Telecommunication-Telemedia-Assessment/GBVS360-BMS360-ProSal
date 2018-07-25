CC = clang++
CFLAGS = -W -Wall -g  \
			 -I/usr/local/opt/opencv/include \
			 -I/usr/local/opt/boost/include \
			 -I../lib/libgnomonic/src -I../lib/libgnomonic/lib/libinter/src \
			 -I../lib/libjsonspirit/src \
			 -I../libgbvs360 \
			 -std=c++11

LDFLAGS = -L/usr/local/opt/boost/lib \
		  -L/usr/local/opt/opencv/lib \
		  -L../lib/libgnomonic/bin -L../lib/libgnomonic/lib/libinter/bin \
		  -L../lib/libjsonspirit/bin \
		  -lopencv_core -lopencv_imgproc -lopencv_objdetect -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio \
		  -lboost_program_options -lboost_exception -lboost_thread-mt -lboost_system -lboost_regex-mt -lboost_filesystem \
		  -lgnomonic -linter \
		  -ljson 

SRC = $(wildcard *.cpp)
OBJS = $(SRC:.cpp=.o)
AOUT = bin/gnomonic

all : $(AOUT) 

bin/gnomonic : $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
%.o : %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
clean :
	@rm *.o 
cleaner : clean
	@rm $(AOUT)

