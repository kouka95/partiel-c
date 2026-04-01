#include "game.h"

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

    Game_init();

    while (Game_running()) {
        Game_update();
        Game_draw();
        Game_wait();
    }

    Game_quit();
    return 0;
}