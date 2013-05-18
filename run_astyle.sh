#!/bin/bash
./astyle --style=kr --indent=spaces -n --lineend=linux --exclude=src/core/third_party -r "src/*.h"
./astyle --style=kr --indent=spaces -n --lineend=linux --exclude=src/core/third_party -r "src/*.cpp"
