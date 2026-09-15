## аналог top/htop (учебный проект)
(базовый функционал, для понимания процессов под капотом Linux)

---

### Использованные концепции ОС Linux и C++ 
1) Создание демона (двойной форк) и контроль его запуска: 
   - fd: open, close
   - Processes: pid, pgid, sid
   - signals
   - Single Instance Control: flock (lock-file, pid-file)
   - Daemonization: fork, dup2, umask, chdir, stdio>>/dev/null
   - Управление loop: volatile, sig_atomic_t

1) 