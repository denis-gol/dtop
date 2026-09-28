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

// Структура для хранения сырых тиков CPU из /proc/stat
struct CpuTicks {
  uint64_t user, nice, system, idle, iowait, irq, softirq, steal;

  uint64_t get_idle() const { return idle + iowait; }
  uint64_t get_active() const { return user + nice + system + irq + softirq + steal; }
  uint64_t get_total() const { return get_idle() + get_active(); }
};

// Функция чтения сырых тиков процессора
CpuTicks read_cpu_ticks() {
    CpuTicks ticks{};
    std::ifstream file("/proc/stat");
    std::string cpu_label;
    // @todo - так плохо читать, нужно брать строку целиком и резать по пробелам
    if (file >> cpu_label >> ticks.user >> ticks.nice >> ticks.system
             >> ticks.idle >> ticks.iowait >> ticks.irq >> ticks.softirq >> ticks.steal) {
        return ticks;
    }
    return {};
}

// Функция расчета реальной загрузки RAM в процентах
uint8_t get_ram_usage_percentage() {
    std::ifstream file("/proc/meminfo");
    std::string label;
    uint64_t value = 0;
    uint64_t total = 0;
    uint64_t available = 0;

    while (file >> label >> value) {
        if (label == "MemTotal:") total = value;
        else if (label == "MemAvailable:") available = value;

        // Переходим к следующей строке (пропускаем "kB")
        std::string dummy;
        file >> dummy;

        if (total && available) break;
    }

    if (total == 0) return 0;

    return static_cast<uint8_t>(100 * (total - available) / total);
}

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

    // Делаем замер CPU перед стартом цикла для расчета дельты времени
    CpuTicks prev_cpu = read_cpu_ticks();

    while (g_running) {
        // Проверяем, прислал ли UI какую-то команду (таймаут 0 — проверяем мгновенно)
        int ret = poll(fds, 1, 200);

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
                break;
            }
        } else if (ret < 0) {
            if (errno == EINTR) continue;
            logger.log_message("Ошибка системного вызова poll()");
            break;
        }

        // Если мы не на паузе, генерируем метрики
        MetricsPacket metrics{};

        if (!is_paused) {
            iter++;
        }
        metrics.iteration = iter;

        // собираем метрики процессоруа
        CpuTicks current_cpu = read_cpu_ticks();
        uint64_t total_delta = current_cpu.get_total() - prev_cpu.get_total();
        uint64_t active_delta = current_cpu.get_active() - prev_cpu.get_active();

        if (total_delta > 0) {
            metrics.cpu_usage = static_cast<uint8_t>((100 * active_delta) / total_delta);
        } else {
            metrics.cpu_usage = 0;
        }
        prev_cpu = current_cpu;

        // собираем метрики памяти
        metrics.ram_usage = get_ram_usage_percentage();

        if (is_paused) {
            std::strncpy(metrics.status_text, "PAUSED", sizeof(metrics.status_text));
        }
        else {
            std::strncpy(metrics.status_text, "RUNNING", sizeof(metrics.status_text));
        }

        // Отправляем пакет с метриками UI-клиенту
        ::send(client_fd, &metrics, sizeof(metrics), 0);

    }

    close(client_fd);
    logger.log_message("Сервер успешно остановлен.");

    return 0;
}
