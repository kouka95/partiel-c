#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_LIGNE 256

static void Config_defaut(Config *cfg);
static void trim(char *s);
static void appliquer(Config *cfg, const char *cle, const char *val);

int Config_charger(Config *cfg, const char *chemin) {
    FILE *f;
    char ligne[MAX_LIGNE];
    char *egal;
    Config_defaut(cfg);

    f = fopen(chemin, "r");
    if (!f) {
        fprintf(stderr, "[Config] '%s' introuvable, valeurs par defaut.\n", chemin);
        return -1;
    }

    while (fgets(ligne, sizeof(ligne), f)) {
        trim(ligne);
        if (!ligne[0] || ligne[0] == '#') continue;

        egal = strchr(ligne, '=');
        if (!egal) continue;

        *egal = '\0';
        trim(ligne);
        trim(egal + 1);
        if (*(egal + 1)) {
            appliquer(cfg, ligne, egal + 1);
        }
    }

    fclose(f);
    return 0;
}

void Config_afficher(const Config *cfg) {
    printf("=== CONFIG ===\n");
    printf(" Fenetre  : %dx%d dt=%.3f\n", cfg->fenetre_largeur, cfg->fenetre_hauteur, cfg->dt);
    printf(" Physique : gravite=%.0f saut=%.0f marge=%.0f\n", cfg->gravite, cfg->force_saut, cfg->collision_marge);
    printf(" Vitesse  : depart=%.0f max=%.0f accel=%.0f\n", cfg->vitesse_depart, cfg->vitesse_max, cfg->acceleration);
    printf(" Obstacles: intervalle_min=%d oiseaux apres %.0f pts\n", cfg->intervalle_min, cfg->score_oiseaux);
    printf("==============\n");
}

static void Config_defaut(Config *cfg) {
    cfg->fenetre_largeur = 900;
    cfg->fenetre_hauteur = 420;
    cfg->dt = 0.016f;
    cfg->sol_y = 330.0f;
    cfg->araignee_largeur = 50.0f;
    cfg->araignee_hauteur = 45.0f;
    cfg->gravite = 1400.0f;
    cfg->force_saut = 580.0f;
    cfg->vitesse_depart = 220.0f;
    cfg->vitesse_max = 700.0f;
    cfg->acceleration = 12.0f;
    cfg->intervalle_min = 35;
    cfg->score_oiseaux = 1000.0f;
    cfg->collision_marge = 5.0f;
}

static void trim(char *s) {
    int d = 0, f;
    while (s[d] == ' ' || s[d] == '\t') {
        d++;
    }
    if (d > 0) {
        memmove(s, s + d, strlen(s) - d + 1);
    }
    f = (int)strlen(s) - 1;
    while (f >= 0 && (s[f] == ' ' || s[f] == '\t' || s[f] == '\n' || s[f] == '\r')) {
        s[f--] = '\0';
    }
}

static void appliquer(Config *cfg, const char *k, const char *v) {
    if (!strcmp(k, "fenetre_largeur")) cfg->fenetre_largeur = atoi(v);
    else if (!strcmp(k, "fenetre_hauteur")) cfg->fenetre_hauteur = atoi(v);
    else if (!strcmp(k, "dt")) cfg->dt = (float)atof(v);
    else if (!strcmp(k, "sol_y")) cfg->sol_y = (float)atof(v);
    else if (!strcmp(k, "araignee_largeur")) cfg->araignee_largeur = (float)atof(v);
    else if (!strcmp(k, "araignee_hauteur")) cfg->araignee_hauteur = (float)atof(v);
    else if (!strcmp(k, "gravite")) cfg->gravite = (float)atof(v);
    else if (!strcmp(k, "force_saut")) cfg->force_saut = (float)atof(v);
    else if (!strcmp(k, "vitesse_depart")) cfg->vitesse_depart = (float)atof(v);
    else if (!strcmp(k, "vitesse_max")) cfg->vitesse_max = (float)atof(v);
    else if (!strcmp(k, "acceleration")) cfg->acceleration = (float)atof(v);
    else if (!strcmp(k, "intervalle_min")) cfg->intervalle_min = atoi(v);
    else if (!strcmp(k, "score_oiseaux")) cfg->score_oiseaux = (float)atof(v);
    else if (!strcmp(k, "collision_marge")) cfg->collision_marge = (float)atof(v);
    else fprintf(stderr, "[Config] Cle inconnue : '%s'\n", k);
}