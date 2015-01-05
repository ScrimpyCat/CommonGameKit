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
sed "s/-I\.\.\/include\/chipmunk/-I..\/include -I..\/include\/chipmunk/g" build.ninja > tmp-build.ninja ; mv tmp-build.ninja build.ninja
ninja
```

Build CommonC (/deps/Common/CommonC/):
```
mkdir build
#Built from our own ninja build file (winbuild-common.ninja)
```


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
CMake - Simplest way is to just install it through a package manager (homebrew, macports, etc.)


Build GLFW (/deps/glfw/):
```
mkdir build
cd build
cmake -G Xcode ..
#Gets built from the Xcode project
```

Build Game:
Compile the xcode project (either with xcodebuild or with the IDE itself).
