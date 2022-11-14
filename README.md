rym
===

> A game about beauty, explosions, lines, squares and a triangle.

![Logo](rym-logo.png)

About the game
--------------

This game is dear to me. I started working on it in 2008 *by accident* when
I was just fooling around. The game in it self is nothing special, it is kind
of beautiful and meditative. And because it is so easy to work with I have
created ports for it to almost every platform that I've used. (Files for
building on these platforms is not included in this project).

In 2020-2021 I did a push to convert the whole project to c++20 modules, and
that will probably make compilation to other platforms more difficult for the
nearest future.

2022-11-14 Finalized real c++-20 modules using newest features of clang


Build on ubuntu using matmake2
---------------

#### install matmake3

```http
https://github.com/mls-m5/matmake3/
```

#### install clang-16
```bash
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 16
sudo apt install libc++-16-dev libc++abi-16-dev # This is needed for modules to work
sudo apt install libsdl2-dev
```


#### install SDL2

```bash
sudo apt install libsdl2-dev
```

Build with
```bash
path/to/matmake3 --link -lSDL2 --link -lSDL2_image --link -lGL -j 1 --debug --flag "-DGL_GLEXT_PROTOTYPES=1 -DGL3_PROTOTYPES=1"
```

Install on android
------------------

If you search for rym on google play stor you should probably find a installer.

![Gameplay](rym1.png)
![Gameplay](rym2.png)
