Blob-Game
=========

Blob Game includes engine and toolset.


Windows
-------

**Requirements:**  
**MinGW** - Download it from http://www.mingw.org (download installer)  
1. Run the installer and then run the package manager and select:
	* mingw-developer-toolkit
	* mingw32-base
	* msys-base
	* mingw-gcc-g++ (needed for ninja bootstrap compilation)
2. Get mingw64 from http://www.drangon.org/mingw/ (e.g. http://www.drangon.org/mirror.wsgi?num=3&fname=mingw-w64-bin-x86_64-20150102.7z)
3. mount C:/mingw64 /mingw

**Ruby** - Download at least 2.1.x from http://rubyinstaller.org/downloads/ (http://dl.bintray.com/oneclick/rubyinstaller/rubyinstaller-2.1.5-x64.exe?direct)  
1. Run the installer (select add Ruby executables to your PATH)

**Ninja** - Download it from https://github.com/martine/ninja  
1. Install Python
	* Download at least 3.x from https://www.python.org/downloads/windows/ (https://www.python.org/ftp/python/3.4.2/python-3.4.2.amd64.msi)
	* Run the installer (select add python.exe to PATH; didn't work for me though, may have to manually add the path `PATH=$PATH:/c/Python34/`)
2. Inside Ninja directory: run `./configure.py --bootstrap --platform=mingw`
3. Add it to path `PATH=$PATH:/c/ninja-master`

**CMake** - Download it from http://www.cmake.org/download/ (http://www.cmake.org/files/v3.1/cmake-3.1.3-win32-x86.exe)  
1. Run the installer (set add CMake to system PATH)

---

**Building:**  
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

**Requirements:**  
**Xcode** - Install it from the Mac App Store  
**CMake** - Simplest way is to just install it through a package manager (homebrew, macports, etc.)  

---

**Building:**  
Build GLFW (/deps/glfw/):
```
mkdir build
cd build
cmake -G Xcode ..
#Gets built from the Xcode project
```

Build Game:
Compile the xcode project (either with xcodebuild or with the IDE itself).
