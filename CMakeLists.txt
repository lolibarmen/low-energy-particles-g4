cmake_minimum_required(VERSION 3.16)
project(geant4_dose_calculation)

# Поиск Geant4
find_package(Geant4 REQUIRED)
include(${Geant4_USE_FILE})

# Настройка компилятора
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Добавление исполняемого файла
add_executable(dose_calculation src/main.cpp)

# Подключение заголовочных файлов
target_include_directories(dose_calculation PRIVATE include)

# Подключение библиотек Geant4
target_link_libraries(dose_calculation ${Geant4_LIBRARIES})

# Установка целевых файлов
install(TARGETS dose_calculation DESTINATION bin)
install(DIRECTORY macros DESTINATION share/geant4-dose-calc)