# Project: rym

CPP  = g++
CC   = gcc
OBJ  = main.o hant.o graf.o vektorer.o draw.o shaderprogram.o
LINKOBJ  = main.o hant.o graf.o vektorer.o draw.o shaderprogram.o
LIBS =  -lSDL -lGL -lGLU
INCS =  
CXXINCS = 
BIN  = rym
CXXFLAGS = $(CXXINCS)   -std=c++0x -g -Ofast 
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: $(OBJ) $(BIN)
#all-before rym all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

main.o: maintub.cpp
	$(CPP) -c maintub.cpp -o main.o $(CXXFLAGS)

hant.o: hant.cpp hant.h
	$(CPP) -c hant.cpp -o hant.o $(CXXFLAGS)

graf.o: graf.cpp graf.h
	$(CPP) -c graf.cpp -o graf.o $(CXXFLAGS)

vektorer.o: vektorer.cpp vektorer.h
	$(CPP) -c vektorer.cpp -o vektorer.o $(CXXFLAGS)

$(BIN): $(OBJ)
	$(CPP) $(LINKOBJ) -o $(BIN) $(LIBS)