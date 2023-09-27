run_bus:
	bin/bus

run_msg:
	bin/message

compile_bus:
	gcc src_C/memory.c src_C/bus.c -o bin/bus

compile_msg:
	gcc src_C/message.c -o bin/message -lrt -lpthread

clean:
	rm bin/*