# OpenGL Abandoned Mineshaft Project

## Description

This project is a 3D abandonded mineshaft, cave generation and visualization tool implemented in C++ using OpenGL 3.3. It utilises Perlin noise to procedurally generate the cave structure and supports features like objects that make up an abandonded mineshaft, crystal formation, lighting, and a controllable camera system.

## Controls
To look around the scene you can use standard mouse and keyboard controls + shift for faster movement
- W: forward
- A: left
- S: backwards
- Q: up
- E: down
- Left Shift: move faster
- mouse: rotate camera

## Features

- Real-time 3D cave generation using Perlin noise
- Procedural placement of crystals within the cave
- Dynamic lighting with torches that cast soft shadows
- A first-person camera system for exploring the cave
- Support for loading and rendering 3D models

## Running it
To execute the program, please navigate to the OpenGL/x64/Debug directory. Within this directory, you'll find the OpenGL.exe executable file. Run this file to start the application, ensuring that all necessary resources are located within the same folder for proper operation.

## Dependencies
- [GLFW](https://www.glfw.org/) for window management
- [GLAD](https://glad.dav1d.de/) for OpenGL function loading
- [GLM](https://github.com/g-truc/glm) for mathematics
- [stb_image](https://github.com/nothings/stb) for texture loading
- [Assimp](http://www.assimp.org/) for model importing
- [IrrKlang](https://www.ambiera.com/irrklang/) for audio processing

## Acknowledgements
Tutorials and Research
- [OpenGL-Tutorial](https://learnopengl.com/)
- [fast terrain generation](https://ar5iv.labs.arxiv.org/html/1610.03525v3)
