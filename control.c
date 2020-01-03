#include "keys.h"

void print_story() {
  FILE *file;
  char text[SEG_SIZE];

  text[0] = 0;

  file = fopen("text.txt", "r");
  printf("The story so far...\n");

  while (fgets(text, SEG_SIZE, file)) {
    printf("%s\n", text);
  }

  fclose(file);
}

int main(int argc, char *argv[]) {
  if (argc == 2) {
    //printf("%s\n", argv[1]);
    if (!strcmp(argv[1], "-c")) {

      union semun semaphore;
      int shmd, semd;

      semd = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | 0644);
      shmd = shmget(SHR_KEY, SEG_SIZE, IPC_CREAT | 0644);
      //printf("shmd %d\n", shmd);
      if (semd < 0) {
        printf("Semaphore error: %s\n", strerror(errno));
        semd = semget(SEM_KEY, 1, 0);
        semctl(semd, 0, GETVAL, 0);
        return errno;
      }
      else {
        printf("semaphore created\n");
        semaphore.val = 1;
        semctl(semd, 0, SETVAL, semaphore);
      }

      if (shmd < 0) {
        printf("shared memory error: %s\n", strerror(errno));
        return errno;
      }
      else {
        printf("shared memory created\n");
      }

      int file = open("text.txt", O_CREAT | O_TRUNC | O_RDWR);

      if (file < 0) {
        printf("file error: %s\n", strerror(errno));
      }
      else {
        printf("file successfully created\n");
      }
      return 0;
    }

    else if (!strcmp(argv[1], "-r")) {
      int shmd, semd;

      semd = semget(SEM_KEY, 1, 0644);
      shmd = shmget(SHR_KEY, 0, 0644);
      struct sembuf semaphorebuffer;
      if (semd < 0) {
        printf("semaphore error: %s\n", strerror(errno));
        return errno;
      }
      else {
        printf("Trying to get in\n");
        semaphorebuffer.sem_num = 0;
        semaphorebuffer.sem_op = -1;
        semop(semd, &semaphorebuffer, 1);
        printf("got the semaphore\n");
      }

      print_story();

      semctl(semd, IPC_RMID, 0);
      printf("semaphore removed\n");

      int check = shmctl(shmd, IPC_RMID, 0);
      if (check < 0) {
        printf("removing shared memory error: %s\n", strerror(errno));
      }
      printf("shared memory removed\n");

      remove("text.txt");
      printf("file removed\n");

    }
    else if (!strcmp(argv[1], "-v")) {
      print_story();
    }
  }
  return 0;
}
