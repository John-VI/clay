#!/bin/bash

echo -e "\033[0;35m$0: Copying assets to $MESON_BUILD_ROOT...\033[0m"
cp "$MESON_BUILD_ROOT/../"*.png "$MESON_BUILD_ROOT/../"*.anis "$MESON_BUILD_ROOT/"
