#ifndef UI_H
#define UI_H

#include "types.h"
#include "config.h"

void UI_init(const Config *cfg, const char *titre);
void UI_quitter(void);
int UI_running(void);
void UI_gerer_evenements(Jeu *jeu, const Config *cfg);
void UI_dessiner(const Jeu *jeu);

#endif