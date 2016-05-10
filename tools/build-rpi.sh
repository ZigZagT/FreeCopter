DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd $DIR/..
PROJECT_DIR="`pwd`"

if [ ! -d build ]; then
    mkdir build
else
    rm -rf build
    mkdir build
fi

DIR="$PROJECT_DIR/src/controller/device/rpi2b"

printf "\ndetect source: \n"
SOURCE_C=$(find $DIR -name "*.c")
SOURCE_CPP=$(find $DIR -name "*.cpp")
printf '%s\n' "${SOURCE_C[@]}"
printf '%s\n' "${SOURCE_CPP[@]}"

printf "\nuse include directory: \n"
INCLUDE="$PROJECT_DIR/src/include  $PROJECT_DIR/src/controller/wcp   $PROJECT_DIR/src/controller/device/rpi2b"
INCLUDE=($INCLUDE)

printf '%s\n' "${INCLUDE[@]}"
printf "\n"

BUILD="gcc -c -std=gnu99"
for d in "${INCLUDE[@]}"; do
    BUILD="$BUILD -I$d"
done
for f in $SOURCE_C; do
    printf "build $(basename -s .c $f).o\n"
    $BUILD -o build/$(basename -s .c $f).o $f
done

BUILD="gcc -c -std=c++11"
for d in "${INCLUDE[@]}"; do
    BUILD="$BUILD -I$d"
done
for f in $SOURCE_CPP; do
    printf "build $(basename -s .cpp $f).o\n"
    $BUILD -o build/$(basename -s .cpp $f).o $f
done
printf "\n"

TARGET=""
SOURCE_C=$(find $DIR/test -name "*.c")
for f in $SOURCE_C; do
    TARGET="$TARGET `basename -s .c $f`"
done
SOURCE_CPP=$(find $DIR/test -name "*.cpp")
for f in $SOURCE_CPP; do
    TARGET="$TARGET `basename -s .cpp $f`"
done

SOURCE=""
SOURCE_C=$(find $DIR -name "*.c" -not -path "$DIR/test/*")
for f in $SOURCE_C; do
    SOURCE="$SOURCE `basename -s .c $f`"
done
SOURCE_CPP=$(find $DIR -name "*.cpp" -not -path "$DIR/test/*")
for f in $SOURCE_CPP; do
    SOURCE="$SOURCE `basename -s .cpp $f`"
done

BUILD="gcc -lstdc++ -lZTunnel -lncurses"
for f in $SOURCE; do
    BUILD="$BUILD build/$f.o"
done
for f in $TARGET; do
    printf "build target %s\n" "$f";
    $BUILD build/$f.o -o build/$f
done
