ROOT = ..
LIB = $(ROOT)/lib/libutils.a
PROGS = snp30

all: $(PROGS)
%: %.c $(LIB) 
	gcc -ggdb -o $@ $@.c -I$(ROOT)/include  -L$(ROOT)/lib -lutils -lapue
 

clean:
	rm -f $(PROGS) *.o

$(LIB):
	(cd $(ROOT)/lib && make -i)
