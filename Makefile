OBJS = ./engine/src/*.cpp ./lua/lua-5.3/src/*.c main.cpp

#CC specifies which compiler we're using
CC = x86_64-w64-mingw32-g++

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -I./lua/lua-5.3/src -I./mingw-dev/include/SDL2 -I./engine/include

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -L./lua/lua-5.3/src -L./mingw-dev/lib -L./lua/src

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window
COMPILER_FLAGS = -w -Wl,-subsystem,windows

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS = -static-libstdc++ -static-libgcc -Wl,-Bstatic -lstdc++ -lpthread -Wl,-Bdynamic -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua53

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = voidGEN

#This is the target that compiles our executable
all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)