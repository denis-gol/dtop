//
// Created by admin on 15.09.2026.
//

#ifndef DTOP_DAEMONHANDLER_H
#define DTOP_DAEMONHANDLER_H

// @todo - пока не реализовано
enum State {
  RUNNING,  // демон работает полностью
  PAUSED    // работает, но пропускает тяжелый шаг парсинга
};

void daemonize();

#endif //DTOP_DAEMONHANDLER_H
