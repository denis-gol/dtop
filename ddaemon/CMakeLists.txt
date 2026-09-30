#
#

set(DAEMON_SOURCES
        main_daemon.cpp
        DaemonGuard.cpp
        DaemonHandler.cpp
        MetricsCollector.cpp
)

set(DAEMON_HEADERS
        DaemonGuard.h
        DaemonHandler.h
        MetricsCollector.h
)

add_executable(ddaemon ${DAEMON_SOURCES} ${DAEMON_HEADERS})

# подключаем общую часть
target_include_directories(ddaemon PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../dcommon)


# если вдруг использовать потоки (пока не планировал)
#find_package(Threads REQUIRED)
#target_link_libraries(ddaemon PRIVATE Threads::Threads)
