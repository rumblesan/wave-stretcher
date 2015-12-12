cmake_minimum_required(VERSION 3.0)
project(wave-stretcher)

include_directories(include)
file(GLOB SOURCES "src/core/*.c" "src/wav_stretch.c")

add_executable(wavstretch ${SOURCES})
target_link_libraries(wavstretch fftw3f sndfile mad mp3lame)

