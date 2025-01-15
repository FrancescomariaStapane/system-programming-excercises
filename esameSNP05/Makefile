ROOT = ..
LIBUTILS = $(ROOT)/lib/libutils.a
LIBUTILS = $(ROOT)/lib/libapue.a
PROGS = UDPClient UDPServer

all: $(PROGS)
%: %.c $(LIB) 
	gcc -ggdb -o $@ $@.c -I$(ROOT)/include  -L$(ROOT)/lib -lutils -lapue
 

clean:
	rm -f $(PROGS) *.o

$(LIBUTILS):
	(cd $(ROOT)/lib && make -i)

$(LIBAPUE):
	(cd $(ROOT)/lib && make -i)
