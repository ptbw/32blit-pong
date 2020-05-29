cmake_minimum_required(VERSION 3.8)
project (pong)
include (~/git/32blit-beta/32blit.cmake)
blit_executable (pong pong.cpp ball.cpp paddle.cpp)


