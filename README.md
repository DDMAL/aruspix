# Aruspix #

Aruspix is software application for the optical recognition, the superimposition and the collation of early music prints. More information about the project is available at [http://www.aruspix.net](http://www.aruspix.net). 

The [Wiki](https://github.com/DDMAL/aruspix/wiki) of this repository gives more information on how to build Aruspix.

## Building on Windows

Aruspix is built as 64-bit on Windows. The dependencies Conan resolves are `x86_64`, so the aruspix target must be compiled from an x64 MSVC environment or the final link will fail with `LNK4272` warnings and unresolved-symbol errors.

### First-time setup (per machine)

Open the **x64 Native Tools Command Prompt for VS 2022** (or run `vcvarsall.bat x64` in a fresh shell), then:

```
conan profile detect --force
echo core.sources:download_urls=["https://c3i.jfrog.io/artifactory/conan-center-backup-sources/", "origin"]>> %USERPROFILE%\.conan2\global.conf
```

The `global.conf` line tells Conan to fall back to Conan Center's backup mirror when a recipe's upstream source URL is unreachable — needed because chronically flaky hosts like `tukaani.org` (xz) will otherwise fail the build.

### Every build

```
conan export recipes/im
conan install . --build=missing -c tools.cmake.cmaketoolchain:generator=Ninja
build\Release\generators\conanbuild.bat
cmake --preset conan-release
cmake --build build/Release
```

`conanbuild.bat` sets vcvars for the arch Conan picked, so subsequent cmake invocations use the x64 toolchain regardless of the ambient shell.

If `conan install` complains that `Windows builds require arch=x86_64`, you are in an x86 shell — reopen the x64 prompt or pass `-s arch=x86_64 -s:b arch=x86_64` to `conan install`.
