CC=gcc
CFLAGS=-Wall
inc_cpu=include/cpu.h
inc_memory=include/memory.h
inc_cache=include/cache.h
inc_msg=include/messages.h
inc_rand=include/random_instruction.h
bus: src_C/memory.c src_C/cache_moesi.c src_C/random_instruction.c src_C/cpu.c src_C/bus.c $(inc_memory) $(inc_cache) $(inc_rand) $(inc_cpu) $(inc_msg)
	$(CC) $(CFLAGS) -o bin/bus src_C/memory.c src_C/cache_moesi.c src_C/random_instruction.c src_C/cpu.c src_C/bus.c -lrt -lpthread


cpu: src_C/cache_moesi.c src_C/cpu.c $(inc_cache) $(inc_rand) $(inc_cpu)
	$(CC) $(CFLAGS) -o bin/cpu src_C/cache_moesi.c src_C/random_instruction.c src_C/cpu.c  -lrt -lpthread

run_bus:
	bin/bus

run_msg:
	bin/message

run_cpu:
	bin/cpu

compile_bus:
	gcc src_C/memory.c src_C/bus.c -o bin/bus

compile_msg:
	gcc src_C/message.c -o bin/message -lrt -lpthread

compile_cpu:
	gcc src_C/cpu.c -o bin/cpu


all: consumer controller

clean:
	rm bin/*