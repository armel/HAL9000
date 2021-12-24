#!/bin/bash

cd ../1440p
for filename in *; do
    resize="../video/$filename"
    ffmpeg -i "$filename" -vf scale=320:-1 -crf 1 -y "$resize"
    convert=`basename "../video/$filename" .mp4`
    convert="../video/$convert.mjpg"
    ffmpeg -i "$resize" -vf "fps=10" -q:v 2 -crf 1 -y "$convert"
    rm "$resize"
    type=${filename:44:3}
    last=$((${#filename}-10))
    indice=${filename:$last:6}
    new="$type-$indice"
    mod=../video/${new// /-}.mjpg
    mv "$convert" "$mod"
done

