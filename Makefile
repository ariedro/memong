OBJS = main.cpp
OBJ_NAME = memong

all : $(OBJS)
	g++ $(OBJS) -w -lSDL2 -o $(OBJ_NAME)
