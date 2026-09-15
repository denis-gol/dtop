//
// Created by admin on 11.09.2026.
//

#include <csignal> // sig_atomic_t
#include <thread> // sleep_for
#include <fstream> // std::ofstream

#include "DaemonGuard.h"
#include "DaemonHandler.h"

// атомик в C-style
volatile sig_atomic_t g_running = 1;

// пишем сюда (@todo - временно, демонстрация работы демона)
const char* const LOG_FILE = "/tmp/ddaemon.log";

// Функция для записи логов в файл по абсолютному пути (@todo - временно, демонстрация работы демона)
void log_message(const std::string& message) {
    std::ofstream log(LOG_FILE, std::ios_base::app);
    if (log.is_open()) {
        auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::string time_str = std::ctime(&now);
        time_str.pop_back(); // Удаляем символ переноса строки из ctime
        log << "[" << time_str << "] " << message << std::endl;
    }
}


int main() {

    daemonize();
    DaemonGuard guard("dtop");

    int iter = 0;

    while (g_running) {
        // Имитация полезной работы
        log_message("демон работает. " + std::to_string(iter++));
        std::this_thread::sleep_for(std::chrono::seconds(2));

    }

    return 0;
}
