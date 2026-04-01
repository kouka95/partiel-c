#ifndef TYPES_H
#define TYPES_H
#include "config.h"
#define MAX_OBSTACLES 10

typedef enum {
    ETAT_ATTENTE,
    ETAT_JEU,
    ETAT_GAME_OVER
} EtatJeu;

typedef enum {
    OBS_BAS,
    OBS_HAUT
} TypeObstacle;

typedef struct Araignee Araignee;
struct Araignee {
    float x, y;
    float vy;
    int au_sol;
    float largeur, hauteur;
};

typedef struct Obstacle Obstacle;
struct Obstacle {
    float x, y;
    float largeur, hauteur;
    TypeObstacle type;
    int actif;
};

typedef struct Jeu Jeu;
struct Jeu {
    Araignee araignee;
    EtatJeu etat;
    float score;
    int meilleur_score;
    float vitesse;
    float sol_y;
    int largeur_ecran;
    Obstacle obstacles[MAX_OBSTACLES];
    int timer_obstacle;
};
#endif