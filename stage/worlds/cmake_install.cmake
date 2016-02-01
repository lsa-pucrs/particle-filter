# Install script for directory: /home/renan/git/particle-filter/stage/worlds

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
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/share/stage/worlds" TYPE FILE FILES
    "/home/renan/git/particle-filter/stage/worlds/uoa_robotics_lab.cfg"
    "/home/renan/git/particle-filter/stage/worlds/autolab.cfg"
    "/home/renan/git/particle-filter/stage/worlds/vfh.cfg"
    "/home/renan/git/particle-filter/stage/worlds/simple.cfg"
    "/home/renan/git/particle-filter/stage/worlds/amcl-sonar.cfg"
    "/home/renan/git/particle-filter/stage/worlds/wavefront.cfg"
    "/home/renan/git/particle-filter/stage/worlds/nd.cfg"
    "/home/renan/git/particle-filter/stage/worlds/camera.cfg"
    "/home/renan/git/particle-filter/stage/worlds/mbicp.cfg"
    "/home/renan/git/particle-filter/stage/worlds/wifi.cfg"
    "/home/renan/git/particle-filter/stage/worlds/lsp_test.cfg"
    "/home/renan/git/particle-filter/stage/worlds/everything.cfg"
    "/home/renan/git/particle-filter/stage/worlds/wavefront-remote.cfg"
    "/home/renan/git/particle-filter/stage/worlds/test.cfg"
    "/home/renan/git/particle-filter/stage/worlds/lsp_test.world"
    "/home/renan/git/particle-filter/stage/worlds/pioneer_walle.world"
    "/home/renan/git/particle-filter/stage/worlds/fasr2.world"
    "/home/renan/git/particle-filter/stage/worlds/wifi.world"
    "/home/renan/git/particle-filter/stage/worlds/camera.world"
    "/home/renan/git/particle-filter/stage/worlds/mbicp.world"
    "/home/renan/git/particle-filter/stage/worlds/fasr_plan.world"
    "/home/renan/git/particle-filter/stage/worlds/simple.world"
    "/home/renan/git/particle-filter/stage/worlds/SFU.world"
    "/home/renan/git/particle-filter/stage/worlds/sensor_noise_module_demo.world"
    "/home/renan/git/particle-filter/stage/worlds/random_particles.world"
    "/home/renan/git/particle-filter/stage/worlds/random_particles2.world"
    "/home/renan/git/particle-filter/stage/worlds/autolab.world"
    "/home/renan/git/particle-filter/stage/worlds/uoa_robotics_lab.world"
    "/home/renan/git/particle-filter/stage/worlds/fasr.world"
    "/home/renan/git/particle-filter/stage/worlds/pioneer_flocking.world"
    "/home/renan/git/particle-filter/stage/worlds/large.world"
    "/home/renan/git/particle-filter/stage/worlds/everything.world"
    "/home/renan/git/particle-filter/stage/worlds/salton.world"
    "/home/renan/git/particle-filter/stage/worlds/beacons.inc"
    "/home/renan/git/particle-filter/stage/worlds/walle.inc"
    "/home/renan/git/particle-filter/stage/worlds/map.inc"
    "/home/renan/git/particle-filter/stage/worlds/chatterbox.inc"
    "/home/renan/git/particle-filter/stage/worlds/pioneer.inc"
    "/home/renan/git/particle-filter/stage/worlds/sick.inc"
    "/home/renan/git/particle-filter/stage/worlds/objects.inc"
    "/home/renan/git/particle-filter/stage/worlds/hokuyo.inc"
    "/home/renan/git/particle-filter/stage/worlds/irobot.inc"
    "/home/renan/git/particle-filter/stage/worlds/ubot.inc"
    "/home/renan/git/particle-filter/stage/worlds/uoa_robotics_lab_models.inc"
    "/home/renan/git/particle-filter/stage/worlds/pantilt.inc"
    "/home/renan/git/particle-filter/stage/worlds/test.sh"
    "/home/renan/git/particle-filter/stage/worlds/cfggen.sh"
    "/home/renan/git/particle-filter/stage/worlds/worldgen.sh"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Unspecified")

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("/home/renan/git/particle-filter/stage/worlds/benchmark/cmake_install.cmake")
  INCLUDE("/home/renan/git/particle-filter/stage/worlds/bitmaps/cmake_install.cmake")
  INCLUDE("/home/renan/git/particle-filter/stage/worlds/wifi/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

