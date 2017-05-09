objs    := sem.o tps.o queue.o

CC = gcc
CFLAGS  = -g -Wall

deps := $(patsubst %.o,%.d,$(objs)) 
-include $(deps)
DEPFLAGS = -MMD -MF $(@:.o=.d)

libuthread.a: 	sem.o tps.o queue.o
		ar rcs libuthread.a sem.o tps.o queue.o

%.o: %.c         
	$(Q)$(CC) $(CFLAGS) -c -o $@ $< $(DEPFLAGS)

clean:
		rm -f *.o *.a *.gch
		$(Q)rm -f $(targets) $(objs) $(deps)


