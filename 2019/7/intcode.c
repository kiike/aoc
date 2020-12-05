#include <ctype.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define VM_CAPACITY 2048
#define N_VMS 5


struct
vm_s {
	bool is_halted;
	bool is_suspended;
	bool is_primed;
	char name;
	int memory[VM_CAPACITY];
	size_t current_address;
	size_t memory_length;
	size_t output;
	size_t runs;
};


void
usage() {
	printf("usage: intcode [INPUTS] < FILE\n");
	exit(EXIT_FAILURE);
}


struct vm_s
read_dump()
{
	char *token, *line;
	size_t address, linesize;
	ssize_t linelen;

	static struct vm_s vm;

	for (address = 0; address < VM_CAPACITY; address++) {
		vm.memory[address] = 0;
	}

	address = 0;
	line = NULL;
	while ((linelen = getline(&line, &linesize, stdin)) != -1) {
		while ((token = strsep(&line, ",")) != NULL) {
			sscanf(token, "%d", &vm.memory[address]);
			address++;
		}
	}

	vm.current_address = 0;
	vm.is_halted = false;
	vm.is_primed = false;
	vm.is_suspended = false;
	vm.memory_length = address;
	vm.name = '?';
	vm.output = 0;
	vm.runs = 0;

	return vm;
}


struct vm_s
write_value(struct vm_s vm, size_t address, int value)
{
	if (address > vm.memory_length) {
		errx(1, "Out-of-bounds vm write detected.");
	}

	vm.memory[address] = value;
	return vm;
}


int
*parse_int(int input_int) {
	/* Deconstruct an input int into an opcode and operation parameters. */

	static int operation[4];
	operation[0] = input_int % 100;
	operation[1] = input_int / 100 % 10;
	operation[2] = input_int / 1000 % 10;
	operation[3] = input_int / 10000 % 10;
	return operation;
}


struct vm_s
run_vm(struct vm_s vm, int *inputs) {
	int opcode, *operation, p1, p2, p3;
	size_t address, prev_address, current_input;

	p1 = p2 = p3 = current_input = 0;
	vm.is_suspended = false;

	while (!vm.is_halted && !vm.is_suspended) {
		address = prev_address = vm.current_address;

		if (address >= vm.memory_length) {
			errx(1, "Unexpected end of program.");
		}

		operation = parse_int(vm.memory[address]);
		opcode = operation[0];

		if (opcode == 1 || opcode == 2 || opcode == 7 || opcode == 8) {
			p1 = (operation[1] == 1) ? vm.memory[address+1] : vm.memory[vm.memory[address+1]];
			p2 = (operation[2] == 1) ? vm.memory[address+2] : vm.memory[vm.memory[address+2]];
			p3 = vm.memory[address+3];
		} else if (opcode == 5 || opcode == 6) {
			p1 = (operation[1] == 1) ? vm.memory[address+1] : vm.memory[vm.memory[address+1]];
			p2 = (operation[2] == 1) ? vm.memory[address+2] : vm.memory[vm.memory[address+2]];
		}

		switch (opcode) {
		case 1:
			vm = write_value(vm, p3, p1 + p2);
			address += 4;
			break;
		case 2:
			vm = write_value(vm, p3, p1 * p2);
			address += 4;
			break;
		case 3:
			if (!vm.is_primed) {
				vm = write_value(vm, vm.memory[address+1], inputs[current_input++]);
			} else {
				vm = write_value(vm, vm.memory[address+1], inputs[1]);
			}
			vm.is_primed = true;
			address += 2;
			break;
		case 4:
			vm.output = vm.memory[vm.memory[address+1]];
			vm.runs++;
			vm.is_suspended = true;
			address += 2;
			break;
		case 5:
			address = (p1 != 0) ? p2 : address+3;
			break;
		case 6:
			address = (p1 == 0) ? p2 : address+3;
			break;
		case 7:
			vm = write_value(vm, p3, (p1 < p2) ? 1 : 0);
			address += 4;
			break;
		case 8:
			vm = write_value(vm, p3, (p1 == p2) ? 1 : 0);
			address += 4;
			break;
		case 99:
			vm.is_halted = true;
			break;
		default:
			errx(1, "Error: unsupported opcode %d at address %zu.",
			     opcode, address);
			break;
		}

		if (!vm.is_halted && prev_address == address) {
			errx(1, "Error: infinite loop detected.\n");
		}

		vm.current_address = address;
	}

	return vm;
}


int
main(int argc, char *argv[]) {
	struct vm_s vms[N_VMS], tmp_vm;
	int inputs[N_VMS], vm_inputs[2], output, vm;
	size_t i;

	for (i = 0; i < sizeof(inputs)/sizeof(int); i++) {
		inputs[i] = 0;
	}

	if (argc > 1) {
		for (i = 1; i < (size_t)argc; i++) {
			inputs[i-1] = *argv[i] - '0';
		}
	}

	/* Initialize all VMs (amplifiers) */
	tmp_vm = read_dump();
	for (i = 0; i < N_VMS; i++) {
		vms[i] = tmp_vm;
		vms[i].name = i+'A';
	}

	/*
	 * Run each VM with the user-given input and the output
	 * of the previous amp.
	 */
	output = 0;
	vm = 0;
	while (!vms[N_VMS-1].is_halted) {
		vm_inputs[0] = inputs[vm];
		vm_inputs[1] = output;
		vms[vm] = run_vm(vms[vm], vm_inputs);
		output = vms[vm].output;
		vm++;
		vm = vm % N_VMS;
	}

	printf("%zu\n", vms[N_VMS-1].output);

	return EXIT_SUCCESS;
}
