# Cascade
<div align="center">*Complexity Cascades from Simplicity*</div>  

## About
Cascade is a framework making games, simulations, and other applications. It wraps around several libraries to create a common interface for drawing, playing sounds, reading input, and rendering text.

Some demos can be found in the [Cascade Demos](https://github.com/voelkerj/Cascade-Demos) project.

## Special Thanks
Cascade wraps the following libraries:  
* [entt](https://github.com/skypjack/entt) for an Entity Component System (ECS)  
* [SDL](https://github.com/libsdl-org/SDL) for window and rendering functionality
* [SDL Image](https://github.com/libsdl-org/SDL_image) for loading images as SDL surfaces
* [SDL ttf](https://github.com/libsdl-org/SDL_ttf) for rendering text with SDL

## Build
1. mkdir build
2. cd build
3. cmake ..
4. mingw32-make.exe  
*(or other build command)*