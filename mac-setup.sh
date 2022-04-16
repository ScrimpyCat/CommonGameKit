git submodule init
git submodule update

cd deps/zlib
rm -rf build
mkdir build && cd build
cmake -G Xcode ..
sed -i -e 's/SDKROOT/MACOSX_DEPLOYMENT_TARGET = 10.10; SDKROOT/' zlib.xcodeproj/project.pbxproj

cd ../../libpng
cp scripts/pnglibconf.h.prebuilt pnglibconf.h
rm -rf build
mkdir build && cd build
cmake -DPNG_BUILD_ZLIB=ON -DZLIB_INCLUDE_DIR=../zlib -G Xcode ..
sed -i -e 's/"make.*postBuildPhase.*"/""/' libpng.xcodeproj/project.pbxproj
sed -i -e 's/SDKROOT/MACOSX_DEPLOYMENT_TARGET = 10.10; SDKROOT/' libpng.xcodeproj/project.pbxproj
