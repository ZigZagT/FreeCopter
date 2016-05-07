DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd $DIR/..
echo "project directory: `pwd`"
PROJECT_DIR="`pwd`"

if [ ! -d build ]; then
    echo "create build directory to store output files."
    mkdir build
fi

printf "\n"
echo "detect source: "
SOURCE_C=$(find src/controller/device/rpi2b | grep '.c$')
SOURCE_CPP=$(find src/controller/device/rpi2b | grep '.cpp$')
printf '%s\n' "${SOURCE_C[@]}"
printf '%s\n' "${SOURCE_CPP[@]}"
printf "\n"

echo "use include directory: "
INCLUDE="$PROJECT_DIR/src/include $PROJECT_DIR/src/controller/wcp $PROJECT_DIR/src/controller/device/rpi2b"
printf '%s\n' "${INCLUDE[@]}"
printf "\n"
BUILD="gcc -o build/simpletest"
for d in $INCLUDE; do
    BUILD="$BUILD -I $d"
done
for f in $SOURCE_C $SOURCE_CPP; do
    BUILD="$BUILD $f"
done
printf "build with command: \n%s\n\n" "$BUILD"
$BUILD
