#!/bin/bash

cd ../1440p
for filename in *; do
    resize="../video/$filename"
    ffmpeg -i "$filename" -vf scale=-1:128 -crf 1 -y "$resize"
    ffmpeg -i "$resize" -filter:v "crop=130:128:48:0" -c:a copy -an out.mp4
    convert=`basename "../video/$filename" .mp4`
    convert="../video/$convert.mjpg"
    ffmpeg -i out.mp4 -vf "fps=25" -q:v 2 -crf 1 -y "$convert"
    rm "$resize"
    type=${filename:44:3}
    last=$((${#filename}-10))
    indice=${filename:$last:6}
    new="$type-$indice"
    mod=../video/${new// /-}-atom.mjpg
    old=../video/${new// /-}-atom.mp4
    mv "$convert" "$mod"
    mv out.mp4 "$old"
done