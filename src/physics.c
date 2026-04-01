#include "physics.h"
#include <stdlib.h>

static void spawner_obstacle(Jeu *jeu, const Config *cfg);
void Physics_step(Jeu *jeu, const Config *cfg) {
    int i;

    jeu->araignee.vy += cfg->gravite * cfg->dt;
    jeu->araignee.y += jeu->araignee.vy * cfg->dt;
    if (jeu->araignee.y >= jeu->sol_y) {
        jeu->araignee.y = jeu->sol_y;
        jeu->araignee.vy = 0.0f;
        jeu->araignee.au_sol = 1;
    }

    jeu->score += jeu->vitesse * cfg->dt;
    jeu->vitesse += cfg->acceleration * cfg->dt;
    if (jeu->vitesse > cfg->vitesse_max) {
        jeu->vitesse = cfg->vitesse_max;
    }

    for (i = 0; i < MAX_OBSTACLES; i++) {
        Obstacle *o = &jeu->obstacles[i];
        if (!o->actif) continue;
        o->x -= jeu->vitesse * cfg->dt;
        if (o->x + o->largeur < 0.0f) {
            o->actif = 0;
        }
    }

    jeu->timer_obstacle--;
    if (jeu->timer_obstacle <= 0) {
        spawner_obstacle(jeu, cfg);
        jeu->timer_obstacle = (int)(14000.0f / jeu->vitesse);
        if (jeu->timer_obstacle < cfg->intervalle_min) {
            jeu->timer_obstacle = cfg->intervalle_min;
        }
    }
}

void Physics_sauter(Jeu *jeu, const Config *cfg) {
    if (!jeu->araignee.au_sol) return;
    jeu->araignee.vy = -cfg->force_saut;
    jeu->araignee.au_sol = 0;
}

static void spawner_obstacle(Jeu *jeu, const Config *cfg) {
    int i;
    int type_haut = (jeu->score > cfg->score_oiseaux) && (rand() % 4 == 0);
    Obstacle *o;

    for (i = 0; i < MAX_OBSTACLES; i++) {
        if (jeu->obstacles[i].actif) continue;
        o = &jeu->obstacles[i];
        o->x = (float)jeu->largeur_ecran + 20.0f;

        if (type_haut) {
            o->largeur = 45.0f;
            o->hauteur = 22.0f;
            o->y = jeu->sol_y - 85.0f;
            o->type = OBS_HAUT;
        } else {
            o->largeur = 28.0f + (float)(rand() % 20);
            o->hauteur = 40.0f + (float)(rand() % 28);
            o->y = jeu->sol_y - o->hauteur;
            o->type = OBS_BAS;
        }
        o->actif = 1;
        return;
    }
}