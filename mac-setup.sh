git submodule init
git submodule update

cd deps/zlib
rm -rf build
mkdir build && cd build
cmake -G Xcode ..
sed -i '' -e 's/SDKROOT[^;]*/MACOSX_DEPLOYMENT_TARGET = "$(RECOMMENDED_MACOSX_DEPLOYMENT_TARGET)"; SDKROOT = macosx/' zlib.xcodeproj/project.pbxproj

# Universal builds
sed -i '' -e 's/ARCHS = "$(NATIVE_ARCH_ACTUAL)";/ARCHS = "$(ARCHS_STANDARD)";/' zlib.xcodeproj/project.pbxproj
sed -i '' -e ':a' -e 'N' -e '$!ba' -e 's/\(\/\* Release \*\/ = {\n[[:space:]]*isa = XCBuildConfiguration;\n[[:space:]]*buildSettings = {\n[[:space:]]*ARCHS = "$(ARCHS_STANDARD)";\n[[:space:]]*ONLY_ACTIVE_ARCH\) = YES;/\1 = NO;/' zlib.xcodeproj/project.pbxproj

cd ../../libpng
cp scripts/pnglibconf.h.prebuilt pnglibconf.h
rm -rf build
mkdir build && cd build
cmake -DZLIB_ROOT=../../zlib -DZLIB_INCLUDE_DIR=../../zlib -G Xcode ..
sed -i '' -e 's/"make.*postBuildPhase.*"/""/' libpng.xcodeproj/project.pbxproj
sed -i '' -e 's/SDKROOT[^;]*/MACOSX_DEPLOYMENT_TARGET = "$(RECOMMENDED_MACOSX_DEPLOYMENT_TARGET)"; SDKROOT = macosx/' libpng.xcodeproj/project.pbxproj

# Universal builds
sed -i '' -e 's/ARCHS = "$(NATIVE_ARCH_ACTUAL)";/ARCHS = "$(ARCHS_STANDARD)";/' libpng.xcodeproj/project.pbxproj
sed -i '' -e ':a' -e 'N' -e '$!ba' -e 's/\(\/\* Release \*\/ = {\n[[:space:]]*isa = XCBuildConfiguration;\n[[:space:]]*buildSettings = {\n[[:space:]]*ARCHS = "$(ARCHS_STANDARD)";\n[[:space:]]*ONLY_ACTIVE_ARCH\) = YES;/\1 = NO;/' libpng.xcodeproj/project.pbxproj

# Change debug lib name
sed -i '' -e 's/libpng16d.a/libpng16.a/' libpng.xcodeproj/project.pbxproj

static_debug_config=$(sed -e ':a' -e 'N' -e '$!ba' -e 's/.*PBXNativeTarget "png_static" \*\/ = {\n[[:space:]]*isa = XCConfigurationList;\n[[:space:]]*buildConfigurations = (\n\([^;]*\).*/\1/' libpng.xcodeproj/project.pbxproj | grep Debug | sed s/.*\\\([0-9A-F]\\{24\\}\\\).*/\\1/)
sed -i '' -e ':a' -e 'N' -e '$!ba' -e "s/\($static_debug_config \/\* Debug \*\/ = {\n[[:space:]]*isa = XCBuildConfiguration;\n[[:space:]]*buildSettings = {[^}]*PRODUCT_NAME\) = png16d;/\1 = png16;/" libpng.xcodeproj/project.pbxproj

# Fix NEON link errors and rename
sed -i '' -e 's/\/\* End PBXBuildFile section \*\//F3A49E632D82D58600DC1D15 \/* palette_neon_intrinsics.c in Sources *\/ = {isa = PBXBuildFile; fileRef = F3A49E5F2D82D58500DC1D15 \/* palette_neon_intrinsics.c *\/; }; F3A49E642D82D58600DC1D15 \/* filter_neon.S in Sources *\/ = {isa = PBXBuildFile; fileRef = F3A49E602D82D58500DC1D15 \/* filter_neon.S *\/; }; F3A49E652D82D58600DC1D15 \/* arm_init.c in Sources *\/ = {isa = PBXBuildFile; fileRef = F3A49E612D82D58500DC1D15 \/* arm_init.c *\/; }; F3A49E662D82D58600DC1D15 \/* filter_neon_intrinsics.c in Sources *\/ = {isa = PBXBuildFile; fileRef = F3A49E622D82D58600DC1D15 \/* filter_neon_intrinsics.c *\/; }; \/* End PBXBuildFile section *\//' libpng.xcodeproj/project.pbxproj

sed -i '' -e 's/\/\* End PBXFileReference section \*\//F3A49E5F2D82D58500DC1D15 \/* palette_neon_intrinsics.c *\/ = {isa = PBXFileReference; lastKnownFileType = sourcecode.c.c; name = palette_neon_intrinsics.c; path = arm\/palette_neon_intrinsics.c; sourceTree = "<group>"; }; F3A49E602D82D58500DC1D15 \/* filter_neon.S *\/ = {isa = PBXFileReference; lastKnownFileType = sourcecode.asm; name = filter_neon.S; path = arm\/filter_neon.S; sourceTree = "<group>"; }; F3A49E612D82D58500DC1D15 \/* arm_init.c *\/ = {isa = PBXFileReference; lastKnownFileType = sourcecode.c.c; name = arm_init.c; path = arm\/arm_init.c; sourceTree = "<group>"; }; F3A49E622D82D58600DC1D15 \/* filter_neon_intrinsics.c *\/ = {isa = PBXFileReference; lastKnownFileType = sourcecode.c.c; name = filter_neon_intrinsics.c; path = arm\/filter_neon_intrinsics.c; sourceTree = "<group>"; }; \/* End PBXFileReference section *\//' libpng.xcodeproj/project.pbxproj

sed -i '' -e ':a' -e 'N' -e '$!ba' -e "s/\($(grep mainGroup libpng.xcodeproj/project.pbxproj | sed s/.*\\\([0-9A-F]\\{24\\}\\\).*/\\1/) = {\n[[:space:]]*isa = PBXGroup;\n[[:space:]]*children = (\)/\1 F3A49E612D82D58500DC1D15 \/* arm_init.c *\/, F3A49E622D82D58600DC1D15 \/* filter_neon_intrinsics.c *\/, F3A49E602D82D58500DC1D15 \/* filter_neon.S *\/, F3A49E5F2D82D58500DC1D15 \/* palette_neon_intrinsics.c *\/,/" libpng.xcodeproj/project.pbxproj

static_sources=$(sed -e ':a' -e 'N' -e '$!ba' -e 's/.*PBXNativeTarget "png_static" \*\/;\n[[:space:]]*buildPhases = (\n[[:space:]]*\([^;]*\).*/\1/' libpng.xcodeproj/project.pbxproj | grep Sources | sed s/.*\\\([0-9A-F]\\{24\\}\\\).*/\\1/)
sed -i '' -e ':a' -e 'N' -e '$!ba' -e "s/\($static_sources \/\* Sources \*\/ = {\n[[:space:]]*isa = PBXSourcesBuildPhase;\n[[:space:]]*buildActionMask = 2147483647;\n[[:space:]]*files = (\n\)/\1F3A49E632D82D58600DC1D15 \/* palette_neon_intrinsics.c in Sources *\/, F3A49E642D82D58600DC1D15 \/* filter_neon.S in Sources *\/, F3A49E652D82D58600DC1D15 \/* arm_init.c in Sources *\/, F3A49E662D82D58600DC1D15 \/* filter_neon_intrinsics.c in Sources *\/, /" libpng.xcodeproj/project.pbxproj

shared_sources=$(sed -e ':a' -e 'N' -e '$!ba' -e 's/.*PBXNativeTarget "png_shared" \*\/;\n[[:space:]]*buildPhases = (\n[[:space:]]*\([^;]*\).*/\1/' libpng.xcodeproj/project.pbxproj | grep Sources | sed s/.*\\\([0-9A-F]\\{24\\}\\\).*/\\1/)
sed -i '' -e ':a' -e 'N' -e '$!ba' -e "s/\($shared_sources \/\* Sources \*\/ = {\n[[:space:]]*isa = PBXSourcesBuildPhase;\n[[:space:]]*buildActionMask = 2147483647;\n[[:space:]]*files = (\n\)/\1F3A49E632D82D58600DC1D15 \/* palette_neon_intrinsics.c in Sources *\/, F3A49E642D82D58600DC1D15 \/* filter_neon.S in Sources *\/, F3A49E652D82D58600DC1D15 \/* arm_init.c in Sources *\/, F3A49E662D82D58600DC1D15 \/* filter_neon_intrinsics.c in Sources *\/, /" libpng.xcodeproj/project.pbxproj

framework_sources=$(sed -e ':a' -e 'N' -e '$!ba' -e 's/.*PBXNativeTarget "png_framework" \*\/;\n[[:space:]]*buildPhases = (\n[[:space:]]*\([^;]*\).*/\1/' libpng.xcodeproj/project.pbxproj | grep Sources | sed s/.*\\\([0-9A-F]\\{24\\}\\\).*/\\1/)
sed -i '' -e ':a' -e 'N' -e '$!ba' -e "s/\($framework_sources \/\* Sources \*\/ = {\n[[:space:]]*isa = PBXSourcesBuildPhase;\n[[:space:]]*buildActionMask = 2147483647;\n[[:space:]]*files = (\n\)/\1F3A49E632D82D58600DC1D15 \/* palette_neon_intrinsics.c in Sources *\/, F3A49E642D82D58600DC1D15 \/* filter_neon.S in Sources *\/, F3A49E652D82D58600DC1D15 \/* arm_init.c in Sources *\/, F3A49E662D82D58600DC1D15 \/* filter_neon_intrinsics.c in Sources *\/, /" libpng.xcodeproj/project.pbxproj
