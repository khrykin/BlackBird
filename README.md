# BlackBird

### [Website](https://khrykin.github.io/BlackBird) · [Sample Sounds](https://soundcloud.com/khrykin/sets/BlackBird)

Simple, yet peculiar VST3/AU Synth

![Plugin window screenshot](https://raw.githubusercontent.com/khrykin/BlackBird/master/Promo/Screenshot.png)

## Contributing

> [!NOTE]
> The development environment is optimized only for macOS for the time being.

### Prerequisites

- A C++17-compliant compiler
- [CMake](https://cmake.org/) (See [CMakeLists.txt](./CMakeLists.txt#L1) for the recommended version)
- [Ninja](https://ninja-build.org/) _(recommended)_
- [clang-format](https://clang.llvm.org/docs/ClangFormat.html)

  - Install the pre-commit hook for formatting:
    ```
    cp ./pre-commit .git/hooks/pre-commit
    ```
  - Setup auto-format on save in your IDE (make sure it uses `clang-format`) _(recommended)_

- Download and extract [JUCE@7.10.0](https://github.com/juce-framework/JUCE/releases/tag/7.0.10) into `./JUCE` in the repo root

To be able to conveniently test the audio output during development (e.g. check the spectrum of the audio), you can route the audio from the standalone app into a DAW of your choice. Click Options -> Audio/MIDI Settings in the standalone app's UI to set up the audio output. E.g. on macOS, you can use
[BlackHole](https://existential.audio/blackhole/) to route the audio to Logic Pro.

### Building

Prepare the build system with:

```
mkdir Build
cmake -B Build -G Ninja
```

Then to build:

```
cmake --build Build
```

To conveniently build and restart the standalone app on macOS:

```
./macos_rerun
```
