//
// Created by admin on 30.09.2026.
//

#include <fstream>
#include <string>
#include <cstdint>

#include "MetricsCollector.h"

// сразу делаем замер при создании
MetricsCollector::MetricsCollector() :m_prev_cpu(read_cpu_ticks()) {}

uint8_t MetricsCollector::calculate_cpu_usage(){

    MetricsCollector::CpuTicks current_cpu = read_cpu_ticks();

    uint64_t total_delta = current_cpu.get_total() - m_prev_cpu.get_total();
    uint64_t active_delta = current_cpu.get_active() - m_prev_cpu.get_active();

    uint8_t cpu_usage = 0;
    if (total_delta > 0) {
        cpu_usage = static_cast<uint8_t>((100 * active_delta) / total_delta);
    }
    m_prev_cpu = current_cpu;

    return cpu_usage;
}

MetricsCollector::CpuTicks MetricsCollector::read_cpu_ticks(){
    MetricsCollector::CpuTicks ticks{};
    std::ifstream file("/proc/stat");
    std::string cpu_label;
    // @todo - так плохо читать, нужно брать строку целиком и резать по пробелам
    if (file >> cpu_label >> ticks.user >> ticks.nice >> ticks.system
             >> ticks.idle >> ticks.iowait >> ticks.irq >> ticks.softirq >> ticks.steal) {
        return ticks;
    }
    return {};
}

uint8_t MetricsCollector::get_ram_usage_percentage() {
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


