LIB = libutils.a

OBJS = fcopy.o

all: $(LIB)

$(LIB): $(OBJS)
	$(AR) rv $(LIB) $?
	ranlib $(LIB)
	
clean:
	rm -f *.o a.out core temp.* $(LIB)
