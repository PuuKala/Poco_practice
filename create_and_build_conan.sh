echo -- CREATING BUILD FOLDER --
mkdir -p build
cd build
echo -- FETCHING CONAN ARTIFACTS --
conan install .. --build=missing
echo -- CREATING PROJECT FILES --
cmake ..
echo -- BUILDING RELEASE CONFIGURATION --
cmake --build . -- -j
echo -- DONE --
cd ..
exit 0
