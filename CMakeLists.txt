cmake_minimum_required(VERSION 3.20)
project(HighschoolLMS LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_path(SQLITE3_INCLUDE_DIR sqlite3.h PATHS "C:/msys64/ucrt64/include" REQUIRED)
find_library(SQLITE3_LIBRARY sqlite3 PATHS "C:/msys64/ucrt64/lib" REQUIRED)
find_path(ARGON2_INCLUDE_DIR argon2.h PATHS "C:/msys64/ucrt64/include" REQUIRED)
find_library(ARGON2_LIBRARY argon2 PATHS "C:/msys64/ucrt64/lib" REQUIRED)

add_executable(HighschoolLMS
    src/main.cpp src/User.cpp src/UserRoles.cpp src/SQLiteDatabase.cpp
    src/UserRepository.cpp src/PasswordHasher.cpp src/LmsRepository.cpp
    src/LatePenaltyStrategy.cpp)
target_include_directories(HighschoolLMS PRIVATE include "${SQLITE3_INCLUDE_DIR}" "${ARGON2_INCLUDE_DIR}")
target_link_libraries(HighschoolLMS PRIVATE "${SQLITE3_LIBRARY}" "${ARGON2_LIBRARY}" bcrypt)
