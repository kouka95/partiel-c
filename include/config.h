#ifndef DEF_HEADER_CONFIG
#define DEF_HEADER_CONFIG

typedef struct Config Config;
struct Config {
    int fenetre_largeur;
    int fenetre_hauteur;
    float dt;
    float sol_y;
    float araignee_largeur, araignee_hauteur;
    float gravite, force_saut;
    float vitesse_depart, vitesse_max, acceleration;
    int intervalle_min;
    float score_oiseaux;
    float collision_marge;
};

int Config_charger(Config *cfg, const char *chemin);
void Config_afficher(const Config *cfg);

#endif