//
// Created by admin on 11.09.2026.
//

#include <csignal> // sig_atomic_t

#include "DaemonGuard.h"
#include "DaemonHandler.h"
#include "Bind.h"
#include "dcommon.h"
#include "Logger.h"

// атомик в C-style. Здесь нужен, т.к. мы обрабатываем сигналы (они асинхронны)
volatile sig_atomic_t g_running = 1;

#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <poll.h>
#include <cstring>

int main() {

    daemonize();
    DaemonGuard guard("dtop");

    Logger logger("SERVER");
    logger.log_message("Daemon's log starts");

    Bind bind(SOCKET_PATH);
    logger.log_message("Ожидание подключения UI-клиента в соседнем терминале...");

    int client_fd = bind.accept_connection();
    logger.log_message("клиент успешно подключился. client_fd=" + std::to_string(client_fd));

    struct pollfd fds[1];
    fds[0].fd = client_fd;
    fds[0].events = POLLIN;

    uint32_t iter = 0;
    bool is_paused = false;
    bool running = true;

    while (g_running) {
        // Проверяем, прислал ли UI какую-то команду (таймаут 0 — проверяем мгновенно)
        int ret = poll(fds, 1, 0);
        if (ret>0 && (fds[0].revents & POLLIN)) {
            ControlPacket cpack{};
            ssize_t bytes = recv(client_fd, &cpack, sizeof(cpack), 0);

            if (bytes<=0) {
                logger.log_message("UI-клиент разорвал соединение.");
                break;
            }


            logger.log_message("Получена команда ID: " + std::to_string((int) cpack.command_id));
            if (cpack.command_id==1) {
                is_paused = true;
            }
            else if (cpack.command_id==2) {
                is_paused = false;
            }
            else if (cpack.command_id==3) {
                running = false;
                break;
            }
        }

        // Если мы не на паузе, генерируем метрики
        MetricsPacket metrics{};


        // @frag - тут обрабатываем реальные метрики (сейчас - фейковые).
        if (!is_paused) {
            iter++;
        }
        metrics.iteration = iter;
        // Генерируем фейковую пилообразную загрузку для теста баров
        metrics.cpu_usage = (iter*7)%101;
        metrics.ram_usage = (40+(iter%30));

        if (is_paused) {
            std::strncpy(metrics.status_text, "PAUSED", sizeof(metrics.status_text));
        }
        else {
            std::strncpy(metrics.status_text, "RUNNING", sizeof(metrics.status_text));
        }

        // Отправляем пакет с метриками UI-клиенту
        ::send(client_fd, &metrics, sizeof(metrics), 0);

        // Имитируем шаг измерения в 200 миллисекунд
        // @frag - а как сделать без имитации, чтобы не загрузить проц???
        usleep(2e5);
    }

    close(client_fd);
    logger.log_message("Сервер успешно остановлен.");

    return 0;
}
