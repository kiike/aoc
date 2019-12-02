#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct opcode_buffer_s {
  int data[256];
  int length;
};


struct opcode_buffer_s load_opcodes() {
  int opcode_index;
  char cur_opcode[4], cur_char;

  static struct opcode_buffer_s opcode_buffer;

  opcode_index = 0;
  strlcpy(cur_opcode, "", 4);

  while ((cur_char = getchar()) != EOF) {
    if (isdigit(cur_char)) {
      strncat(cur_opcode, &cur_char, 1);
    }
    else if (cur_char == ',' || cur_char == '\n') {
      sscanf(cur_opcode, "%d", &opcode_buffer.data[opcode_index]);
      strlcpy(cur_opcode, "", 4);
      ++opcode_index;
    }
    else {
      continue;
    }
  }

  opcode_buffer.length = opcode_index;

  return opcode_buffer;
}

int main() {
  struct opcode_buffer_s opcode_buffer = load_opcodes();
  int r1, r2, r3;

  /* Repair alarm 1202 */
  opcode_buffer.data[1] = 12;
  opcode_buffer.data[2] = 2;

  int i = 0;
  int operation;
  while(true) {
    operation = opcode_buffer.data[i];
    if (operation == 1 || operation == 2) {
      r1 = opcode_buffer.data[i+1];
      r2 = opcode_buffer.data[i+2];
      r3 = opcode_buffer.data[i+3];
      if (operation == 1)
	opcode_buffer.data[r3] = opcode_buffer.data[r1] + opcode_buffer.data[r2];
      else if (operation == 2)
	opcode_buffer.data[r3] = opcode_buffer.data[r1] * opcode_buffer.data[r2];
    } else if (operation == 99) {
      break;
    }
    i += 4;
  }

  printf("%d\n", opcode_buffer.data[0]);

  return 0;
}
