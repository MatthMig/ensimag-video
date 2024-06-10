#include <SDL2/SDL.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

#include "oggstream.h"
#include "stream_common.h"

int main(int argc, char *argv[]) {
  int res;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s FILE", argv[0]);
    exit(EXIT_FAILURE);
  }
  assert(argc == 2);

  // Initialisation de la SDL
  res = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS);
  atexit(SDL_Quit);
  assert(res == 0);

  // Your code HERE
  // start the two stream readers (theoraStreamReader and vorbisStreamReader)
  // each in a thread

  pthread_t theora_thread;
  pthread_t vorbis_thread;

  if (pthread_create(&theora_thread, NULL, theoraStreamReader, argv[1]) != 0) {
    fprintf(stderr, "Error: pthread_create theora failed\n");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&vorbis_thread, NULL, vorbisStreamReader, argv[1]) != 0) {
    fprintf(stderr, "Error: pthread_create vorbis failed\n");
    exit(EXIT_FAILURE);
  }
  
  // wait for vorbis thread

  if(pthread_join(vorbis_thread, NULL) != 0) {
    fprintf(stderr, "Error: pthread_join vorbis failed\n");
    exit(EXIT_FAILURE);
  }

  // 1 seconde of sound in advance, thus wait 1 seconde
  // before leaving
  sleep(1);

  // Wait for theora and theora2sdl threads

  if(pthread_join(theora_thread, NULL) != 0) {
    fprintf(stderr, "Error: pthread_join theora failed\n");
    exit(EXIT_FAILURE);
  }

  // TODO
  /* liberer des choses ? */

  exit(EXIT_SUCCESS);
}
