#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define LOWER 231832
#define UPPER 767346


bool check_two_adjacent_digits(int input[]) {
  int tmp = input[0];
  for (size_t i = 1; i < 6; i++) {
    if (input[i] == tmp) {
      return true;
    }
    tmp = input[i];
  }
  return false;
}

bool check_part_2(int input[]) {
  int repetitions[6];
  for (size_t i = 0; i < 6; i++) {
    size_t counter = 0;
    for (size_t j = 0; j < 6; j++) {
      if (input[i] == input[j])
	counter++;
    }
    repetitions[i] = counter;
  }
  for (size_t i = 0; i < 6; i++) {
    printf("%d,", repetitions[i]);
    if (repetitions[i] == 2) {
      return true;
    }
  }
  return false;
}

bool check_always_increasing_digits(int input[]) {
  int tmp = input[0];
  for (size_t i = 1; i < 6; i++) {
    if (input[i] < tmp) {
      return false;
    }
    tmp = input[i];
  }
  return true;
}


int main() {
  int password[6];
  char password_str[7];
  size_t count_part1, count_part2, i, j;

  count_part1 = count_part2 = 0;
  for (i = LOWER; i <= UPPER; i++) {
    sprintf(password_str, "%zu", i);
    for (j = 0; j < 6; j++) {
      password[j] = password_str[j] - '0';
    }

    if (check_two_adjacent_digits(password) &&
	check_always_increasing_digits(password)) {
      count_part1++;
    }

    if (check_two_adjacent_digits(password) &&
	check_always_increasing_digits(password) &&
	check_part_2(password)) {
      count_part2++;
    }
  }
  printf("%zu, %zu\n", count_part1, count_part2);
  return 0;
}
