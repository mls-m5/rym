# Project: rym

CPP  = g++
CC   = gcc
OBJ  = hant.o graf.o vektorer.o roamingbroadphase.o

BIN  = rym
CXXFLAGS = -std=c++14 -Ofast 
RM = rm -f
GL = 3 #version of opengl, standard is 3

debug: CXXFLAGS+= -g -O0

ifeq ($(OS),Windows_NT)
	OBJ+= main-win.o
	LIBS = -Llib  -lopengl32 -mwindows
	BIN = rym.exe
	
	#Using opengl version 1:
	GL = 1
else
	LIBS =  -lSDL2 -lGL
	OBJ+= shaderprogram.o main-nix.o
endif

#selecting files according to opengl version
ifeq ($(GL), 1)
	OBJ+= draw-gl1.o
else
	OBJ+= draw.o
endif

.PHONY: all all-before all-after clean clean-custom

all: all-before $(OBJ) $(BIN)

all-before:
	@echo openGL version $(GL)

clean: clean-custom
	${RM} $(OBJ) $(BIN)

%.o: %.cpp %.h graf.h
	$(CPP) -c $< -o $@ $(CXXFLAGS)

$(BIN): $(OBJ)
	$(CPP) $(OBJ) -o $(BIN) $(LIBS)
	
