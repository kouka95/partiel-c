#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "types.h"
#include "config.h"
#include "physics.h"
#include "ui.h"

static int verifier_collision(const Jeu *jeu, const Config *cfg) {
    float m = cfg->collision_marge;
    float ax1 = jeu->araignee.x - jeu->araignee.largeur / 2.0f + m;
    float ax2 = jeu->araignee.x + jeu->araignee.largeur / 2.0f - m;
    float ay1 = jeu->araignee.y - jeu->araignee.hauteur + m;
    float ay2 = jeu->araignee.y - m;
    int i;

    for (i = 0; i < MAX_OBSTACLES; i++) {
        const Obstacle *o = &jeu->obstacles[i];
        if (!o->actif) continue;
        if (ax2 > o->x && ax1 < o->x + o->largeur && ay2 > o->y && ay1 < o->y + o->hauteur) return 1;
    }
    return 0;
}

static Jeu Jeu_creer(const Config *cfg) {
    Jeu jeu;
    int i;

    jeu.etat = ETAT_ATTENTE;
    jeu.score = 0.0f;
    jeu.meilleur_score = 0;
    jeu.vitesse = cfg->vitesse_depart;
    jeu.sol_y = cfg->sol_y;
    jeu.largeur_ecran = cfg->fenetre_largeur;
    jeu.timer_obstacle = 90;

    for (i = 0; i < MAX_OBSTACLES; i++) {
        jeu.obstacles[i].actif = 0;
    }

    jeu.araignee.x = 130.0f;
    jeu.araignee.y = cfg->sol_y;
    jeu.araignee.vy = 0.0f;
    jeu.araignee.au_sol = 1;
    jeu.araignee.largeur = cfg->araignee_largeur;
    jeu.araignee.hauteur = cfg->araignee_hauteur;
    return jeu;
}

int main(int argc, char *argv[]) {
    Config cfg;
    Jeu jeu;
    (void)argc; (void)argv;

    srand((unsigned int)time(NULL));
    Config_charger(&cfg, "data/config.txt");
    Config_afficher(&cfg);

    jeu = Jeu_creer(&cfg);
    UI_init(&cfg, "Jeu de Survie - Araignee");

    while (UI_running()) {
        UI_gerer_evenements(&jeu, &cfg);

        if (jeu.etat == ETAT_JEU) {
            Physics_step(&jeu, &cfg);

            if (verifier_collision(&jeu, &cfg)) {
                if ((int)jeu.score > jeu.meilleur_score){
                    jeu.meilleur_score = (int)jeu.score;
                }
                jeu.etat = ETAT_GAME_OVER;
                printf("[GAME OVER] Score : %d\n", (int)jeu.score);
            }
        }

        UI_dessiner(&jeu);
    }

    UI_quitter();
    printf("Meilleur score : %d\n", jeu.meilleur_score);
    return 0;
}