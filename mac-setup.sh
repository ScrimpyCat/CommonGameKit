cd deps/zlib
rm -rf build
mkdir build && cd build
cmake -G Xcode ..
cd ../../libpng
cp scripts/pnglibconf.h.prebuilt pnglibconf.h
rm -rf build
mkdir build && cd build
cmake -DPNG_BUILD_ZLIB=ON -DZLIB_INCLUDE_DIR=../zlib -G Xcode ..
sed -i -e 's/"make.*postBuildPhase.*"/""/' libpng.xcodeproj/project.pbxproj
