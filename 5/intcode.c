#include <ctype.h>
#include <err.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MEMORY_CAPACITY 1024
#define MAX_RECORD_SIZE 4
#define DEFAULT_INPUT 5

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

struct memory_s read_dump() {
  char *token, *line;
  size_t pointer, linesize;
  ssize_t linelen;

  static struct memory_s memory;

  pointer = 0;
  line = NULL;

  while ((linelen = getline(&line, &linesize, stdin)) != -1) {
    while ((token = strsep(&line, ",")) != NULL) {
      sscanf(token, "%d", &memory.data[pointer]);
      pointer++;
    }
  }

  memory.length = pointer;

  return memory;
}

struct memory_s write_value(struct memory_s memory, size_t address, int value) {
  if (address > memory.length) {
    errx(1, "Out-of-bounds memory writing operation detected.");
  }

  memory.data[address] = value;
  return memory;
}


int *parse_int(int input) {
  /* Deconstruct an input int into an opcode plus operation. */

  static int operation[4];
  operation[0] = input % 100;
  operation[1] = input / 100 % 10;
  operation[2] = input / 1000 % 10;
  operation[3] = input / 10000 % 10;
  return operation;
}


struct memory_s run_vm(struct memory_s memory, int input) {
  int opcode, *operation, p1, p2, p3;
  size_t pointer, prev_pointer, steps;
  bool bye;

  steps = 1, p1 = p2 = p3 = pointer = 0, bye = false;

  while (!bye) {
    if (TRACE) {
      printf("PC=%zu [", pointer);
	for (size_t i = 0; i < memory.length; i++) {
	printf("%d%s", memory.data[i], (i < memory.length - 1) ? "," : "");
	}
      printf("]\n");
    }

    if (pointer >= memory.length)
      errx(1, "Unexpected end of program.");

    prev_pointer = pointer;
    operation = parse_int(memory.data[pointer]);
    opcode = operation[0];

    if (opcode == 1 || opcode == 2 || opcode == 7 || opcode == 8) {
      p1 = (operation[1] == 1) ? memory.data[pointer+1] : memory.data[memory.data[pointer+1]];
      p2 = (operation[2] == 1) ? memory.data[pointer+2] : memory.data[memory.data[pointer+2]];
      p3 = memory.data[pointer+3];
    } else if (opcode == 5 || opcode == 6) {
      p1 = (operation[1] == 1) ? memory.data[pointer+1] : memory.data[memory.data[pointer+1]];
      p2 = (operation[2] == 1) ? memory.data[pointer+2] : memory.data[memory.data[pointer+2]];
    }

    switch (opcode) {
    case 1:
      memory = write_value(memory, p3, p1 + p2);
      pointer += 4;
      break;
    case 2:
      memory = write_value(memory, p3, p1 * p2);
      pointer += 4;
      break;
    case 3:
      memory = write_value(memory, memory.data[pointer+1], input);
      pointer += 2;
      break;
    case 4:
      if (!TRACE) {printf("%d\n", memory.data[memory.data[pointer+1]]);}
      pointer += 2;
      break;
    case 5:
      pointer = (p1 != 0) ? p2 : pointer+3;
      break;
    case 6:
      pointer = (p1 == 0) ? p2 : pointer+3;
      break;
    case 7:
      memory = write_value(memory, p3, (p1 < p2) ? 1 : 0);
      pointer += 4;
      break;
    case 8:
      memory = write_value(memory, p3, (p1 == p2) ? 1 : 0);
      pointer += 4;
      break;
    case 99:
      bye = true;
      pointer++;
      break;
    default:
      errx(1, "Error: unsupported opcode %d at pointer %zu.",
	   opcode, pointer);
      break;
    }

    if (prev_pointer == pointer) {
      errx(1, "Error: infinite loop detected.\n");
    }
  }

  return memory;
}


int main(int argc, char *argv[]) {
  struct memory_s memory;
  int input;
  bool custom_input;

  /* Initialize options */
  custom_input = false;

  if (argc == 2) {
    input = *argv[1] - '0';
    custom_input = true;
  } else {
    input = DEFAULT_INPUT;
  }

  memory = read_dump();
  memory = run_vm(memory, input);

  return EXIT_SUCCESS;
}
