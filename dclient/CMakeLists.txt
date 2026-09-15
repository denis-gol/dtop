#
#


# пока делаю обычный вывод текста
add_executable(dclient main_client.cpp)
target_link_libraries(dclient PRIVATE dcommon)


# @todo но планирую UI на ncurses
#find_package(Curses REQUIRED)
# Подключаем пути к заголовочным файлам ncurses (там, где лежит curses.h)
#   будет искать в: /usr/includes, /usr/local/includes и других
#target_include_directories(dclient PRIVATE ${CURSES_INCLUDE_DIRS})

# Линкуем саму библиотеку ncurses к клиенту
#   будет искать в: /usr/lib, /usr/local/lib/ и других
#target_link_libraries(dclient PRIVATE ${CURSES_LIBRARIES})




