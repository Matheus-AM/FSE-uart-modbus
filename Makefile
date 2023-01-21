CC = gcc
CCPP = g++
LDFLAGS = -lwiringPi -lpthread
BLDDIR = .
INCDIR = $(BLDDIR)/inc
SRCDIR = $(BLDDIR)/src
OBJDIR = $(BLDDIR)/obj
CFLAGS = -c -Wall -I$(INCDIR)
SRC = $(wildcard $(SRCDIR)/*.c)
SRCPP = $(wildcard $(SRCDIR)/*.cpp)
OBJ = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRC))
OBJPP = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRCPP))
EXE = bin/bin

all: clean $(EXE) 
    
$(EXE): $(OBJPP) 
	$(CCPP) $(LDFLAGS) $(OBJDIR)/*.o -o $@ 

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CCPP) $(LDFLAGS) $(CFLAGS) $< -o $@

libs:    
	$(CC) $(LDFLAGS) $(CFLAGS) $(SRC)
	cp ./*.o obj/
	rm ./*.o

clean:
	-rm -f $(OBJDIR)/*.o $(EXE)
