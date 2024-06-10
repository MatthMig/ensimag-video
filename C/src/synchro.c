#include "ensitheora.h"
#include "synchro.h"

/* les variables pour la synchro, ici */
pthread_mutex_t mutex_taille_fenetre = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_fenetre_texture = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_taille_fenetre = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_fenetre_texture = PTHREAD_COND_INITIALIZER;

bool taille_fenetre_envoyee = false;
bool fenetre_texture_pret = false;

int nb_textures_deposees = 0;

/* l'implantation des fonctions de synchro ici */
void envoiTailleFenetre(th_ycbcr_buffer buffer) {
    pthread_mutex_lock(&mutex_taille_fenetre);

    windowsx = buffer[0].width;
    windowsy = buffer[0].height;

    taille_fenetre_envoyee = true;

    pthread_cond_signal(&cond_taille_fenetre);
    pthread_mutex_unlock(&mutex_taille_fenetre);
}

void attendreTailleFenetre() {
    pthread_mutex_lock(&mutex_taille_fenetre);
    while (!taille_fenetre_envoyee) {
        pthread_cond_wait(&cond_taille_fenetre, &mutex_taille_fenetre);
    }
    taille_fenetre_envoyee = false;
    pthread_mutex_unlock(&mutex_taille_fenetre);
}

void signalerFenetreEtTexturePrete() {
    pthread_mutex_lock(&mutex_fenetre_texture);
    fenetre_texture_pret = true;
    pthread_cond_signal(&cond_fenetre_texture);
    pthread_mutex_unlock(&mutex_fenetre_texture);
}

void attendreFenetreTexture() {
    pthread_mutex_lock(&mutex_fenetre_texture);
    while (!fenetre_texture_pret) {
        pthread_cond_wait(&cond_fenetre_texture, &mutex_fenetre_texture);
    }
    fenetre_texture_pret = false;
    pthread_mutex_unlock(&mutex_fenetre_texture);
}

void debutConsommerTexture() {
    pthread_mutex_lock(&mutex_fenetre_texture);
    while(nb_textures_deposees == 0) {
        pthread_cond_wait(&cond_fenetre_texture, &mutex_fenetre_texture);
    }
    pthread_mutex_unlock(&mutex_fenetre_texture);
}

void finConsommerTexture() {
    pthread_mutex_lock(&mutex_fenetre_texture);
    nb_textures_deposees--;
    pthread_cond_signal(&cond_fenetre_texture);
    pthread_mutex_unlock(&mutex_fenetre_texture);
}

void debutDeposerTexture() {
    pthread_mutex_lock(&mutex_fenetre_texture);
    while (nb_textures_deposees >= NBTEX) {
        pthread_cond_wait(&cond_fenetre_texture, &mutex_fenetre_texture);
    }
    pthread_mutex_unlock(&mutex_fenetre_texture);
}

void finDeposerTexture() {
    pthread_mutex_lock(&mutex_fenetre_texture);
    nb_textures_deposees++;
    pthread_cond_signal(&cond_fenetre_texture);
    pthread_mutex_unlock(&mutex_fenetre_texture);
}
