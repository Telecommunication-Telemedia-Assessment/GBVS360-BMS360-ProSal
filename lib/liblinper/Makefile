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
			-I/usr/local/opt/boost/include \
			-I/usr/local/opt/opencv/include \
			-Ijlinkage/include \
			-Ilsd/src

LDFLAGS = -L/usr/local/opt/boost/lib \
			-L/usr/local/opt/opencv/lib \
			-Lbin \
			-lopencv_core -lopencv_imgproc -lopencv_objdetect -lopencv_highgui -lopencv_imgcodecs \
			-lboost_program_options -lboost_exception -lboost_thread${BOOST_MT} -lboost_system \
			-llinper

SRC = $(wildcard ./src/*.cpp jlinkage/src/*.cpp lsd/src/*.cpp)
OBJS = $(SRC:.cpp=.o)

TARGET_LIB = bin/liblinper.a
TARGET = bin/linper

all : $(TARGET_LIB) $(TARGET)

$(TARGET_LIB): $(OBJS)
	$(CL) ${CLFLAGS} $@ $^


$(TARGET): main.o  $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o : %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
clean :
	@rm jlinkage/src/*.o lsd/src/*.o ./*.o ./src/*.o
cleaner : clean
	@rm $(AOUT)



