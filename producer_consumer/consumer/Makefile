#the compiler: define as gcc for C
CC = gcc

#compiler flags:
CFLAGS  = -g -Werror -Wall -Wno-unknown-pragmas 

# define any libraries to link into executable:
LIBS = -lpthread -lrt

# the build target executable:
TARGET=main
mkfile_path:=$(abspath $(lastword $(MAKEFILE_LIST)))
OUTPUT:=$(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

all: 
	$(CC) $(TARGET).c -o $(OUTPUT).out $(CFLAGS) $(LIBS)

clean:
	rm *.out
