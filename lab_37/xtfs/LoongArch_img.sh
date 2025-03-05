#!/bin/sh

echo -n "I Love LoongArch # " > temp

dd if=/dev/zero of=xtfs.img bs=512 count=2048 2> /dev/null

dd if=temp of=xtfs.img conv=notrunc 2> /dev/null

mv xtfs.img ../run

rm temp
