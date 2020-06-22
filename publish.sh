#!/bin/bash
folder=FestivalSimulator_artists

rm -r ${folder}/*
cp -r objects ${folder}
cp -r samples ${folder}
cp -r songs ${folder}
cp -r tools ${folder}
cp FestivalSim* ${folder}
cp *.dll ${folder}
cp *.bmp ${folder}
cp *.ttf ${folder}
cp LICENCE* ${folder}
cp README.txt ${folder}

zip -r ${folder} ${folder}/*
mv ${folder}.zip ~/Dropbox/Public/

