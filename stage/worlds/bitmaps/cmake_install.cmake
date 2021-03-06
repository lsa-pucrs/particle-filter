# Install script for directory: /home/renan/git/particle-filter/stage/worlds/bitmaps

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "RELEASE")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "1")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/stage/worlds/bitmaps" TYPE FILE FILES
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/hospital_section.png"
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/rink.png"
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/table.png"
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/cave_compact.png"
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/salton_world.png"
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/simple_rooms.png"
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/cave.png"
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/sal2.png"
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/submarine_small.png"
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/889_05.png"
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/SFU_1200x615.png"
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/space_invader.png"
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/submarine.png"
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/hospital.png"
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/salton_world2.png"
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/frieburg.png"
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/uoa_robotics_lab.png"
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/mbicp.png"
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/cave_filled.png"
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/SRI-AIC-kwing.png"
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/human_outline.png"
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/autolab.png"
    "/home/renan/git/particle-filter/stage/worlds/bitmaps/ghost.png"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

