Blob-Game
=========

Blob Game includes engine and toolset.


Windows
-------

*Todo:* Add instructions for installing all required tools



Build GLFW (/deps/glfw/):
```
mkdir build
cd build
cmake -G Ninja ..
ninja
```

Build Chipmunk2D (/deps/Chipmunk2D/):
```
mkdir build
cd build
cmake -G Ninja -DBUILD_DEMOS=OFF ..
sed "s/-I\.\.\/include\/chipmunk/-I..\/include -I..\/include\/chipmunk/g" build.ninja
ninja
```

Build Common (/deps/Common/):
*Todo*

Build Game:
```
mkdir build
ninja -f winbuild.ninja
```


Linux
-----

*Todo*


Mac
---

Requirements:
Xcode - Install it from the Mac App Store


Build Game:
Compile the xcode project (either with xcodebuild or with the IDE itself).
