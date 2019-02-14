# Airplane Monitoring System

A cost-effective retrofit system that collects, stores and transmits Airplane Environmental Data.

### Prerequisites

Install `cmake`, `make`, `gcc/g++` compilers for arm (for cross compiling), and the latest `node` and `npm` packages.

### Installing and running on Linux

1. Build the system:<br>
    `mkdir <some-build-dir> && cd <some-build-dir>`
    <br>
2. Run CMake command pointing to the sources root:<br>
    `cmake <sources-root>`
    <br>
3. Make the system from your build directory:<br>
    `make` (to build tests, you can run `make tests`)
    <br>
4. Now to run the embedded system's internal server's binary, run the following from your build directory:<br>
    `./dst/embeddedserver/internalserver/bin/internalserver`
    <br>
5. Now install the embedded system's NodeJS server's dependencies from your build directory:<br>
    `cd dst/embdeddedserver/nodeserver && npm install && cd -`
    <br>
6. Now run the embedded system's NodeJS server (the gateway server to the host server) from your build directory:<br>
    `node dst/embdeddedserver/nodeserver/server.js`
    <br>
7. Now install the host system's NodeJS server's dependencies from your build directory:<br>
    `cd dst/hostserver/nodeserver && npm install && cd -`
    <br>
8. Setup the embedded server's locations by editing the file `dst/hostserver/nodeserver/lib/locations.json`, and editing the `airplane-name: location` pairs.<br>
9. Now run the host system's NodeJS server from your build directory:<br>
    `node dst/hostserver/nodeserver/server.js`
    <br>
10. Voilà! Now you can test your airplane systems.

## The directory structure of the project is as follows:

alpha<br>
├── embedded_system .................. Code that will run on the BeagleBone<br>
│   ├── c/ ........................... C/C++ application code and resources<br>
│   │   ├── build/ ................... Output folder for any compiled objects/executables<br>
│   │   ├── conf/ .................... Configurations for application/tests<br>
│   │   ├── doc/ ..................... Text documentation and development notes<br>
│   │   ├── extras/ .................. Other stuff<br>
│   │   │   └── resources/ ........... Stuff like icons, fonts etc in application<br>
│   │   ├── include/ ................. Header files (.h) for internal libraries<br>
│   │   │   ├── LightSensor.h<br>
│   │   │   ├── System.h<br>
│   │   │   └── ...<br>
│   │   ├── lib/ ..................... External and internal libraries<br>
│   │   │   ├── Unity/ ............... ("external library")<br>
│   │   │   │   └── ...<br>
│   │   │   ├── LightSensor.c ........ ("internal library")<br>
│   │   │   ├── System.c<br>
│   │   │   └── ...<br>
│   │   ├── src/ ..................... Entry point of application<br>
│   │   │   └── main.c<br>
│   │   └── tests/ ................... Unit tests (.c) go here<br>
│   │       └── SystemTest.c<br>
│   └── node/ ........................ Node.js code and resources<br>
├── host_system<br>
└────── node/ ........................ Node.js code and resources<br>

## Authors

* **Faisal Al-Humaimidi**   (falhumai@sfu.ca)
* **Karan Sharma**          (ksa95@sfu.ca)
* **Yernur Nursultanov**    (ynursult@sfu.ca)
