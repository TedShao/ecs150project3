objs    := queue.o sem.o tps.o

CC = gcc
CFLAGS  = -g -Wall

ifneq ($(V),1)
Q=@
endif

deps := $(patsubst %.o,%.d,$(objs)) 
-include $(deps)
DEPFLAGS = -MMD -MF $(@:.o=.d)
	

libuthread.a: 	$(objs) thread.o
		ar rcs libuthread.a $(objs) thread.o

queue.o:    queue.c queue.h	
	    $(CC) $(CFLAGS) -c -o queue.o queue.c $(DEPFLAGS)

sem.o:	    sem.h queue.h thread.h
	    $(CC) $(CFLAGS) -c -o sem.o sem.c $(DEPFLAGS)
	
tps.o:	    tps.h queue.h thread.h
	    $(CC) $(CFLAGS) -c -o tps.o tps.c $(DEPFLAGS)
clean:
	    $(Q)echo "clean"
	    $(Q)rm -f $(objs) libuthread.a $(deps)


