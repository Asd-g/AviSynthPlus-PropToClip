## Description

Extracts a clip from the frames attached to the frame property prop in clip. This function is mainly used to extract a mask/alpha clip that was stored in another one.

This is a port of the VapourSynth PropToClip.

### Requirements:

- AviSynth+ 3.6 or later

- Microsoft VisualC++ Redistributable Package 2022 (can be downloaded from [here](https://github.com/abbodi1406/vcredist/releases))

### Usage:

```
PropToClip (clip input, string "prop", string "pixel_type")
```

### Parameters:

- input\
    Clip with the frames attached to frame property.
        
- prop\
    Name of the frame property.\
    Default: "_Alpha".
    
- pixel_type\
    Pixel type of the atached frames.\
    Default: "Y8".

### Building:

- Windows\
    Use solution files.

- Linux
    ```
    Requirements:
        - Git
        - C++14 compiler
        - CMake >= 3.16
    ```
    ```
    git clone https://github.com/Asd-g/AviSynthPlus-PropToClip && \
    cd AviSynthPlus-PropToClip && \
    mkdir build && \
    cd build && \
    
    cmake ..
    make -j$(nproc)
    sudo make install
    ```
