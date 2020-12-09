# Cellular Automata Pseudoranom Number Generator Analyzer (CAPY)

ECE 3710 Project by Ava Decker  
10730830

# Usage

```
Cellular Automata Pseudorandom number generator analYzer (CAPY)

Simulates a CA PRNG and maps arbitrary output streams to all possible walks
through the machines internal state. Determines the total number of distinct
output streams and distinct internal states the machine can be determined to
exist in as the output stream grows in length.

Usage:
capy.exe [options] <output file>

Options:
-h          Displays this message.
-r <rule>   Specifies the elementary cellular automata rule to use.
            Supported rules are 30, 45, 54, 90, 105, and 124.
-s <size>   Uses an automata of <size> cells. Must be between 1 and 64.
-o <cell>   Read output from the <cell>th cell. Must be between 0 and <size>.
            Cells are counted from right to left.
            Default: 0

```

# Setting up a Development Environment

## Installing MSYS2
- Download and run the [MSYS2 installer](https://www.msys2.org).

- Launch MSYS2 from the `MSYS2 MSYS` start menu shortcut.

- Ensure packages are up to date with `pacman -Syu` (case sensitive).

- You may be prompted to close the terminal. If so, enter (Y)es, restart MSYS2, and run `pacman -Su`.

- After core packages have updated, run `pacman -S git make gcc` to install necessary build tools.

- You will also need to install the right gcc toolchain for your system.
    - For 64 bit systems, run `-pacman -S mingw-w64-x86_64-gcc`.
    - For 32 bit systems, run `-pacman -S mingw-w64-i686-gcc`.

## Building
- Close MSYS2 if you have it open and launch it again from the `MSYS2 MinGW 64-bit` start menu shortcut.
    - If you are building on a 32-bit system, use `MSYS2 MinGW 32-bit` instead.

- Optionally, change directory `cd <path>` if you'd like to clone this project elsewhere.
    - Note: Any drive with a letter is accessible under `cd /<drive>/`. For `C:\` it's `cd /c/`.

- Run `git clone https://github.com/BeefaloKing/capy.git`.

- Change into the project root with `cd capy`.

- Run `make release` to compile. When finished, the executable will be located under `./bin/x86_64/release` (or `./bin/i686/release` if you're building for 32-bit).
