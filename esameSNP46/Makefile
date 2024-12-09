ROOT = ..
LIB = $(ROOT)/lib/libutils.a
PROGS = snp46

all: $(PROGS)
%: %.c $(LIB) 
	gcc -ggdb -o $@ $@.c -I$(ROOT)/include  -L$(ROOT)/lib -lutils
 

clean:
	rm -f $(PROGS) *.o

$(LIB):
	(cd $(ROOT)/lib && make -i)
