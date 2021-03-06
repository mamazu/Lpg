## CI
* TyRoXx (main)
[![Build Status](https://travis-ci.org/TyRoXx/Lpg.svg?branch=master)](https://travis-ci.org/TyRoXx/Lpg)
[![Build status](https://ci.appveyor.com/api/projects/status/lq9sc1am1xn5fvgg/branch/master?svg=true)](https://ci.appveyor.com/project/TyRoXx/lpg/branch/master)
* mamazu (fork)
[![Build Status](https://travis-ci.org/mamazu/Lpg.svg?branch=master)](https://travis-ci.org/mamazu/Lpg)

## Usage
For a general introduction, have a look at the file `documentation/Language.md`

Just write a `.lpg` file and run it. If you want to have syntax highlighting, there is a [Visual Studio Code Plugin](https://github.com/mamazu/LPG-for-Visual-Studio-Code) and a Notepad++ syntax file in this repository.

### Compilation
In order to compile and run an `.lpg` file you need to build the whole project and run the following command in the terminal:

* On Windows:
> `{build-directory}/cli/lpg.exe` and give it the `.lpg` file as argument.

* On Linux
> `{build-directory}/cli/lpg` and give it the `.lpg` file as argument.

## Development
Currently supported operating systems are:
* Windows
* Linux

### Build and run on Ubuntu
Install at least the following tools:
```bash
sudo apt-get install git cmake gcc valgrind ninja-build nodejs
```

How to build and run the tests:
```bash
git clone https://github.com/TyRoXx/Lpg.git
mkdir build
cd build
cmake ../Lpg -G "CodeBlocks - Unix Makefiles"
cmake --build .
./tests/tests
```

You can also open the project using an IDE. QtCreator is recommended:
```bash
sudo apt-get install qtcreator
```

### Test coverage on Ubuntu

How to measure test coverage:
```bash
sudo apt-get install lcov firefox
cd build
make testcoverage
```
It will run the tests, generate an HTML coverage report and open it in Firefox.

### Code formatting
Install `clang-format` like this on Ubuntu:
```bash
sudo apt-get install clang-format-3.9
```

On Windows, you can get clang-format by installing Clang for Windows:

* http://releases.llvm.org/3.9.1/LLVM-3.9.1-win64.exe

You have to use clang-format 3.9 or otherwise the formatting will be inconsistent.

Re-run `cmake` in the build directory to make it will look for `clang-format`:
```bash
cmake .
```

To format, run this in the build directory:
```bash
make clang-format
```

### Build and run on Windows

* Use CMake GUI to generate a solution for your favorite version of Visual Studio.
* Open the solution in Visual Studio as usual.
* Run the `tests` project to check whether your changes work.
