#!/bin/bash

cd ../1440p
for filename in *; do
    type=${filename:44:3}
    last=$((${#filename}-10))
    indice=${filename:$last:6}
    new="$type-$indice"
    mod=../video/${new// /-}.mjpg
    jpg=../video/${new// /-}.jpg
    gz=../video/${new// /-}.mjpg.gz

    resize="../video/$filename"
    # Scale
    ffmpeg -i "$filename" -ss 8 -vf scale=426:240 -vcodec libx265 -crf 24 -an -y /tmp/tmp.mp4
    # Crop
    ffmpeg -i /tmp/tmp.mp4 -filter:v "crop=236:240:95:0" -c:a copy -an -y /tmp/crop.mp4
    # Grab miniature jpg
    ffmpeg -i /tmp/crop.mp4 -vf "select=eq(n\,0)" -q:v 3 -y "$jpg"
    # Convert mjpg
    convert=`basename "../video/$filename" .mp4`
    convert="../video/$convert.mjpg"
    ffmpeg -i /tmp/crop.mp4 -ss 1 -vf "fps=12" -q:v 6  -y "$convert"
    # Clean
    rm "$resize"
    mv "$convert" "$mod"
    gzip -c "$mod" > "$gz"
    rm "$mod"
done