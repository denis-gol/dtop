#
#

add_executable(ddaemon main_daemon.cpp)

# подключаем общую часть
target_link_libraries(ddaemon PRIVATE dcommon)

# если вдруг использовать потоки (но пока не планировал)
#find_package(Threads REQUIRED)
#target_link_libraries(ddaemon PRIVATE Threads::Threads)
