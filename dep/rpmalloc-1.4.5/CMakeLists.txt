cmake_minimum_required(VERSION 3.29)
project(rpmalloc)

add_library(rpmalloc
        rpmalloc/malloc.c
        rpmalloc/rpmalloc.c
        rpmalloc/rpmalloc.h
        rpmalloc/rpnew.h)