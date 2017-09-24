CC = clang++
CL = ar
CLFLAGS = rvs

CFLAGS = -W -Wall -g \
			 -I/usr/local/opt/opencv3/include \
			 -I/usr/local/opt/boost/include 

LDFLAGS = -L/usr/local/opt/boost/lib \
		  -L/usr/local/opt/opencv3/lib \
		  -lopencv_core -lopencv_imgproc -lopencv_objdetect -lopencv_highgui -lopencv_imgcodecs \
		  -lboost_program_options -lboost_exception -lboost_thread-mt -lboost_system 

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

