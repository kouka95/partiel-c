#include "ui.h"
#include "physics.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#define FPS 60

static SDL_Surface *ecran = NULL;
static int actif = 0;
static void dessin_disque(int cx, int cy, int r, Uint8 rouge, Uint8 vert, Uint8 bleu) {
    filledCircleRGBA(ecran, cx, cy, r, rouge, vert, bleu, 255);
}

static void dessin_texte(const char *txt, int x, int y, Uint8 r, Uint8 g, Uint8 b) {
    if (!txt || !txt[0]) return;
    stringRGBA(ecran, x, y, txt, r, g, b, 255);
}

static void dessin_score(int x, int y, const char *label, int val, Uint8 r, Uint8 g, Uint8 b) {
    char buf[64];
    sprintf(buf, "%s%05d", label, val);
    dessin_texte(buf, x, y, r, g, b);
}

static void dessiner_araignee(const Jeu *jeu) {
    int cx = (int)jeu->araignee.x;
    int pied = (int)jeu->araignee.y;
    int cy = pied - 22;
    int i;

    dessin_disque(cx, cy, 14, 50, 50, 50);
    dessin_disque(cx - 16, cy + 4, 10, 40, 40, 40);
    dessin_disque(cx + 16, cy - 2, 9, 60, 60, 60);
    pixelRGBA(ecran, cx + 20, cy - 5, 220, 220, 50, 255);
    pixelRGBA(ecran, cx + 21, cy - 5, 220, 220, 50, 255);
    pixelRGBA(ecran, cx + 20, cy - 4, 220, 220, 50, 255);

    for (i = 0; i < 4; i++) {
        int ox = cx - 10 + i * 6;
        int oy = cy - 2;
        int mx_g = ox - 18 - i * 4;
        int mx_d = ox + 18 + i * 4;
        int ex_g = mx_g - 10;
        int ex_d = mx_d + 10;
        int mid_y;
        if (jeu->araignee.au_sol) {
            mid_y = oy - 8 + i * 3;
            lineRGBA(ecran, ox, oy, mx_g, mid_y, 35, 35, 35, 255);
            lineRGBA(ecran, mx_g, mid_y, ex_g, pied, 35, 35, 35, 255);
            lineRGBA(ecran, ox, oy, mx_d, mid_y, 35, 35, 35, 255);
            lineRGBA(ecran, mx_d, mid_y, ex_d, pied, 35, 35, 35, 255);
        } else {
            mid_y = oy - 14 + i * 2;
            lineRGBA(ecran, ox, oy, mx_g, mid_y, 35, 35, 35, 255);
            lineRGBA(ecran, mx_g, mid_y, ex_g, mid_y + 8, 35, 35, 35, 255);
            lineRGBA(ecran, ox, oy, mx_d, mid_y, 35, 35, 35, 255);
            lineRGBA(ecran, mx_d, mid_y, ex_d, mid_y + 8, 35, 35, 35, 255);
        }
    }
}

static void dessiner_obstacles(const Jeu *jeu) {
    int i;
    for (i = 0; i < MAX_OBSTACLES; i++) {
        const Obstacle *o = &jeu->obstacles[i];
        if (!o->actif) continue;

        switch (o->type) {
            case OBS_BAS: {
                boxRGBA(ecran, (int)o->x, (int)o->y,
                        (int)(o->x + o->largeur), (int)(o->y + o->hauteur),
                        45, 120, 45, 255);
                rectangleRGBA(ecran, (int)o->x, (int)o->y,
                              (int)(o->x + o->largeur), (int)(o->y + o->hauteur),
                              70, 170, 70, 255);
            } break;
            case OBS_HAUT: {
                int bx = (int)(o->x + o->largeur / 2);
                int by = (int)(o->y + o->hauteur / 2);
                int w = (int)(o->largeur / 2);
                filledCircleRGBA(ecran, bx, by, 6, 170, 130, 50, 255);
                lineRGBA(ecran, bx - 6, by, bx - w, by - 10, 200, 160, 60, 255);
                lineRGBA(ecran, bx - 6, by, bx - w + 4, by + 4, 200, 160, 60, 255);
                lineRGBA(ecran, bx + 6, by, bx + w, by - 10, 200, 160, 60, 255);
                lineRGBA(ecran, bx + 6, by, bx + w - 4, by + 4, 200, 160, 60, 255);
            } break;
            default: break;
        }
    }
}

static void dessiner_sol(const Jeu *jeu, int w) {
    int x, offset;
    hlineRGBA(ecran, 0, w, (int)jeu->sol_y + 1, 110, 110, 110, 255);
    offset = (int)(jeu->score) % 55;
    for (x = -offset; x < w; x += 55) {
        hlineRGBA(ecran, x, x + 22, (int)jeu->sol_y + 5, 65, 65, 65, 255);
    }
}

static void dessiner_ecran_attente(const Jeu *jeu, int w, int h) {
    dessiner_araignee(jeu);
    dessin_texte("JEU DE SURVIE - ARAIGNEE", w/2 - 90, h/2 - 70, 220, 220, 220);
    dessin_texte("Appuie sur ESPACE pour commencer", w/2 - 120, h/2 - 20, 255, 215, 0);
    dessin_texte("Saute par-dessus les obstacles !", w/2 - 115, h/2 + 20, 120, 120, 120);
}

static void dessiner_game_over(const Jeu *jeu, int w, int h) {
    boxRGBA(ecran, 0, 0, w, h, 0, 0, 0, 160);
    dessin_texte("GAME OVER", w/2 - 35, h/2 - 55, 210, 50, 50);
    dessin_score(w/2 - 45, h/2 - 5, "Score : ", (int)jeu->score, 220, 220, 220);
    if ((int)jeu->score > 0 && (int)jeu->score >= jeu->meilleur_score) {
        dessin_texte("-- NOUVEAU RECORD --", w/2 - 75, h/2 + 30, 255, 215, 0);
    }
    dessin_texte("ESPACE pour rejouer", w/2 - 70, h/2 + 65, 120, 120, 120);
}

void UI_init(const Config *cfg, const char *titre) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "[UI] SDL_Init : %s\n", SDL_GetError());
        return;
    }
    ecran = SDL_SetVideoMode(cfg->fenetre_largeur, cfg->fenetre_hauteur, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (!ecran) {
        fprintf(stderr, "[UI] SDL_SetVideoMode : %s\n", SDL_GetError());
        SDL_Quit();
        return;
    }
    SDL_WM_SetCaption(titre, NULL);
    actif = 1;
    printf("[UI] %dx%d | ESPACE=sauter ECHAP=quitter\n",cfg->fenetre_largeur, cfg->fenetre_hauteur);
}

void UI_quitter(void) {
    if (ecran) {
        SDL_FreeSurface(ecran);
        ecran = NULL;
    }
    SDL_Quit();
    actif = 0;
}

int UI_running(void) { return actif; }

static void reinit_partie(Jeu *jeu, const Config *cfg) {
    int i;
    for (i = 0; i < MAX_OBSTACLES; i++) {
        jeu->obstacles[i].actif = 0;
    }
    jeu->araignee.y = jeu->sol_y;
    jeu->araignee.vy = 0.0f;
    jeu->araignee.au_sol = 1;
    jeu->score = 0.0f;
    jeu->vitesse = cfg->vitesse_depart;
    jeu->timer_obstacle = 90;
    jeu->etat = ETAT_JEU;
}

void UI_gerer_evenements(Jeu *jeu, const Config *cfg) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: {
                actif = 0;
            } break;
            case SDL_KEYDOWN: {
                switch (event.key.keysym.sym) {
                    case SDLK_SPACE:
                    case SDLK_UP: {
                        if (jeu->etat == ETAT_JEU) {
                            Physics_sauter(jeu, cfg);
                        } else {
                            if ((int)jeu->score > jeu->meilleur_score) {
                                jeu->meilleur_score = (int)jeu->score;
                            }
                            reinit_partie(jeu, cfg);
                        }
                    } break;
                    case SDLK_ESCAPE: {
                        actif = 0;
                    } break;
                    default: break;
                }
            } break;
            default: break;
        }
    }
}

void UI_dessiner(const Jeu *jeu) {
    int w, h;
    if (!ecran) return;
    w = ecran->w;
    h = ecran->h;

    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 22, 22, 22));
    dessiner_sol(jeu, w);
    
    switch (jeu->etat) {
        case ETAT_ATTENTE: {
            dessiner_ecran_attente(jeu, w, h);
        } break;
        case ETAT_JEU: {
            dessiner_obstacles(jeu);
            dessiner_araignee(jeu);
            dessin_score(w - 130, 14, "Score:", (int)jeu->score, 220, 220, 220);
            if (jeu->meilleur_score > 0) {
                dessin_score(w - 260, 14, "HI:", jeu->meilleur_score, 120, 120, 120);
            }
        } break;
        case ETAT_GAME_OVER: {
            dessiner_obstacles(jeu);
            dessiner_araignee(jeu);
            dessin_score(w - 130, 14, "Score:", (int)jeu->score, 220, 220, 220);
            if (jeu->meilleur_score > 0) {
                dessin_score(w - 260, 14, "HI:", jeu->meilleur_score, 120, 120, 120);
            }
            dessiner_game_over(jeu, w, h);
        } break;
        default: break;
    }

    SDL_Flip(ecran);
}