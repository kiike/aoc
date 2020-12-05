#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
  Solution for Advent of Code Day #1 (https://adventofcode.com/2019/day/1)
  
  Reads numbers from stdin and returns the "fuel" needed to return the
  spacecraft. Supports two fuel calculation methods: the complete and
  the naive methods.

  Options:
  * -n use the naive (part 1) fuel calculation formula
  * -c use the complete fuel calculation formula (part 2).
 */

void usage()
{
  printf("usage: %s [-nc] < STDIN\n", getprogname());
  exit(1);
}


long crunch_module_cost(size_t mass, short method)
{
  long cost = floor(mass / 3) - 2;
  if (method == 1)
    return cost;

  if (cost > 0)
    return cost + crunch_module_cost(cost, method);
  else
    return 0;
}


int main(int argc, char** argv)
{
  size_t total_cost = 0;
  size_t mass;
  long cost;
  short method = 2;
  int c;

  while ((c = getopt(argc, argv, "n::c::")) != -1) {
    switch (c) {
    case 'n': method = 1;
      break;
    case 'c': method = 2;
      break;
    default: usage();
    }
  }

  while (true) {
    if (scanf("%zu", &mass) == EOF)
      break;

    cost = crunch_module_cost(mass, method);
    total_cost += cost;
  }

  printf("%zu\n", total_cost);
  return 0;
}
