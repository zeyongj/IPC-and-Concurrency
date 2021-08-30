APP=s-talk 
all:$(APP)

.c.o:
	gcc -c $*.c 
	# $*.o

$(APP): list.o stalk.o  
	gcc -Wall -pthread $^ -g -o $@


clean: 
	rm stalk.o
	rm -rf $(APP)
