CC = g++

# turn off optimisations and include symbols to help with running in a debugger
CFLAGS = -Wall -O0 -g -Wl,--subsystem,console -DDEBUG -L ../lib -I ../include

# Any libraries you need
# LIBS = -lwininet
LIBS = -lfreeglut -lopengl32 -lglew32

all: testtest.exe

testtest.exe: main.cpp viewer.cpp
	$(CC) -o $@ $(CFLAGS) $^ $(LIBS) ctxgraf.dll
