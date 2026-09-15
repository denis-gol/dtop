//
// Created by admin on 11.09.2026.
//
#include <iostream>
#include <chrono>
#include <thread>
#include <string>

int main() {
    int frame = 0;

    // Отключаем синхронизацию со стандартными C-потоками для ускорения вывода
    std::ios_base::sync_with_stdio(false);

    while (frame < 100) {
        // \r возвращает курсор в начало строки.
        // Манипулятор std::flush принудительно выталкивает текст на экран без переноса строки.
        std::cout << "\r[DTOP] Статус демона: работает. Кадр: " << frame++ << std::flush;

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    std::cout << std::endl; // В конце переносим строку, чтобы не испортить вывод терминала
    return 0;
}
