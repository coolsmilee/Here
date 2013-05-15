objects =   ../common/protocol.o server.o operate_funcs.o dbapi.o

server : $(objects)
	gcc -o server $(objects) -lmysqlclient

server.o : operate_funcs.h ../common/protocol.h ../common/debug.h

../common/protocol.o : ../common/protocol.h ../common/debug.h

operate_funcs.o : dbapi.h operate_funcs.h ../common/protocol.h ../common/debug.h

dbapi.o : dbapi.h
	gcc -c dbapi.c 

clean :
	-rm server $(objects)
