#include <ctype.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define VM_CAPACITY 8*1024
#define N_VMS 1
#define N_PARAMS 3


struct
vm_s {
	bool is_halted;
	bool is_primed;
	bool is_suspended;
	char name;
	int relative_base;
	long long int memory[VM_CAPACITY];
	long long int output;
	size_t current_address;
	size_t memory_length;
	size_t runs;
};


struct
operation {
	int code;
	long long int params[2];
	size_t dest;
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
			sscanf(token, "%lli", &vm.memory[address]);
			address++;
		}
	}

	vm.current_address = 0;
	vm.is_halted = false;
	vm.is_primed = false;
	vm.is_suspended = false;
	vm.memory_length = address;
	vm.relative_base = 0;
	vm.name = '?';
	vm.output = 0;
	vm.runs = 0;

	return vm;
}


void
write_value(struct vm_s *vm, size_t address, long long int value)
{
	if (address > VM_CAPACITY) {
		char *msg = "Error. Prevented write to address %zu on VM of size %zu.";
		errx(1, msg, address, VM_CAPACITY);
	}

	vm->memory[address] = value;
}


struct operation
get_operation(struct vm_s *vm, int address) {
	size_t i, modes[3];
	int offset;
	long long int raw_operation;
	struct operation op;

	/* Initialize all declarations */
	offset = op.dest = op.code = op.params[0] = op.params[1] = 0;
	modes[0] = modes[1] = modes[2] = 0;

	/* Figure out opcode and parameter modes */
	raw_operation = vm->memory[address];
	op.code = raw_operation % 100;
	if (op.code == 99) {
		return op;
	} else {
		modes[0] = raw_operation / 100 % 10;
		modes[1] = raw_operation / 1000 % 10;
		modes[2] = raw_operation / 10000 % 10;
	}

	/* Force opcode 3 to be immediate mode if mode unspecified */
	if (op.code == 3 && modes[0] == 0) modes[0] = 1;

	/* Work out what the 1st and 2nd parameters will be */
	for (i = 0; i < 2; i++) {
		switch (modes[i]) {
		case 0:  /* "Position" mode */
			op.params[i] = vm->memory[vm->memory[address+1+i]];
			break;
		case 1:  /* "Immediate mode */
			op.params[i] = vm->memory[address+i+1];
			break;
		case 2:  /* "Relative mode */
			if (op.code == 3) op.params[i] = vm->memory[address+1+i] + vm->relative_base;
			else op.params[i] = vm->memory[vm->memory[address+1+i]+vm->relative_base];
			break;
		}

		/* Opcodes 3, 4 and 9 only have one parameter. */
		if (op.code == 3 || op.code == 4 || op.code == 9) break;
	}

	if (op.code == 1 || op.code == 2 || op.code == 7 || op.code == 8) {
		if (modes[2] == 2) offset = vm->relative_base;
		op.dest = vm->memory[address + 3] + offset;
	}

	return op;
}


struct vm_s
run_vm(struct vm_s vm, int *inputs) {
	long long int result;
	size_t address, prev_address, current_input, value;
	struct operation op;

	value = result = current_input = 0;
	vm.is_suspended = false;

	while (!vm.is_halted && !vm.is_suspended) {
		address = prev_address = vm.current_address;

		if (address >= vm.memory_length) {
			errx(1, "Unexpected end of program.");
		}

		op = get_operation(&vm, address);

		switch (op.code) {
		case 1: /* Add */
			result = op.params[0] + op.params[1];
			write_value(&vm, op.dest, result);
			address += 4;
			break;

		case 2: /* Multiply */
			result = op.params[0] * op.params[1];
			write_value(&vm, op.dest, result);
			address += 4;
			break;

		case 3: /* Read */
			if (!vm.is_primed) {
			  write_value(&vm, op.params[0], inputs[current_input++]);
			} else {
			  write_value(&vm, vm.memory[address+1], inputs[1]);
			}
			// vm.is_primed = true;
			address += 2;
			break;

		case 4: /* Output */
			vm.output = op.params[0];
			vm.runs++;
			vm.is_suspended = true;
			address += 2;
			break;

		case 5: /* Jump if not zero */
			address = (op.params[0] != 0) ? op.params[1] : address+3;
			break;

		case 6: /* Jump if zero */
			address = (op.params[0] == 0) ? op.params[1] : address+3;
			break;

		case 7: /* Less than */
			result = op.params[0] < op.params[1];
			write_value(&vm, op.dest, result);
			address += 4;
			break;

		case 8: /* Equals to */
			result = op.params[0] == op.params[1];
			write_value(&vm, op.dest, result);
			address += 4;
			break;

		case 9: /* Adjust relative base */
			vm.relative_base += op.params[0];
			address += 2;
			break;	

		case 99: /* Halt */
			vm.is_halted = true;
			break;

		default:
			errx(1, "Error: unsupported opcode %d at address %zu.",
			     op.code, address);
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
	int inputs[N_VMS], vm_inputs[2], output, vm;
	size_t i;
	struct vm_s vms[N_VMS], tmp_vm;

	for (i = 0; i < sizeof(inputs)/sizeof(int); i++) {
		inputs[i] = 0;
	}

	if (argc == 1) {
		usage();
	} else {
		for (i = 1; i < (size_t)argc; i++) {
			inputs[i - 1] = atoi(argv[i]);
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

	printf("%lli\n", vms[N_VMS-1].output);

	return EXIT_SUCCESS;
}
