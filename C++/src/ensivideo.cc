#include <SDL2/SDL.h>
#include <cassert>
#include <string>
#include <thread>
#include <pthread.h>

#include "ensivideo.hpp"
#include "oggstream.hpp"
#include "stream_common.hpp"

using namespace std;

unique_ptr<thread> theorathread;
unique_ptr<thread> vorbisthread;

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

  if (pthread_create(&theora_thread, NULL, [](void* arg) -> void* {
    char* charPtr = static_cast<char*>(arg);
    std::string filename =  std::string(charPtr);
    theoraStreamReader(filename);
    return nullptr;
  }, static_cast<void*>(argv[1])) != 0) {
    fprintf(stderr, "Error: pthread_create theoraStreamReader failed\n");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&vorbis_thread, NULL, [](void* arg) -> void* {
    char* charPtr = static_cast<char*>(arg);
    std::string filename =  std::string(charPtr);
    vorbisStreamReader(filename);
    return nullptr;
  }, static_cast<void*>(argv[1])) != 0) {
    fprintf(stderr, "Error: pthread_create vorbisStreamReader failed\n");
    exit(EXIT_FAILURE);
  }

  // wait for vorbis thread

  if (pthread_join(vorbis_thread, NULL) != 0) {
    fprintf(stderr, "Error: pthread_join vorbisStreamReader failed\n");
    exit(EXIT_FAILURE);
  }

  // 1 seconde of sound in advance, thus wait 1 seconde
  // before leaving
  this_thread::sleep_for(1s);

  // Wait for theora and theora2sdl threads

  if (pthread_join(theora_thread, NULL) != 0) {
    fprintf(stderr, "Error: pthread_join theoraStreamReader failed\n");
    exit(EXIT_FAILURE);
  }

  // TODO
  /* liberer des choses ? */

  exit(EXIT_SUCCESS);
}
