#include "game.h"
#include "types.h"
#include "config.h"
#include "physics.h"
#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL/SDL.h>
#define FPS 60

static Config cfg;
static Jeu jeu;
static int debut_frame;
static Jeu creer_jeu(void) {
    Jeu j;
    int i;
    j.etat = ETAT_ATTENTE;
    j.score = 0.0f;
    j.meilleur_score = 0;
    j.vitesse = cfg.vitesse_depart;
    j.sol_y = cfg.sol_y;
    j.largeur_ecran = cfg.fenetre_largeur;
    j.timer_obstacle = 90;
    j.araignee.x = 130.0f;
    j.araignee.y = cfg.sol_y;
    j.araignee.vy = 0.0f;
    j.araignee.au_sol = 1;
    j.araignee.largeur = cfg.araignee_largeur;
    j.araignee.hauteur = cfg.araignee_hauteur;
    for (i = 0; i < MAX_OBSTACLES; i++)
        j.obstacles[i].actif = 0;
    return j;
}

void Game_init(void) {
    srand((unsigned int)time(NULL));
    Config_charger(&cfg, "config.txt");
    Config_afficher(&cfg);
    jeu = creer_jeu();
    UI_init(&cfg, "Jeu de Survie");
}

int Game_running(void) {
    return UI_running();
}

static int verifier_collision(void) {
    float m = cfg.collision_marge;
    float ax1 = jeu.araignee.x - jeu.araignee.largeur / 2.0f + m;
    float ax2 = jeu.araignee.x + jeu.araignee.largeur / 2.0f - m;
    float ay1 = jeu.araignee.y - jeu.araignee.hauteur + m;
    float ay2 = jeu.araignee.y - m;
    int i;
    for (i = 0; i < MAX_OBSTACLES; i++) {
        const Obstacle *o = &jeu.obstacles[i];
        if (!o->actif) continue;
        if (ax2 > o->x && ax1 < o->x + o->largeur &&
            ay2 > o->y && ay1 < o->y + o->hauteur)
            return 1;
    }
    return 0;
}

void Game_update(void) {
    debut_frame = (int)SDL_GetTicks();
    UI_gerer_evenements(&jeu, &cfg);

    if (jeu.etat != ETAT_JEU) return;

    Physics_step(&jeu, &cfg);

    if (verifier_collision()) {
        if ((int)jeu.score > jeu.meilleur_score)
            jeu.meilleur_score = (int)jeu.score;
        jeu.etat = ETAT_GAME_OVER;
        printf("[GAME OVER] Score : %d\n", (int)jeu.score);
    }
}

void Game_draw(void) {
    UI_dessiner(&jeu);
}

void Game_wait(void) {
    int elapsed = (int)SDL_GetTicks() - debut_frame;
    int restant = (1000 / FPS) - elapsed;
    if (restant > 0) SDL_Delay((Uint32)restant);
}

void Game_quit(void) {
    UI_quitter();
    printf("Meilleur score : %d\n", jeu.meilleur_score);
}