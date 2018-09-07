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

CFLAGS = -W -Wall -g -std=c++14 \
			 -I/usr/local/opt/opencv/include \
			 -I/usr/local/opt/boost/include 

LDFLAGS = -L/usr/local/opt/boost/lib \
		  -L/usr/local/opt/opencv/lib \
		  -lopencv_core -lopencv_imgproc -lopencv_objdetect -lopencv_highgui -lopencv_imgcodecs \
		  -lboost_program_options -lboost_exception -lboost_thread${BOOST_MT} -lboost_system 

SRC = $(wildcard src/*.cpp)
OBJS = $(SRC:.cpp=.o)
AOUT = bin/hmd

TARGET_LIB = bin/libhmd.a

all : $(AOUT)  $(TARGET_LIB)

lib : $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	$(CL) ${CLFLAGS} $@ src/HMDSim.o

bin/hmd : $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^
%.o : %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
clean :
	@rm src/*.o 
cleaner : clean
	@rm $(AOUT)

