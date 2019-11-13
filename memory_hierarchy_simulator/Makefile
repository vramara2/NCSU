CC = g++
OPT = -O3
#OPT = -g
WARN = -Wall
CFLAGS = $(OPT) 

# List all your .cc/.cpp files here (source files, excluding header files)
TOP_MODULE = top_module.cpp
CACHE_MODULE = cache_top.cpp
CACHE_L2_MODULE = cache_l2_top.cpp
# List corresponding compiled object files here (.o files)
SIM_OBJ = sim_cache.o
 
#################################

# default rule

all: sim_cache
	@echo "my work is done here..."


# rule for making sim_cache

sim_cache: 
	$(CC) -o sim_cache $(CFLAGS) $(TOP_MODULE) -lm
	@echo "-----------DONE WITH sim_cache-----------"


# generic rule for converting any .cpp file to any .o file
 
.cc.o:
	$(CC) $(CFLAGS)  -c $*.cc

.cpp.o:
	$(CC) $(CFLAGS)  -c $*.cpp


# type "make clean" to remove all .o files plus the sim_cache binary

clean:
	rm -f *.o sim_cache


# type "make clobber" to remove all .o files (leaves sim_cache binary)

clobber:
	rm -f *.o


