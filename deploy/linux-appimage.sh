#!/bin/bash

mkdir -p ./appdir/usr/bin

cp ../build/src/Release/guu-cpp ./appdir/usr/bin

export NO_STRIP=true
wget "https://github.com/linuxdeploy/linuxdeploy/releases/download/1-alpha-20240109-1/linuxdeploy-x86_64.AppImage"
wget "https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/1-alpha-20240109-1/linuxdeploy-plugin-qt-x86_64.AppImage"
chmod +x ./linuxdeploy-x86_64.AppImage
chmod +x ./linuxdeploy-plugin-qt-x86_64.AppImage

mkdir ./out
cd ./out

../linuxdeploy-x86_64.AppImage --appdir=../appdir -d ../appdir/usr/share/applications/io.github.vancer0.guu.desktop --output appimage --plugin qt

rm ../*.AppImage