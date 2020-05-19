/*
 Anusha, Vakamalla
 1001759496
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LINE 1024
int len, stln, opnum = 0,fpnum = 0,lrnum =0, mfnum = 0, ficl[100], mfcl[100], opcl[100], lrcl[100],e, f, h,working_set_size;
int matches(int sent) {
  opnum = 0;
  for (int e = 0; e < working_set_size; e++) {
    if (mfcl[e] == sent) {
      opnum = 1;
      break;
    }
  }return opnum;
}
int matchesfifo(int sent) {
  fpnum = 0;
  for (int e = 0; e < working_set_size; e++) {
    if (ficl[e] == sent) {
      fpnum = 1;
      break;
    }
  }
  return fpnum;
}
int matcheslru(int sent) {
  lrnum = 0;
  for (int e = 0; e < working_set_size; e++) {
    if (opcl[e] == sent) {
      lrnum = 1;
      break;
    }
  }
  return lrnum;
}
int matchesmfu(int sent) {
  mfnum = 0;
  for (int e = 0; e < working_set_size; e++) {
    if (lrcl[e] == sent) {
      mfnum = 1;
      break;
    }
  }
  return mfnum;
}
int getLocation(int sent) {
  int miss;

  for (h = 0; h < working_set_size; h++) {
    if (lrcl[h] == sent) {
      miss = h;
      break;
    }
  }
  return miss;
}
int main(int argc, char * argv[]) {
  char * line = NULL;
  size_t line_length = MAX_LINE;
  char * filename;
  int input[300];
  int k, i;
  int in ;
  int index = 0;
  FILE * file;
  if (argc < 2) {
    printf("Error: You must provide a datafile as an argument.\n");
    printf("Example: ./fp datafile.txt\n");
    exit(EXIT_FAILURE);
  }
  filename = (char * ) malloc(strlen(argv[1]) + 1);
  line = (char * ) malloc(MAX_LINE);
  memset(filename, 0, strlen(argv[1] + 1));
  strncpy(filename, argv[1], strlen(argv[1]));
  printf("Opening file %s\n", filename);
  file = fopen(filename, "r");
  if (file) {
    while (fgets(line, line_length, file)) {
      char * token;
      len = strlen(line);
      stln = (len - 1) / 2;
      token = strtok(line, " ");
      working_set_size = atoi(token);
      printf("\nWorking set size: %d\n", working_set_size);
      for (i = 0; i < stln; i++) {
        if (token != NULL) {
          token = strtok(NULL, " ");
          if (token != NULL) {
            in = atoi(token);
            input[i] = in ;
            printf("%d\t",input[i] );
          }
        }
      }
      int fpf = 0, faults = 0, lpf = 0, mpf = 0;
      for (e = 0; e < working_set_size; e++) {
        faults = 0;
        fpf = 0;
        lpf = 0;
        mpf = 0;
        mfcl[e] = 10000;
        ficl[e] = 10000;
        opcl[e] = 10000;
        lrcl[e] = 10000;
      }

      //FIFO Replacement
      for (e = 0; e < stln; e++) {
        if (matchesfifo(input[e]) == 0) {
          for (h = 0; h < working_set_size - 1; h++) {
            ficl[h] = ficl[h + 1];
          }
          ficl[h] = input[e];
          fpf++;
        }
      }
      printf("\nPage faults of FIFO: %d\n", fpf);
      //MFU Replacement
      int compare[220];
      int alpha = 0, beta, theta = 0;
      int highest;
      int location;
      for (e = 0; e < working_set_size; e++)
        compare[e] = 0;
      for (e = 0; e < stln; e++) {
        if (matchesmfu(input[e])) {
          int location = getLocation(input[e]);
          compare[location]++;
        } else {
          mpf++;
          if (working_set_size > alpha) {
            lrcl[alpha] = input[e];
            compare[alpha] = compare[alpha] + 1;
            alpha++;
          } else {
            highest = 0;
            for (h = 0; h < working_set_size; h++) {
              if (highest < compare[h]) {
                highest = compare[h];
                beta = h;
              }
            }
            lrcl[beta] = input[e];
            theta = 0;
            for (h = 0; h <= e; h++)
              if (input[e] == input[h])
                theta = theta + 1;
            compare[beta] = theta;
          }
        }
      }
      printf("Page faults of MFU: %d\n", mpf);
      //LRU Replacement
      int gotr[400];
      for (e = 0; e < stln; e++) {
        if (matcheslru(input[e]) == 0) {
          for (f = 0; f < working_set_size; f++) {
            int val = 0;
            int eq = opcl[f];
            for (h = e - 1; h >= 0; h--) {
              if (eq == input[h]) {
                gotr[f] = h;
                val = 1;
                break;
              } else {
                val = 0;
              }
            }
            if (!val) {
              gotr[f] = -10000;
            }
          }
          int highest = 10000;
          int change;
          for (f = 0; f < working_set_size; f++) {
            if (highest > gotr[f]) {
              highest = gotr[f];
              change = f;
            }
          }
          opcl[change] = input[e];
          lpf++;
        }
      }
      printf("Page faults of LRU: %d\n", lpf);

      //OPT Replacement 
      int ref[300];
      for (e = 0; e < stln; e++) {
        if (matches(input[e]) == 0) {
          for (f = 0; f < working_set_size; f++) {
            int val = 0;
            int eq = mfcl[f];
            for (h = e; h < stln; h++) {
              if (eq == input[h]) {
                ref[f] = h;
                val = 1;
                break;
              } else {
                val = 0;
              }
            }
            if (!val) {
              ref[f] = 10000;
            }
          }
          int highest = -10000;
          int change;
          for (f = 0; f < working_set_size; f++) {
            if (highest < ref[f]) {
              highest = ref[f];
              change = f;
            }
          }
          mfcl[change] = input[e];
          faults++;
        }
      }
      printf("Page faults of Optimal: %d\n", faults);
    }
    free(line);
    fclose(file);
  }
  return 0;
}
