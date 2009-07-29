PROJECT(tokyopp)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)

SET(CMAKE_VERBOSE_MAKEFILE ON)

# binaries get installed here
SET(CMAKE_INSTALL_PREFIX "/usr/")

#add definitions, compiler switches, etc.
ADD_DEFINITIONS(-Wall -O2 -DNDEBUG -g `getconf LFS_CFLAGS` -fPIC)

INCLUDE_DIRECTORIES(include
                    )

ADD_EXECUTABLE(test
               test/main.cpp
               )

TARGET_LINK_LIBRARIES(test tokyocabinet tokyotyrant)

INSTALL(DIRECTORY include/tokyooo DESTINATION include PATTERN ".svn" EXCLUDE)