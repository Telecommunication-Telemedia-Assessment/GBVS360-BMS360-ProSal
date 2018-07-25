CC = clang++
CL = ar
CFLAGS = -fPIC -O2 -W -Wall -g -std=c++14 \
			 -I/usr/local/opt/opencv/include \
			 -Isrc

LDFLAGS = -L/usr/local/opt/boost/lib \
		  -L/usr/local/opt/opencv/lib \
		  -lopencv_core -lopencv_imgproc -lopencv_objdetect -lopencv_highgui -lopencv_imgcodecs -lopencv_videoio \
		  -lboost_program_options -lboost_exception -lboost_thread-mt -lboost_system -lboost_regex-mt 

CLFLAGS = rvs

SRC = $(wildcard src/*.cpp src/*.c)
OBJS = $(SRC:.cpp=.o)
RM = rm -f  # rm command
TARGET_LIB = bin/libbms.a # target lib

.PHONY: all
all: $(TARGET_LIB) 


$(TARGET_LIB): $(OBJS)
	$(CL) ${CLFLAGS} $@ $^

%.o : %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

include $(SRCS:.cpp=.d)

.PHONY: clean
clean:
	-${RM} ${TARGET_LIB} ${OBJS} $(SRCS:.c=.d)
