cmake_minimum_required(VERSION 3.6)
project(Lab0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    common/Linux/MicroGlut.c
    common/Linux/MicroGlut.h
    common/Mac/MicroGlut.h
    common/Windows/glew.c
    common/Windows/glew.h
    common/Windows/MicroGlut.c
    common/Windows/MicroGlut.h
    common/Windows/wglew.h
    common/GL_utilities.c
    common/GL_utilities.h
    common/loadobj.c
    common/loadobj.h
    common/LoadTGA.c
    common/LoadTGA.h
    common/VectorUtils3.c
    common/VectorUtils3.h
    common/zpr.c
    common/zpr.h
    Lab0/objects/bilskiss.obj
    Lab0/objects/bunnyplus.obj
    Lab0/objects/cubeplus.obj
    Lab0/objects/stanford-bunny.obj
    Lab0/objects/teapot.obj
    Lab0/objects/teddy.obj
    Lab0/textures/bilskissred.tga
    Lab0/textures/conc.tga
    Lab0/textures/dirt.tga
    Lab0/textures/grass.tga
    Lab0/textures/maskros512.tga
    Lab0/lab0
    Lab0/lab0.c
    Lab0/lab0.frag
    Lab0/lab0.vert
    Lab0/makefile
    "Lab0/makefile (copy)")

add_executable(Lab0 ${SOURCE_FILES})