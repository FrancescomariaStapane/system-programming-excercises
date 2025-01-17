ROOT = .
#LIBUTILS = $(ROOT)/lib/libutils.a
LIBAPUE = $(ROOT)/lib/libapue.a
PROGS = shell3

all: $(PROGS)
%: %.c $(LIB) 
	gcc -ggdb -o $@ $@.c -I$(ROOT)/include  -L$(ROOT)/lib -lapue
 

clean:
	rm -f $(PROGS) *.o

#$(LIBUTILS):
#	(cd $(ROOT)/lib && make -i)

$(LIBAPUE):
	(cd $(ROOT)/lib && make -i)

