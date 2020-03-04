#!/bin/bash

if ! [ -x "$(command -v ffmpeg)" ]; then
  echo 'Error: ffmpeg is not installed.' >&2
  exit 1
fi

for i in $(find . -name "*.ogg" -type f); do
    echo "$i"
    ffmpeg -n -i "$i" "${i%.*}.wav"
done