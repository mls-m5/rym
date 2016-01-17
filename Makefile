# Project: rym

CPP  = g++
CC   = gcc
OBJ  = hant.o graf.o vektorer.o

BIN  = rym
CXXFLAGS = -std=c++0x -g -Ofast 
RM = rm -f
GL = 3 #version of opengl, standard is 3

ifeq ($(OS),Windows_NT)
	CXXFLAGS+= -Iwin
	OBJ+= main-win.o
	LIBS = -Llib  -lopengl32 -mwindows
	BIN = rym.exe
	GL = 1 #Using opengl version 1
else
	LIBS =  -lSDL2 -lGL
	OBJ+= draw.o shaderprogram.o main-nix.o
endif

#selecting files according to opengl version
ifeq ($(GL), 1)
	OBJ+= draw-gl1.o
else
	OBJ+= draw.o
endif

.PHONY: all all-before all-after clean clean-custom

all: $(OBJ) $(BIN)

clean: clean-custom
	${RM} $(OBJ) $(BIN)

%.o: %.cpp
	$(CPP) -c $< -o $@ $(CXXFLAGS)


$(BIN): $(OBJ)
	$(CPP) $(OBJ) -o $(BIN) $(LIBS)
	
