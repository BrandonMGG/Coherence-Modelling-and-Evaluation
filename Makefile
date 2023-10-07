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

controller: src_C/cache_moesi.c src_C/cpu.c src_C/controller.c $(inc_cache) $(inc_rand) $(inc_cpu)
	$(CC) $(CFLAGS) -o bin/controller src_C/cache_moesi.c src_C/random_instruction.c src_C/cpu.c src_C/controller.c  -lrt

mainUI: src_C/memory.c src_C/messages.c src_C/cache_moesi.c src_C/random_instruction.c src_C/cpu.c src_C/bus.c mainUI.c $(inc_memory) $(inc_cache) $(inc_rand) $(inc_cpu) $(inc_msg)
	$(CC) $(CFLAGS) -o bin/mainUI src_C/memory.c src_C/messages.c src_C/cache_moesi.c src_C/random_instruction.c src_C/cpu.c src_C/bus.c mainUI.c -lrt -lpthread `pkg-config --cflags --libs gtk+-3.0`

run_bus:
	bin/bus

run_msg:
	bin/message

run_cpu:
	bin/cpu

run_controller:
	bin/controller

run_simulator:
	bin/mainUI


clean:
	rm bin/*