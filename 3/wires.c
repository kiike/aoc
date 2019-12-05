#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define N 200000

int main() {
     int x, y, wires[2][N][2], wire;
     char *line = NULL, *tok, direction;
     size_t magnitude, i, j, point, linesize = 0;

     wire = 0;
     while (getline(&line, &linesize, stdin) != -1) {
	  x = y = point = 0;
	  while ((tok = strsep(&line, ",")) != NULL) {
	       sscanf(tok, "%c%zu", &direction, &magnitude);

	       for (i = 0; i < magnitude; i++) {
		    switch (direction) {
		    case 'U':
			 y++;
			 break;
		    case 'D':
			 y--;
			 break;
		    case 'L':
			 x--;
			 break;
		    case 'R':
			 x++;
			 break;
		    }
		    wires[wire][point][0] = x;
		    wires[wire][point][1] = y;
		    point++;
	       }
	  }
	  wire++;
     }

     for (i = 0; i < N; i++) {
	  for (j = 0; j < N; j++) {
	       if (wires[0][i][0] == wires[1][j][0] &&
		   wires[0][i][1] == wires[1][j][1]) {
		    printf("%d\n", wires[0][i][0] + wires[0][i][1]);
	       }
	  }
     }

     return EXIT_SUCCESS;
}
