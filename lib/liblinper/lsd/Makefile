CC = clang++
CL = ar
CLFLAGS = rvs

CFLAGS = -W -Wall -g 

LDFLAGS = 

SRC = $(wildcard src/*.cpp src/*.cc)
OBJS = $(SRC:.cpp=.o)

TARGET_LIB = bin/liblsd.a

all : $(TARGET_LIB)

$(TARGET_LIB): $(OBJS)
	$(CL) ${CLFLAGS} $@ $^

%.o : %.cpp
	$(CC) $(CFLAGS) -o $@ -c $<
clean :
	@rm src/*.o 
cleaner : clean
	@rm $(AOUT)

