## Build/Install Commands

```sh
mkdir <build directory>
cd <build directory>
cmake <path to the directory of top-level CMakeLists.txt>
# build
cmake --build .

# change top-level CMakeLists.txt, to rebuild:
cmake --build .
# Or changes need to clear the cache:
rm CMakeCache.txt
cmake <path to the directory of top-level CMakeLists.txt>
cmake --build .

# install (only after build)
cmake --install . --prefix <destination of install>
```

## `PUBLIC`|`PRIVATE`|`INTERFACE`

### Example: `target_include_directories()`
- `PUBLIC`: When you specify an include directory as `PUBLIC`, it means two things:
    - The specified directory will be added to the include path for the target itself.
    - The directory will also be added to the include path for any other targets that link against this target. In other words, the include directories are propagated to dependent targets.
- `PRIVATE`: When you specify an include directory as `PRIVATE`, it means:
    - The specified directory will be added to the include path for the target itself.
    - However, it will not be added to the include path for targets that link against this target. So, the include directories are not propagated to dependent targets.
- `INTERFACE`: This keyword is used when you don't need to add the directories to the target itself, but you want them to be used by targets that link against this target.

## Build Tree vs Install Tree

### Build Tree

The **build tree** refers to the directory where you compile and build your project. In the case of `MyLibrary`, this is where you run your CMake commands and where the compiled library and intermediate files (like object files) are stored. It's essentially the workspace where the source code is turned into an executable library.

- **Location**: It's typically a separate directory from your source code, often called `build` or `bin`, created inside your project folder. For instance, you might create it at `MyLibrary/build/`.

- **Contents**: The build tree contains:
    - CMake generated files (like `Makefile`, `CMakeCache.txt`, etc.).
    - Compiled object files and the final library binary (`MyLibrary.a` or `MyLibrary.so`/`MyLibrary.dll`).
    - Any other files generated during the build process.

- **Usage**: This is where you would run commands like `cmake ..` (to configure the project), `make` (to build the project), and possibly run tests or execute the library if it's an executable.

### Install Tree

The **install tree** is the directory structure of the installed version of your project. This is what you would distribute to users or deploy. It's a cleaner version of your project, containing only what's necessary to use the library (e.g., library binaries, headers, configuration files) and excluding build-specific files.

- **Location**: It's not a fixed location within the project directory. Instead, it's determined by the installation path you specify (like `/usr/local` on Unix systems or a directory chosen by the user).

- **Contents**: The install tree contains:
    - The final library binaries (`.so`, `.dll`, or `.a` files).
    - The public header files (from the `include/` directory in the source tree).
    - Any other files you specify to be installed (like configuration files, documentation, etc.).

- **Usage**: This is what you use when you deploy the library for others to use, or when you install it on your system for development of other projects that depend on `MyLibrary`.

### In the CMake Configuration

In your `CMakeLists.txt`:

- The `BUILD_INTERFACE` generator expressions apply settings (like include paths and compile options) that are only relevant during the building and developing within the build tree.
- The `INSTALL_INTERFACE` generator expressions apply settings that are relevant when the library is installed and used elsewhere.

This separation allows for different configurations and setups for development (build tree) versus deployment and distribution (install tree).
