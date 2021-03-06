#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
  Solution for AoC 2019 Day 2.

  Restores the memory state from a "memory dump" passed by stdin and
  crunches the cost of a gravity assist.
*/

struct memory_s {
  int data[256];
  int length;
};


void usage() {
  printf("usage %s [-d] [-f target_velocity]\n", getprogname());
  exit(EXIT_FAILURE);
}


struct memory_s read_dump() {
  int index;
  char cur_int[4], cur_char;

  static struct memory_s memory;

  index = 0;
  strlcpy(cur_int, "", 4);

  while ((cur_char = getchar()) != EOF) {
    if (isdigit(cur_char)) {
      strncat(cur_int, &cur_char, 1);
    }
    else if (cur_char == ',' || cur_char == '\n' || cur_char == EOF) {
      sscanf(cur_int, "%d", &memory.data[index]);
      strlcpy(cur_int, "", 4);
      ++index;
    }
  }

  memory.length = index;

  return memory;
}


struct memory_s run_vm(struct memory_s memory) {
  int opcode, p1, p2, p3;
  for (int i = 0; i < memory.length; i += 4) {
    opcode = memory.data[i];
    if (opcode == 1 || opcode == 2) {
      p1 = memory.data[i+1];
      p2 = memory.data[i+2];
      p3 = memory.data[i+3];
      if (opcode == 1)
	memory.data[p3] = memory.data[p1] + memory.data[p2];
      else if (opcode == 2)
	memory.data[p3] = memory.data[p1] * memory.data[p2];
    } else if (opcode == 99) {
      break;
    }
  }
  return memory;
}


int main(int argc, char *argv[]) {
  struct memory_s memory, memory_bak;
  int ch, find, target_velocity, dump;

  /* Initialize options */
  dump = find = target_velocity = 0;

  while ((ch = getopt(argc, argv, "dhf:")) != -1) {
    switch (ch) {
    case 'd':
      dump = true;
      break;
    case 'h':
      usage();
      break;
    case 'f':
      find = true;
      sscanf(optarg, "%d", &target_velocity);
      break;
    }
  }

  memory = read_dump();

  if (find) {
    /* Part 2: Find noun and verb for gravity assist */
    for (int i = 0; i < 100; ++i) {
      for (int j = 0; j < 100; ++j) {
	memory_bak = memory;
	memory_bak.data[1] = i;
	memory_bak.data[2] = j;
	memory_bak = run_vm(memory_bak);
	if (memory_bak.data[0] == target_velocity) {
	  printf("%d%d\n", i, j);
	  break;
	}
      }
    }
  } else {
    /* Part 1: Repair alarm 1202 */
    memory.data[1] = 12;
    memory.data[2] = 2;
    memory = run_vm(memory);
    printf("%d\n", memory.data[0]);
  }

  if (dump) {
    /* Dump memory to a format suitable for loading */
    for (int i = 0; i < memory.length; ++i) {
      if (i > 0)
	printf(",");
      printf("%d", memory.data[i]);
    }
    printf("\n");
  }
  
  return EXIT_SUCCESS;
}
