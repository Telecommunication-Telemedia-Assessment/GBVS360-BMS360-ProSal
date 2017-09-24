CC = clang++
CL = ar
CFLAGS = -fPIC -O2 -W -Wall -g \
			 -I/usr/local/opt/boost/include
LDFLAGS = -L/usr/local/opt/boost/lib 
CLFLAGS = rvs

SRC = $(wildcard src/*.cpp src/*.c)
OBJS = $(SRC:.cpp=.o)
RM = rm -f  # rm command
TARGET_LIB = bin/libjson.a # target lib


.PHONY: all
all: ${TARGET_LIB}

$(TARGET_LIB): $(OBJS)
	$(CL) ${CLFLAGS} $@ $^

%.o : %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<

include $(SRCS:.cpp=.d)

.PHONY: clean
clean:
	-${RM} ${TARGET_LIB} ${OBJS} $(SRCS:.c=.d)
