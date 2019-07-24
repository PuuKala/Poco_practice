echo -- CREATING BUILD FOLDER --
mkdir -p build
cd build
echo -- FETCHING CONAN ARTIFACTS --
conan install .. -s compiler.libcxx="libstdc++11" --build=missing
echo -- CREATING PROJECT FILES --
cmake -DCMAKE_BUILD_TYPE=Release -G "Unix Makefiles" ..
echo -- BUILDING RELEASE CONFIGURATION --
cmake --build . -- -j3
echo -- DONE --
cd ..
exit 0
