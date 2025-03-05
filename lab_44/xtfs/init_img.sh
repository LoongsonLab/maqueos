#!/bin/bash

cd bin
./compile.sh xtsh 
./compile.sh print
./compile.sh share
./compile.sh shmem
./compile.sh hello
./compile.sh read
./compile.sh write
./compile.sh create
./compile.sh destroy
./compile.sh sync

mv xtsh ../xtsh 
mv print ../print
mv share ../share
mv shmem ../shmem
mv hello ../hello
mv read ../read
mv write ../write
mv create ../create
mv destroy ../destroy
mv sync ../sync

cd ../

#################
../../cross-tool/bin/loongarch64-unknown-linux-gnu-gcc -nostdinc -c bin/pwar/pwar.S -o pwar.o
../../cross-tool/bin/loongarch64-unknown-linux-gnu-gcc -nostdinc -c bin/pwar/refresh.S -o refresh.o
../../cross-tool/bin/loongarch64-unknown-linux-gnu-gcc -nostdinc -c bin/pwar/plane.S -o plane.o
../../cross-tool/bin/loongarch64-unknown-linux-gnu-gcc -nostdinc -c bin/pwar/bullet.S -o bullet.o
../../cross-tool/bin/loongarch64-unknown-linux-gnu-gcc -nostdinc -c bin/pwar/bullet_create.S -o bullet_create.o

../../cross-tool/bin/loongarch64-unknown-linux-gnu-ld -z max-page-size=4096 -N -e start -Ttext 0 -o pwar.tmp pwar.o plane.o bullet.o bullet_create.o refresh.o
../../cross-tool/bin/loongarch64-unknown-linux-gnu-objcopy -S -O binary pwar.tmp pwar.out

let size=`stat -c %s pwar.out`
let size=($size+0xfff)/0x1000
let count=$size*8+1
dd if=/dev/zero of=pwar bs=512 count=${count} 2> /dev/null
dd if=pwar.out of=pwar bs=512 seek=1 conv=notrunc 2> /dev/null

let size=$size*0x1000
size=`printf "%08x" $size`

for ((i=0;i<4;i++))
do
    length+=`echo ${size:0-2:2}`
    size=`echo ${size%??}`
done

echo -n "xt" > header
echo $length | xxd -p -r >> header
dd if=header of=pwar bs=512 seek=0 conv=notrunc 2> /dev/null

chmod 0777 pwar

#################
dd if=/dev/zero of=xtfs.img bs=512 count=4096 2> /dev/null
./format 
./copy xtsh 1
./copy print 1
./copy share 1
./copy shmem 1
./copy hello 1
./copy read 1
./copy write 1
./copy create 1
./copy destroy 1
./copy sync 1
./copy pwar 1

rm -f xtsh print share shmem hello read write create destroy sync pwar *.o *.tmp *.out *.d header

mv xtfs.img ../run
