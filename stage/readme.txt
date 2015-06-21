
=================================================
HOW TO USE 'RANDOM PARTICLES' STAGE CONTROLLER
=================================================

- Install Stage 4.1.1
- In the stage distribution, go to dir examples/ctrl and open the
file CMakeLists.txt

SET( PLUGINS
  fasr
  sink
  source
  wander
  pioneer_flocking
  rasterize
  lasernoise
  dynamic
  random_particles   <==== add random_particles here !!!
)

- add random_particles.cc to the examples/ctrl dir
- go to stage home dir and proceed with configuration, compilation, installation
  $ ccmake .
  $ make 
  $ sudo make install
  
- if you already have stage compiled than execute the following commands to compile only  random_particles
  $ ccmake .
  $ sudo make random_particles install

- copy the files *.world to the 'worlds' dir
- go to 'worlds' dir and execute for single robot simulation
  $ stage random_particles.world
- go to 'worlds' dir and execute for two robot simulation
  $ stage random_particles2.world
