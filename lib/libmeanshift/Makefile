BOOST_MT=""
ifneq ($(OS),Windows_NT)
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S),Darwin)
        BOOST_MT=-mt
    endif
endif

CC = clang++
CL = ar
CLFLAGS = rvs

CFLAGS = -W -Wall -g -std=c++14  \
			 -I/usr/local/opt/opencv/include \
			 -I/usr/local/opt/boost/include 

LDFLAGS = -L/usr/local/opt/boost/lib \
		  -L/usr/local/opt/opencv/lib \
		  -lopencv_core -lopencv_imgproc -lopencv_objdetect -lopencv_highgui -lopencv_imgcodecs \
		  -lboost_program_options -lboost_exception -lboost_thread${BOOST_MT} -lboost_system 

SRC = $(wildcard src/*.cpp src/edge/*.cpp src/segm/*.cpp)
OBJS = $(SRC:.cpp=.o)

TARGET_LIB = bin/libmean_shift.a

all : $(TARGET_LIB)

lib : $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	$(CL) ${CLFLAGS} $@ $^

%.o : %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
clean :
	@rm src/*.o src/edge/*.o src/segm/*.o
cleaner : clean
	@rm $(AOUT)

