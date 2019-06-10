#!/bin/bash
# Original Author: Erhard List (dunkanx@pokerth.net) Version: 1.0
# Updated by Lothar May

# ATTENTION: this is for MAC only!
#
# This script integrates the necessary frameworks into the binary.
#
# The integration of Qt is optional (commandlineswitch --without-qt) as you can save 30MB 
# of binary-size if you leave Qt out.
# Using such a "small" version needs to install qt 4.2.3 on the mac seperatly as a framework 
# (see http://trolltech.com/developer/downloads/qt/mac)

SDL_FW_PATH="/Library/Frameworks"
APPLICATION="./qupil.app"
BINARY="$APPLICATION/Contents/MacOs/qupil"
RESOURCES="$APPLICATION/Contents/Resources/"

# strip binary
strip $BINARY

cp -R ./data $RESOURCES
# create framework-path
BINARY_FW_PATH="$APPLICATION/Contents/Frameworks"
mkdir $BINARY_FW_PATH

# integrate SDL-frameworks into binary
cp -R $SDL_FW_PATH/SDL.framework $BINARY_FW_PATH
cp -R $SDL_FW_PATH/SDL_mixer.framework $BINARY_FW_PATH

# integrate Qt-frameworks into binary
macdeployqt $APPLICATION -dmg

