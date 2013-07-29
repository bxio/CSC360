CC = gcc
LINK = gcc

PROJECT = rose.exe
OBJFILES = j_vm.o j_float.o j_gc.o j_lang.o j_mem.o j_thread.o \
           j_vars.o j_bcode.o lookup.o timer.o 
#j_disk.c

CFLAGS = -O0 -Wall -g
LFLAGS = -g


$(PROJECT): $(OBJFILES) 
	$(LINK) -o $(PROJECT) $(OBJFILES)

%.o:%.c %.h

clean:
	rm -f $(PROJECT) j_vars.o j_vm.o j_lang.o j_thread.o j_disk.o lookup.o j_gc.o
