cmake_minimum_required(VERSION 3.0)
project(wave-stretcher)

include_directories(include)
file(GLOB SOURCES "src/core/*.c")

add_executable(wavstretch "src/wav_stretch.c" ${SOURCES})
target_link_libraries(wavstretch fftw3f sndfile mad mp3lame)
