#ifndef CONFIG_H
#define CONFIG_H

typedef struct Config Config;
struct Config {
    /* Fenetre */
    int fenetre_largeur;
    int fenetre_hauteur;

    /* Temps */
    float dt;

    /* Joueur */
    float sol_y;
    float araignee_largeur;
    float araignee_hauteur;

    /* Physique */
    float gravite;
    float force_saut;
    float vitesse_depart;
    float vitesse_max;
    float acceleration;

    /* Obstacles */
    int intervalle_min;
    float score_oiseaux;

    /* Collision */
    float collision_marge;
};

int Config_charger(Config *cfg, const char *chemin);
void Config_afficher(const Config *cfg);

#endif