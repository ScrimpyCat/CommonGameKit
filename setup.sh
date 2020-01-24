git submodule update

cd deps/zlib
rm -rf build
mkdir build && cd build
cmake -G Ninja ..
ninja

cd ../../libpng
cp scripts/pnglibconf.h.prebuilt pnglibconf.h
rm -rf build
mkdir build && cd build
cmake -DPNG_SHARED=OFF -DPNG_TESTS=OFF -DPNG_BUILD_ZLIB=ON -DZLIB_INCLUDE_DIR=../zlib -G Ninja ..
ninja

cd ../../../
rm -rf  build
ruby build.rb \
--library=deps/CommonC/CommonC,deps/CommonC \
--static=deps/libpng/build/libpng.a,deps/libpng \
--header=deps/stdatomic \
--header=deps/threads \
--library=deps/tinycthread/source \
--static=deps/zlib/build/libz.a,deps/libz \
--lib=opengl \
--platform=mac \
--library=deps/Common/CommonObjc/CommonObjc,deps/Common/CommonObjc \
--lib=apple \
--lib=metal \
--platform=win \
--library=deps/GL
cd build
ninja
