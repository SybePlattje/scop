# Description
This project is a small graphical application designed to help me begin learning OpenGL.  
It uses GLAD and GLFW to load andrender `.obj` 3D bodel files, and it allows rotation of the loaded object using the keyboard input.  
All transformation mathematics (rotation matrices) are inplemented manually as was a constraint for this project.

![42 in grayscale color](/images/42Color.png)

![42 with a nyan cat texture](/images/42Texture.png)

![teapot in grayscale color](/images/teapotColor.png)

![teapot with a nyan cat texture](/images/teapotTexture.png)

![teapot with a nyan cat texture zoomed out](/images/teapotTextureZoomedOut.png)

![teapot with a nyan cat texture aplied per triangle face](/images/teapotTexturePerFace.png)

# Features
* Loads and parse `.obj` 3D model files
* Displays 3D geometry using OpenGL
* Manual implementation of rotation math (no GLM)
* interactive rotation using keyboard input
* Buils system using Make (as per project constraint)

# Requirements / Dependencies
* GLFW
* OpenGL
* c++ copiler (GCC / G++ / CLANG)
* Make

## For Linux
`sudo apt install libglfw2-def libgl1-mesa-dev xorg-dev`

## For Windows
### Using MSYS2
1. Install MSYS2
2. OPen MSYS2 MingGW64 terminal
3. Install dependencies

`pacman -S mingw-w64-x86_64-glfw mingw-w64-x86_64-toolchain`  

### Uning vcpkg
1. Install vcpkg
2. install GLFW

`vcpkg install glfw3`

### Notes
* make sure that GLWF headers and `.lib` files are visible to the complier/linker
* The executable will aslo require `glfw3.dll` at runtime (same folder as the `.exe` or in PATH)
* OpenGL drivers should already be provided by the GPU vender (NVIDIA / AMD / Intal)

# build
`make`

# Run
`./scop <path/to/model.obj>`

# Controls
W / S Rotate object around X-axis  
A / D Rotate object around Y-axis  
Q / E Rotate object around Z-axis  
R Reset the object back to original rotation  
F Change mesh from hole object to per face  
T Change from color to Texture  
\- / + Zooming in/out on the object ( non num lock keys )  
ESC closes application  
