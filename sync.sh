#!/usr/bin/env bash
# dpw@plaza.localdomain
# 2025-03-01 19:36:01
#

set -eu

# Define source directories
SRC_DIRS=("include/app" "src" "tests")

# Define the destination directory (change this to your desired destination)
DEST_DIR="dpw@tiburon.local:raincity/c-projects/data-logger"

# Loop through each source directory and sync .hpp and .cpp files
for DIR in "${SRC_DIRS[@]}"; do
    rsync -av --include='*.hpp' --include='*.cpp' --exclude='*' "$DIR/" "$DEST_DIR/$DIR/"
done


