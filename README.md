# Z-Flat ![cmake test](https://img.shields.io/github/workflow/status/adamhutchings/zflat/cmake?style=for-the-badge) <a href="https://discord.gg/UPqeyvzPcq">![discord](https://img.shields.io/badge/Discord-7289DA?style=for-the-badge&logo=discord&logoColor=white)</a>
![logo](https://github.com/adamhutchings/zflat/blob/main/logo.png)


## Overview

Z-flat is a compiled, statically-typed programming language. Its design goals
are intuitive syntax, type-safety, and easy library management.

![screenshot](https://github.com/adamhutchings/zflat/blob/main/screenshot.png)

## Syntax highlighting
#### Vim
- [JakeRoggenbuck/zflat.vim](https://github.com/JakeRoggenbuck/zflat.vim/)

#### Others
- Please help us grow the zflat community and add support for your favorite tools

## Building
- run `./scripts/build.sh` to start the build process

## Running
- run the compiler at `./cmake/czfc`
- see instructions at [docs/compiler.md](https://github.com/adamhutchings/zflat/blob/main/docs/compiler.md)

## Compiler Test
- `./cmake/czfc input/file.zf`
- compiles to `input/file.o` (in the future)
