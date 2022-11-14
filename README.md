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



Alternative 1. Build on ubuntu with make
--------------------------------------

#### Install SDL2
```bash
sudo apt install libsdl2-dev
```

#### Build
```bash
make
```

Alternative 2. Build on ubuntu using matmake2
---------------

#### install matmake2

```http
https://github.com/Laserskold/matmake2
```

#### install clang-11
```bash
wget https://apt.llvm.org/llvm.sh
chmod +x llvm.sh
sudo ./llvm.sh 11
sudo apt install libc++-11-dev libc++abi-11-dev # This is needed for modules to work
sudo apt install libsdl2-dev
```


#### install SDL2

```bash
sudo apt install libsdl2-dev
```

Build with
```
path/to/matmake3 --link -lSDL2 --link -lSDL2_image --link -lGL
```

Install on android
------------------

If you search for rym on google play stor you should probably find a installer.

![Gameplay](rym1.png)
![Gameplay](rym2.png)
