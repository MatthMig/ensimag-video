#include "ensitheora.hpp"
#include "synchro.hpp"

mutex allstrm;
mutex mutextex;
condition_variable texprod;
condition_variable texconso;

extern bool fini;
int tex_nbready;
bool windowsdone;

void envoiTailleFenetre(th_ycbcr_buffer buffer) {
    lock_guard<mutex> lock(allstrm);

    windowsx = buffer[0].width;
    windowsy = buffer[0].height;

    fini = true;

    texprod.notify_one();
}

void attendreTailleFenetre() {
    unique_lock<mutex> lock(allstrm);
    texprod.wait(lock, []{return fini;});
    fini = false;
}

void signalerFenetreEtTexturePrete() {
    lock_guard<mutex> lock(mutextex);
    windowsdone = true;
    texconso.notify_one();
}

void attendreFenetreTexture() {
    unique_lock<mutex> lock(mutextex);
    texconso.wait(lock, []{return windowsdone;});
    windowsdone = false;
}

void debutConsommerTexture() {
    unique_lock<mutex> lock(mutextex);
    texconso.wait(lock, []{return tex_nbready > 0;});
}

void finConsommerTexture() {
    lock_guard<mutex> lock(mutextex);
    tex_nbready--;
    texconso.notify_one();
}

void debutDeposerTexture() {
    unique_lock<mutex> lock(mutextex);
    texconso.wait(lock, []{return tex_nbready < NBTEX;});
}

void finDeposerTexture() {
    lock_guard<mutex> lock(mutextex);
    tex_nbready++;
    texconso.notify_one();
}