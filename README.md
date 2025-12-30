# Cascade
*Complexity cascades from simplicity*
## About
Cascade is a framework that can be used to make games, simulations, and other applications. It wraps around several libraries to create a common interface for drawing, playing sounds, reading input, and rendering text.

Some demos can be found in the [Cascade Demos](https://github.com/voelkerj/Cascade-Demos) project.

## Stucture
The backbone of Cascade is an Entity Component System (ECS) using the [entt](https://github.com/skypjack/entt) library.

To use Cascade, you write classes that inherit from two base classes: **Systems** and **Scenes**.

**Systems** contain Load(), Update(), and Cleanup() functions. They are designed to operate on entities and their data.

**Scenes** also contain Load(), Update(), and Cleanup() functions. They are meant to manage loading of entities and updating based on user input.

There actually isn't much of a difference in these behind the scenes, they can pretty much be used interchangeably.

## Build
*(your build system may vary)*
1. mkdir build
2. cd build
3. cmake ..
4. mingw32-make.exe

[Cascade Demos](https://github.com/voelkerj/Cascade-Demos) contains examples of how to link this library to a project using CMake.