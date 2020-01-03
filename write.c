#include "keys.h"

int main(int argc, char const *argv[]) {
  int semd, shmd, file;
  char text[SEG_SIZE];
  struct sembuf semaphorebuffer;

  //semaphorebuffer.sem_num = 0;
  semaphorebuffer.sem_op = -1;

  semd = semget(SEM_KEY, 1, 0);
  shmd = shmget(SHR_KEY, SEG_SIZE, 0644);

  if (semd < 0) {
    printf("semaphore error: %s\n", strerror(errno));
    return errno;
  }
  else {
    printf("Trying to get in\n");
    semop(semd, &semaphorebuffer, 1);
    printf("Got the semaphore\n");
  }

  if (shmd < 0) {
    printf("shared memory error: %s\n", strerror(errno));
    return errno;
  }

  file = open("text.txt", O_WRONLY | O_APPEND);

  char *lastline = shmat(shmd, 0, 0);
  printf("Most recent addition: %s\n", lastline);
  printf("Add entry: ");
  fgets(text, SEG_SIZE, stdin);
  text[strlen(text)-1] = 0;
  write(file, text, strlen(text));
  strcpy(lastline, text);

  close(file);
  shmdt(lastline);
  semaphorebuffer.sem_op = 1;
  semop(semd, &semaphorebuffer, 1);

  return 0;
}
