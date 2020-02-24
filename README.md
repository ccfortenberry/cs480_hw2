# CS 487/687 Game Engine and Game Project

## VCPKG commandline

We want to use the OpenGL Mathematics Library (GLM), SDL2, SDL2 (with JPEG support), SDL2_ttf, SDL2_mixer (with MP3 and OGG support), and CZMQ (with cURL, HTTPD, and UUID support). First install VCPKG by cloning the repository `https://github.com/Microsoft/vcpkg`. Follow the build instructions. We want to install the libraries for x64 Windows, so make sure to use the `--triplet` command. The `--recurse` option is to ensure that we update any existing packages.

     Z:\github\vcpkg> .\vcpkg install --recurse --triplet x64-windows glm sdl2 sdl2-image[libjpeg-turbo] sdl2-ttf sdl2-mixer[mpg123,libvorbis] czmq[curl,httpd,uuid]

## Building with CMake

```
$ cd build
$ cmake ..
$ cmake --build .
```

## Linux
Install g++-9 (Required to use std::filesystem on ubuntu), cmake, make, SDL2_ttf, SDL2_mixer, and CZMQ with your 
preferred pacakge manager. Next run
```
cd build
cmake ..
cmake --build .
./simplegame/simplegame
```
On ubuntu the version of cmake is 2 versions out of date. Inorder to install the latest cmake run
```
$ sudo snap install --classic cmake
```


MIDI Playback: You may need to install `timidity++` and `gt` which are the soundfonts and patches needed for SDL2_Mixer.

## Adding files to CMakeLists.txt

If you add files to the project, then be sure to add them to the CMakeLists.txt file to add them to the build. Since our main platform is Visual Studio, make sure they get added to the solutions and projects as well.

WARNING: Do not overwrite the VS build with something made by CMake.

## GameLib

### `namespace GameLib`

Every part of the game library will use the namespace `GameLib` so we can use nice short class names like `Context`.

### `Context`

This class is responsible for managing the initialization/shutdown of external libraries in addition to the screen.

### `Object`

This class is a base class for managing any kind of game entity used in the engine.

### `Actor`

This class (child class of  `Object`) is a type of object that can interact in the game world. Think of these as *"physical"* objects.

### `World`

This class (child class of `Object`) manages a list of `Actor`s that interact in the game world.

## SimpleGame

This program is designed to test various features of the engine. The primary goal is to make this a simple arcade game to test essential features.

## PlatformAdventure

This program is the class project. Each team member will help carve out their own unique world based on the theme "What is our story?"
