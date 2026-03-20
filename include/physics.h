#ifndef PHYSICS_H
#define PHYSICS_H

#include "types.h"
#include "config.h"

void Physics_step(Jeu *jeu, const Config *cfg);
void Physics_sauter(Jeu *jeu, const Config *cfg);

#endif