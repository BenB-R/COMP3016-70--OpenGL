# OpenGL Abandoned Mineshaft Project
This project is a 3D abandonded mineshaft, cave generation and visualisation tool implemented in C++ using OpenGL 3.3. It utilises Perlin noise to procedurally generate the cave structure and supports features like objects that make up an abandonded mineshaft, crystal formation, lighting, and a controllable camera system.
## User Interaction
### Controls
To look around the scene you can use standard mouse and keyboard controls + shift for faster movement
- W: forward
- A: left
- S: backwards
- Q: up
- E: down
- Left Shift: move faster
- mouse: rotate camera

### How to open the executable
To execute the program, please navigate to the OpenGL/x64/Debug directory. Within this directory, you'll find the OpenGL.exe executable file. Run this file to start the application, ensuring that all necessary resources are located within the same folder for proper operation.

## Youtube video
- [Abandoned Mineshaft](https://www.youtube.com/watch?v=p5mzPo0so-U)

## Program Architecture
The program is designed around several core components that work together to create an interactive 3D cave environment:

- `CaveGenerator`: This class is responsible for the procedural generation of the cave system. It uses Perlin noise to create a realistic terrain and manages the logic for adding and removing blocks to form the cave structure. It also handles the generation of crystals within the cave.

- `Model` and `Mesh`: These classes are part of the model-loading system. The `Model` class manages the loading of 3D models from external files using the Assimp library. Each model contains several `Mesh` objects that hold the data for rendering the geometry of the model parts.

- `Shader`: Defined in `shader.h`, this class simplifies the process of loading, compiling, and linking GLSL shaders. It provides methods for setting shader uniforms and for using the shaders during rendering.

- `Camera`: This class controls the view into the 3D world. It processes input for camera movement and updates the view matrix accordingly. It allows the user to navigate the cave from a first-person perspective.

## Controls
To look around the scene you can use standard mouse and keyboard controls + shift for faster movement
- W: forward
- A: left
- S: backwards
- Q: up
- E: down
- Left Shift: move faster
- mouse: rotate camera

## Unique Features

What makes this project stand out is its advanced procedural generation techniques, intuitive user interaction, and efficient rendering optimisations:

- **Advanced Procedural Generation**: Unlike simple random terrain, the cave systems are generated using noise algorithms, creating more natural and varied landscapes. 

- **Crystal Formation**: In addition to terrain, there are randomised crystal formations, adding depth and points of interest to the cave environment.

- **Dynamic Lighting**: The application includes a lighting system that simulates torchlight within the caves, providing a realistic and atmospheric experience.

- **First-Person Camera Control**: The user can freely navigate through the cave in first-person view, with smooth camera controls.

- **Selective Rendering**: To enhance performance, the code employs a selective rendering technique. It identifies adjacent blocks within the terrain and omits the rendering of shared faces, optimising the rendering process.

- **Inspiration and Evolution**: Inspired by the blocky aesthetics of games like 'Minecraft' and the vast procedural worlds of 'No Man's Sky', our project began with basic terrain generation and evolved to feature complex cave systems.

- **Custom Shader and Model Loading**: The project has a shader loader and model importing system, allowing for easy shader and model importing of multiple formats.

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
