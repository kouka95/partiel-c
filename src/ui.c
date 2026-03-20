#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include "ui.h"
#include "physics.h"

/* ================================================================
   ui.c -  Affichage et evenements
   ================================================================
   ESPACE / Fleche haut -> sauter / demarrer / rejouer
   ECHAP -> quitter
   ================================================================ */

static SDL_Window *fenetre = NULL;
static SDL_Renderer *rendu = NULL;
static TTF_Font *police = NULL;
static int actif = 0;


/* ================================================================
   Utilitaires internes
   ================================================================ */

static void dessiner_disque(int cx, int cy, int r) {
    int dy;
    for (dy = -r; dy <= r; dy++) {
        int dx = (int)SDL_sqrt((double)(r * r - dy * dy));
        SDL_RenderDrawLine(rendu, cx - dx, cy + dy, cx + dx, cy + dy);
    }
}

static void afficher_texte(const char *txt, int x, int y,
                            SDL_Color col) {
    SDL_Surface *s;
    SDL_Texture *t;
    if (!police || !txt || !txt[0]) return;
    s = TTF_RenderText_Blended(police, txt, col);
    if (!s) return;
    t = SDL_CreateTextureFromSurface(rendu, s);
    if (t) {
        SDL_Rect d = {x, y, s->w, s->h};
        SDL_RenderCopy(rendu, t, NULL, &d);
        SDL_DestroyTexture(t);
    }
    SDL_FreeSurface(s);
}

static void afficher_fmt(int x, int y, SDL_Color col,
                          const char *fmt, ...) {
    char buf[128];
    va_list args;
    va_start(args, fmt);
    SDL_vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    afficher_texte(buf, x, y, col);
}


/* ================================================================
   INIT / QUITTER
   ================================================================ */

void UI_init(const Config *cfg, const char *titre) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "[UI] SDL_Init : %s\n", SDL_GetError());
        return;
    }
    if (TTF_Init() != 0) {
        fprintf(stderr, "[UI] TTF_Init : %s\n", TTF_GetError());
        SDL_Quit(); return;
    }

    fenetre = SDL_CreateWindow(titre,
                SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                cfg->fenetre_largeur, cfg->fenetre_hauteur,
                SDL_WINDOW_SHOWN);
    if (!fenetre) { TTF_Quit(); SDL_Quit(); return; }

    rendu = SDL_CreateRenderer(fenetre, -1,
                SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!rendu)
        rendu = SDL_CreateRenderer(fenetre, -1, SDL_RENDERER_SOFTWARE);
    if (!rendu) {
        SDL_DestroyWindow(fenetre); TTF_Quit(); SDL_Quit(); return;
    }

    police = TTF_OpenFont("C:/Windows/Fonts/arial.ttf", 17);
    if (!police)
        police = TTF_OpenFont("C:/Windows/Fonts/consola.ttf", 17);
    if (!police)
        police = TTF_OpenFont(
            "C:/msys64/ucrt64/share/fonts/TTF/DejaVuSans.ttf", 17);
    if (!police)
        fprintf(stderr, "[UI] Police introuvable\n");

    actif = 1;
    printf("[UI] %dx%d | ESPACE=sauter ECHAP=quitter\n",
           cfg->fenetre_largeur, cfg->fenetre_hauteur);
}

void UI_quitter(void) {
    if (police)  { TTF_CloseFont(police);      police = NULL; }
    if (rendu)   { SDL_DestroyRenderer(rendu); rendu = NULL; }
    if (fenetre) { SDL_DestroyWindow(fenetre); fenetre = NULL; }
    TTF_Quit(); SDL_Quit(); actif = 0;
}

int UI_running(void) { return actif; }


/* ================================================================
   EVENEMENTS
   ================================================================ */

/* reinit_partie : utilise la config pour les valeurs de depart */
static void reinit_partie(Jeu *jeu, const Config *cfg) {
    int i;
    for (i = 0; i < MAX_OBSTACLES; i++)
        jeu->obstacles[i].actif = 0;

    jeu->araignee.y = jeu->sol_y;
    jeu->araignee.vy = 0.0f;
    jeu->araignee.au_sol = 1;
    jeu->score = 0.0f;
    jeu->vitesse         = cfg->vitesse_depart;  /* lu depuis config.txt */
    jeu->timer_obstacle = 90;
    jeu->etat = ETAT_JEU;
}

void UI_gerer_evenements(Jeu *jeu, const Config *cfg) {
    SDL_Event ev;

    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) { actif = 0; }

        else if (ev.type == SDL_KEYDOWN) {
            switch (ev.key.keysym.sym) {

                case SDLK_SPACE:
                case SDLK_UP:
                    if (jeu->etat == ETAT_JEU) {
                        Physics_sauter(jeu, cfg);
                    } else {
                        if ((int)jeu->score > jeu->meilleur_score)
                            jeu->meilleur_score = (int)jeu->score;
                        reinit_partie(jeu, cfg);
                    }
                    break;

                case SDLK_ESCAPE:
                    actif = 0;
                    break;

                default: break;
            }
        }
    }
}


/* ================================================================
   DESSIN
   ================================================================ */

static void dessiner_araignee(const Jeu *jeu) {
    int cx = (int)jeu->araignee.x;
    int pied = (int)jeu->araignee.y;
    int cy = pied - 22;
    int i;

    /* Corps */
    SDL_SetRenderDrawColor(rendu, 50, 50, 50, 255);
    dessiner_disque(cx, cy, 14);

    /* Abdomen */
    SDL_SetRenderDrawColor(rendu, 40, 40, 40, 255);
    dessiner_disque(cx - 16, cy + 4, 10);

    /* Tete */
    SDL_SetRenderDrawColor(rendu, 60, 60, 60, 255);
    dessiner_disque(cx + 16, cy - 2, 9);

    /* Oeil */
    SDL_SetRenderDrawColor(rendu, 220, 220, 50, 255);
    SDL_RenderDrawPoint(rendu, cx + 20, cy - 5);
    SDL_RenderDrawPoint(rendu, cx + 21, cy - 5);
    SDL_RenderDrawPoint(rendu, cx + 20, cy - 4);

    /* 8 pattes */
    SDL_SetRenderDrawColor(rendu, 35, 35, 35, 255);
    for (i = 0; i < 4; i++) {
        int ox = cx - 10 + i * 6;
        int oy = cy - 2;
        int mid_x_g = ox - 18 - i * 4;
        int mid_x_d = ox + 18 + i * 4;
        int end_x_g = mid_x_g - 10;
        int end_x_d = mid_x_d + 10;
        int mid_y;

        if (jeu->araignee.au_sol) {
            mid_y = oy - 8 + i * 3;
            SDL_RenderDrawLine(rendu, ox,      oy,    mid_x_g, mid_y);
            SDL_RenderDrawLine(rendu, mid_x_g, mid_y, end_x_g, pied);
            SDL_RenderDrawLine(rendu, ox,      oy,    mid_x_d, mid_y);
            SDL_RenderDrawLine(rendu, mid_x_d, mid_y, end_x_d, pied);
        } else {
            mid_y = oy - 14 + i * 2;
            SDL_RenderDrawLine(rendu, ox,      oy,    mid_x_g, mid_y);
            SDL_RenderDrawLine(rendu, mid_x_g, mid_y, end_x_g, mid_y+8);
            SDL_RenderDrawLine(rendu, ox,      oy,    mid_x_d, mid_y);
            SDL_RenderDrawLine(rendu, mid_x_d, mid_y, end_x_d, mid_y+8);
        }
    }
}

static void dessiner_obstacles(const Jeu *jeu) {
    int i;
    for (i = 0; i < MAX_OBSTACLES; i++) {
        const Obstacle *o = &jeu->obstacles[i];
        if (!o->actif) continue;

        if (o->type == OBS_BAS) {
            SDL_Rect r = {(int)o->x, (int)o->y,
                          (int)o->largeur, (int)o->hauteur};
            SDL_SetRenderDrawColor(rendu, 45, 120, 45, 255);
            SDL_RenderFillRect(rendu, &r);
            SDL_SetRenderDrawColor(rendu, 70, 170, 70, 255);
            SDL_RenderDrawRect(rendu, &r);
            SDL_RenderDrawLine(rendu, (int)o->x-8, (int)o->y+12,
                               (int)o->x,           (int)o->y+10);
            SDL_RenderDrawLine(rendu, (int)o->x-8, (int)o->y+12,
                               (int)o->x-8,         (int)o->y+6);
            SDL_RenderDrawLine(rendu,
                (int)(o->x+o->largeur)+8, (int)o->y+12,
                (int)(o->x+o->largeur),   (int)o->y+10);
            SDL_RenderDrawLine(rendu,
                (int)(o->x+o->largeur)+8, (int)o->y+12,
                (int)(o->x+o->largeur)+8, (int)o->y+6);
        } else {
            int bx = (int)(o->x + o->largeur / 2);
            int by = (int)(o->y + o->hauteur / 2);
            int w = (int)(o->largeur / 2);
            SDL_SetRenderDrawColor(rendu, 170, 130, 50, 255);
            dessiner_disque(bx, by, 6);
            SDL_SetRenderDrawColor(rendu, 200, 160, 60, 255);
            SDL_RenderDrawLine(rendu, bx-6, by, bx-w,   by-10);
            SDL_RenderDrawLine(rendu, bx-6, by, bx-w+4, by+4);
            SDL_RenderDrawLine(rendu, bx+6, by, bx+w,   by-10);
            SDL_RenderDrawLine(rendu, bx+6, by, bx+w-4, by+4);
        }
    }
}

static void dessiner_sol(const Jeu *jeu, int w) {
    int x, offset;
    SDL_SetRenderDrawColor(rendu, 110, 110, 110, 255);
    SDL_RenderDrawLine(rendu, 0, (int)jeu->sol_y + 1,
                       w,    (int)jeu->sol_y + 1);
    offset = (int)(jeu->score) % 55;
    SDL_SetRenderDrawColor(rendu, 65, 65, 65, 255);
    for (x = -offset; x < w; x += 55)
        SDL_RenderDrawLine(rendu, x,      (int)jeu->sol_y + 5,
                           x + 22, (int)jeu->sol_y + 5);
}

void UI_dessiner(const Jeu *jeu) {
    int w, h;
    SDL_Color blanc = {220, 220, 220, 255};
    SDL_Color jaune = {255, 215,   0, 255};
    SDL_Color rouge = {210,  50,  50, 255};
    SDL_Color gris = {120, 120, 120, 255};

    if (!rendu) return;
    SDL_GetRendererOutputSize(rendu, &w, &h);

    SDL_SetRenderDrawColor(rendu, 22, 22, 22, 255);
    SDL_RenderClear(rendu);
    dessiner_sol(jeu, w);

    if (jeu->etat == ETAT_JEU || jeu->etat == ETAT_GAME_OVER) {
        dessiner_obstacles(jeu);
        dessiner_araignee(jeu);
        afficher_fmt(w - 160, 14, blanc,
                     "Score : %05d", (int)jeu->score);
        if (jeu->meilleur_score > 0)
            afficher_fmt(w - 320, 14, gris,
                         "HI %05d", jeu->meilleur_score);
    }

    if (jeu->etat == ETAT_ATTENTE) {
        dessiner_araignee(jeu);
        afficher_texte("JEU DE SURVIE - ARAIGNEE",
                       w/2 - 145, h/2 - 70, blanc);
        afficher_texte("Appuie sur ESPACE pour commencer",
                       w/2 - 155, h/2 - 20, jaune);
        afficher_texte("Saute par-dessus les obstacles !",
                       w/2 - 150, h/2 + 20, gris);
    }

    if (jeu->etat == ETAT_GAME_OVER) {
        SDL_SetRenderDrawBlendMode(rendu, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(rendu, 0, 0, 0, 160);
        { SDL_Rect r = {0, 0, w, h}; SDL_RenderFillRect(rendu, &r); }
        SDL_SetRenderDrawBlendMode(rendu, SDL_BLENDMODE_NONE);

        afficher_texte("GAME OVER",
                       w/2 - 65, h/2 - 55, rouge);
        afficher_fmt(w/2 - 85, h/2 - 5,
                     blanc, "Score : %d", (int)jeu->score);
        /* Nouveau record ? */
        if ((int)jeu->score > 0
         && (int)jeu->score >= jeu->meilleur_score)
            afficher_texte("-- NOUVEAU RECORD --",
                           w/2 - 100, h/2 + 30, jaune);
        /* Toujours afficher "rejouer" */
        afficher_texte("ESPACE pour rejouer",
                       w/2 - 95, h/2 + 65, gris);
    }

    SDL_RenderPresent(rendu);
}