//
// Created by denisg on 30.09.2026.
//

#ifndef DTOP_METRICSHANDLER_H
#define DTOP_METRICSHANDLER_H

#include <cstdint>

class MetricsCollector{
private:
    // храним сырые тики CPU из /proc/stat
    struct CpuTicks {
      uint64_t user, nice, system, idle, iowait, irq, softirq, steal;

      uint64_t get_idle() const { return idle+iowait; }
      uint64_t get_active() const { return user+nice+system+irq+softirq+steal; }
      uint64_t get_total() const { return get_idle()+get_active(); }
    };

    // предыдущее состояние тиков
    CpuTicks m_prev_cpu;

    static CpuTicks read_cpu_ticks();

public:
    MetricsCollector();

    uint8_t calculate_cpu_usage();
    uint8_t get_ram_usage_percentage();

};

#endif //DTOP_METRICSHANDLER_H
