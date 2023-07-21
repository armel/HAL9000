#!/bin/bash

cd ../1440p
for filename in *; do
    resize="../video/$filename"
    ffmpeg -i "$filename" -ss 8 -vf scale=426:240 -vcodec libx265 -crf 24 -an -y /tmp/tmp.mp4
    ffmpeg -i /tmp/tmp.mp4 -filter:v "crop=236:240:95:0" -c:a copy -an -y /tmp/crop.mp4
    convert=`basename "../video/$filename" .mp4`
    convert="../video/$convert.mjpg"
    ffmpeg -i /tmp/crop.mp4 -vf "fps=12" -q:v 8  -y "$convert"
    rm "$resize"
    type=${filename:44:3}
    last=$((${#filename}-10))
    indice=${filename:$last:6}
    new="$type-$indice"
    mod=../video/${new// /-}-medium.mjpg
    jpg=../video/${new// /-}-medium.jpg
    gz=../video/${new// /-}-medium.mjpg.gz
    mv "$convert" "$mod"
    convert "$mod" "$jpg"
    gzip -c "$mod" > "$gz"
    rm "$mod"
done