#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

int matchFuzzy(char *needle, char *path, int fuzzy);


// https://en.wikibooks.org/wiki/Algorithm_Implementation/Strings/Levenshtein_distance#C
#define MIN3(a, b, c) ((a) < (b) ? ((a) < (c) ? (a) : (c)) : ((b) < (c) ? (b) : (c)))
int levenshtein(char *s1, char *s2) {
    unsigned int s1len, s2len, x, y, lastdiag, olddiag;
    s1len = strlen(s1);
    s2len = strlen(s2);
    unsigned int column[s1len+1];
    for (y = 1; y <= s1len; y++)
        column[y] = y;
    for (x = 1; x <= s2len; x++) {
        column[0] = x;
        for (y = 1, lastdiag = x-1; y <= s1len; y++) {
            olddiag = column[y];
            column[y] = MIN3(column[y] + 1, column[y-1] + 1, lastdiag + (s1[y-1] == s2[x-1] ? 0 : 1));
            lastdiag = olddiag;
        }
    }
    return(column[s1len]);
}

int main(int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "Usage: %s function\n\n"
            "Function will be looked up at the autodocs.\n", argv[0]);
    return 20;
  }

  char *path = getenv("autodoc");
  if (path == NULL) {
    path = "/home/alex/t/amiga-gcc/projects/NDK_3.9/Documentation/Autodocs";
  }

  //printf("Path: %s\n", path);
  DIR *dir = opendir(path);
  if (dir == NULL) {
    fprintf(stderr, "%s error: %s when trying to get dir on \"%s\"", argv[0], strerror(errno), path);
    return 20;
  }

  /* If we find _LVO or LVO at the beginning of the argument, we skip it
     So if its LVOOpen, we reduce it to Open
  */
  int skip = 0;
  if (!strncmp("_LVO", argv[1], 4)) skip = 4;
  if (!strncmp("LVO", argv[1], 3)) skip = 3;
  if (skip) {
    argv[1]+=skip;
    //printf("%s\n", argv[1]);
  }

  /*
    Ok  this is  very  crude.  We  loop  through all  the  files in  the
    directory, and look at the table of contents at the top (till the \f
    char on the  first char on a new  line) When we get a  match, we run
    through the  whole file to  find it again, and  then we print  it at
    stdout.  Hey, it's instant...so...
  */
  char needle[256];
  needle[0]='\0';
  strcat(needle, "/");
  strcat(needle, argv[1]);
  strcat(needle, "\n");
  int done = 0;
  struct dirent *entry = readdir(dir);
  while (entry) {
    if (!strcmp(".", entry->d_name) || !strcmp("..", entry->d_name)) {
      /* yeah, we skip "." and ".." */
    } else {
      //printf("%s\n", entry->d_name);
      char buffer[256];
      buffer[0]='\0';
      strcat(buffer, path);
      strcat(buffer, "/");
      strcat(buffer, entry->d_name);
      FILE *file = fopen(buffer,"r");
      if (file) {
        while (fgets(buffer, sizeof(buffer), file)) {
          if (strstr(buffer, needle)) {
            // We found it at table of contents
            while (fgets(buffer, sizeof(buffer), file))
              if (strstr(buffer, needle))
                break;
            // And just above we found it again after TOC
            // Let's start printing it
            printf("%s", buffer+1); // eat the \f at the beggining
            while (fgets(buffer, sizeof(buffer), file)) {
              if (buffer[0] == '\f') {
                done = 1;
                break;
              }
              printf("%s", buffer);
            }
          }
          if (done) break;
          if (buffer[0] == '\f') break;
        }
        fclose(file);
      }
    }
    if (done) break;
    entry = readdir(dir);
  }
  closedir(dir);
  if (!done) {
    int fuzziness = 0;
    while (!done && (fuzziness < 5)) {
      ++fuzziness;
      done = matchFuzzy(argv[1], path, fuzziness);
    }

    if (done)
      printf("\n\n%d fuzzy match%s.\n", done, (done > 1) ? "es" : "");
    else
      printf("\n\nNo match...\n");
  }
  return 0;
}

int matchFuzzy(char *needle, char *path, int fuzzy) {
  int match = 0;
  DIR *dir = opendir(path);
  struct dirent *entry = readdir(dir);
  while (entry) {
    if (!strcmp(".", entry->d_name) || !strcmp("..", entry->d_name)) {
      /* yeah, we skip "." and ".." */
    } else {
      char *function;
      char buffer[256];
      buffer[0]='\0';
      strcat(buffer, path);
      strcat(buffer, "/");
      strcat(buffer, entry->d_name);
      FILE *file = fopen(buffer, "r");
      if (file) {
        while (fgets(buffer, sizeof(buffer), file)) {
          if (buffer[0] == '\f') break;
          if ((function = strchr(buffer, '/'))) {
            int d = levenshtein(function+1, needle);
            if (d <= fuzzy) {
              printf("%s", buffer);
              ++match;
            }
          }
        }
        fclose(file);
      }
    }
    entry = readdir(dir);
  }
  closedir(dir);

  return match;
}

