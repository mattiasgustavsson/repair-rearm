![build](https://github.com/mattiasgustavsson/dos-like/workflows/build/badge.svg)

# Repair/Rearm

Shoot-em-up game made for Global Game Jam 2020.

The game is written in plain C, and can be played in the browser here:
https://mattiasgustavsson.com/wasm/repair-rearm


## Building

No build system is used, simply call the compiler from the commandline.


### Windows

From a Visual Studio Developer Command Prompt, do:
```
  cl source\repair-rearm.c
```  

For building the final release version, you probably want all optimizations enabled. There's a helper script (a windows bat file) in the `build` folder of the repo, which will build with full optimizations, and also include an application icon. It will also call the compiled exe to generate the `repair-rearm.dat` data file. See the `build\build_win.bat` file for details.


### Mac

```
  clang source/repair-rearm.c `sdl2-config --libs --cflags` -lGLEW -framework OpenGL -lpthread
```

SDL2 and GLEW are required - if you don't have them installed you can do so with Homebrew by running
```
  brew install sdl2 glew  
```


### Linux

```
  gcc source/repair-rearm.c `sdl2-config --libs --cflags` -lGLEW -lGL -lm -lpthread
```

SDL2 and GLEW are required - if you don't have them installed you can do so on Ubuntu (or wherever `apt-get` is available) by running
```
  sudo apt-get install libsdl2-dev
  sudo apt-get install libglew-dev
```


### WebAssembly

Using WAjic:
```
  wasm\node wasm/wajicup.js -embed repair-rearm.dat repair-rearm.dat source/repair-rearm.c repair-rearm.html
```

Note that you must have generated the `repair-rearm.dat` file (by running the repair-rearm executable once with command line argument -b or --build) before running this build command.

A WebAssembly build environment is required. You can download it (for Windows) here: [wasm-env](https://github.com/mattiasgustavsson/dos-like/releases/tag/wasm-env).
Unzip it so that the `wasm` folder in the zip file is at your repository root.

The wasm build environment is a compact distribution of [node](https://nodejs.org/en/download/), [clang/wasm-ld](https://releases.llvm.org/download.html),
[WAjic](https://github.com/schellingb/wajic) and [wasm system libraries](https://github.com/emscripten-core/emscripten/tree/main/system).

For a final release, you probably want the web page it is embedded on to look a bit nicer - there is a helper build script `build\build_web.bat` which does this, specifying a template html file.
