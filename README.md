# PixelGame

This project implements a game engine using the SDL2 library with rudimentary 3D graphics rendered with raycasting. It supports rendering rectangular walls and sprites facing the direction of the camera as well as projectiles and simple pathfinding.

This implementation of raycasting works by projecting rays starting from the player in various directions until they hit an opaque wall. The perpendicular distance between the camera plane (a surface representing the screen) and the wall is then the length of the wall to be rendered on screen. Each ray is responsible for one vertical column on the screen.

The textures and sprites used in the demonstration are from [Wolfenstein 3D](https://en.wikipedia.org/wiki/Wolfenstein_3D).

# Building

First, run this command to install the necessary packages:

```
apt-get install mesa-utils libsdl2-dev libsdl2-ttf-dev libsdl2-ttf-dev
```

Run the following commands in the terminal to build this project:

```
git clone https://github.com/BattleMage0231/PixelGame.git
cd PixelGame
mkdir build && cd build
cmake ..
make
```

The executable will then be built at `./game`.

# Gallery

![image](./screenshots/screenshot1.png)

![image](./screenshots/screenshot2.png)

![image](./screenshots/screenshot3.png)
