#include <ctype.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MEMORY_CAPACITY 4096

/* TRACE=1 Dump the memory at each step and disable normal program output. */
/* TRACE=0 Quiet, normal program operation. */
#define TRACE 0 


struct memory_s {
	int data[MEMORY_CAPACITY];
	size_t length;
};


void usage() {
	printf("usage: intcode [-d]\n");
	exit(EXIT_FAILURE);
}

struct memory_s
read_dump()
{
	char *token, *line;
	size_t address, linesize;
	ssize_t linelen;

	static struct memory_s memory;

	address = 0;
	for (address = 0; address < MEMORY_CAPACITY; address++) {
		memory.data[address] = 0;
	}

	address = 0;
	line = NULL;
	while ((linelen = getline(&line, &linesize, stdin)) != -1) {
		while ((token = strsep(&line, ",")) != NULL) {
			sscanf(token, "%d", &memory.data[address]);
			address++;
		}
	}

	memory.length = address;

	return memory;
}

struct memory_s
write_value(struct memory_s memory, size_t address, int value)
{
	if (address > memory.length) {
		errx(1, "Out-of-bounds memory writing operation detected.");
	}

	memory.data[address] = value;
	return memory;
}


int
*parse_int(int input) {
	/* Deconstruct an input int into an opcode plus operation. */

	static int operation[4];
	operation[0] = input % 100;
	operation[1] = input / 100 % 10;
	operation[2] = input / 1000 % 10;
	operation[3] = input / 10000 % 10;
	return operation;
}


int
run_vm(struct memory_s memory, int *inputs) {
	int opcode, *operation, p1, p2, p3, output;
	size_t address, prev_address, steps, current_input;
	bool bye;

	steps = 1;
	p1 = p2 = p3 = current_input = output = address = 0;
	bye = false;

	while (!bye) {
		if (TRACE) {
			printf("PC=%zu [", address);
			for (size_t i = 0; i < memory.length; i++) {
				printf("%d%s", memory.data[i], (i < memory.length - 1) ? "," : "");
			}
			printf("]\n");
		}

		if (address >= memory.length) {
			errx(1, "Unexpected end of program.");
		}

		prev_address = address;
		operation = parse_int(memory.data[address]);
		opcode = operation[0];

		if (opcode == 1 || opcode == 2 || opcode == 7 || opcode == 8) {
			p1 = (operation[1] == 1) ? memory.data[address+1] : memory.data[memory.data[address+1]];
			p2 = (operation[2] == 1) ? memory.data[address+2] : memory.data[memory.data[address+2]];
			p3 = memory.data[address+3];
		} else if (opcode == 5 || opcode == 6) {
			p1 = (operation[1] == 1) ? memory.data[address+1] : memory.data[memory.data[address+1]];
			p2 = (operation[2] == 1) ? memory.data[address+2] : memory.data[memory.data[address+2]];
		}

		switch (opcode) {
		case 1:
			memory = write_value(memory, p3, p1 + p2);
			address += 4;
			break;
		case 2:
			memory = write_value(memory, p3, p1 * p2);
			address += 4;
			break;
		case 3:
			memory = write_value(memory, memory.data[address+1], inputs[current_input++]);
			address += 2;
			break;
		case 4:
			output = memory.data[memory.data[address+1]];
			address += 2;
			break;
		case 5:
			address = (p1 != 0) ? p2 : address+3;
			break;
		case 6:
			address = (p1 == 0) ? p2 : address+3;
			break;
		case 7:
			memory = write_value(memory, p3, (p1 < p2) ? 1 : 0);
			address += 4;
			break;
		case 8:
			memory = write_value(memory, p3, (p1 == p2) ? 1 : 0);
			address += 4;
			break;
		case 99:
			bye = true;
			address++;
			break;
		default:
			errx(1, "Error: unsupported opcode %d at address %zu.",
			     opcode, address);
			break;
		}

		if (prev_address == address) {
			errx(1, "Error: infinite loop detected.\n");
		}
	}

	return output;
}


int main(int argc, char *argv[]) {
	struct memory_s memories[5];
	int inputs[16], vm_inputs[2], arg, output, memory;
	size_t i;

	for (i=0; i < sizeof(inputs)/sizeof(int); i++) {
		inputs[i] = 0;
	}

	if (argc > 1) {
		for (arg=1; arg < argc; arg++) {
			inputs[arg-1] = *argv[arg] - '0';
		}
	}

	memories[0] = read_dump();
	memcpy(memories[1].data, memories[0].data, MEMORY_CAPACITY);
	memcpy(memories[2].data, memories[0].data, MEMORY_CAPACITY);
	memcpy(memories[3].data, memories[0].data, MEMORY_CAPACITY);
	memcpy(memories[4].data, memories[0].data, MEMORY_CAPACITY);
	memories[1].length = memories[0].length;
	memories[2].length = memories[0].length;
	memories[3].length = memories[0].length;
	memories[4].length = memories[0].length;

	output = 0;
	for (memory=0; memory<5; memory++) {
		vm_inputs[0] = inputs[memory];
		vm_inputs[1] = output;
		output = run_vm(memories[memory], vm_inputs);
	}

	printf("%d\n", output);

	return EXIT_SUCCESS;
}
