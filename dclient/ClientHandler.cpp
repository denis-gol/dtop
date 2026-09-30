//
// Created by denisg on 18.09.2026.
//


#include "ClientHandler.h"

void draw_progress_bar(WINDOW* win, int y, int x, const std::string& label, uint8_t percentage) {
    const int BAR_WIDTH = 20;
    int filled_bars = (percentage * BAR_WIDTH) / 100;

    // Снижаем мерцание: перемещаем курсор и перезаписываем строку точечно
    mvwprintw(win, y, x, "%s: [", label.c_str());

    for (int i = 0; i < BAR_WIDTH; ++i) {
        if (i < filled_bars) {
            waddch(win, '|'); // Символ заливки
        } else {
            waddch(win, '.'); // Символ пустоты
        }
    }
    wprintw(win, "] %3d%% ", percentage); // Пробел в конце затирает старые символы, если процент уменьшился
}
