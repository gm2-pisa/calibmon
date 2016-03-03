## compile

cd build
cmake ..
make

## execute

build/calibmonrun

generated histograms are served on port 9090 and can be examined by
ROODY as follows

cd roody
bin/roody -H localhost

quit by pressing '!'

## cleanup

rm -rf build/*
