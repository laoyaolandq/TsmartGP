#!/bin/bash
echo "Hello World !"
dirpath="/home/landq/Desktop/files/part"


p1=("-1" "-5" "-9")
p2=("-p 1" "-p 4" "-p 8")
p3=(" " "-c")
p4=(" " "-i")
p5=(" " "-K")
p6=(" " "-q")
p7=(" " "-R")
p8=(" " "-v")
p9=(" " "-z")

count=0
for i1 in 0 1 2
do
   for i2 in 0 1 2
do
   for i3 in 0 1
do
   for i4 in 0 1
do
   for i5 in 0 1
do
   for i6 in 0 1
do
   for i7 in 0 1
do
   for i8 in 0 1
do
   for i9 in 0 1
do

((count++))
#if [ $((count%2)) = 0 ]
#then
#kill 0
#fi
curcommand='-rf '${p1[$i1]}' '${p2[$i2]}' '${p3[$i3]}' '${p4[$i4]}' '${p5[$i5]}' '${p6[$i6]}' '${p7[$i7]}' '${p8[$i8]}' '${p9[$i9]}' '${dirpath}
config=${i1}" "${i2}" "${i3}" "${i4}" "${i5}" "${i6}" "${i7}" "${i8}" "${i9}" "
echo -e $config >> result.txt
echo -e $curcommand >> commands.txt
begin_time=`date +%s`
./pigz $curcommand
end_time=`date +%s`
echo compression
echo $((10#$end_time-10#$begin_time))
echo -e $((10#$end_time-10#$begin_time)) >> result.txt

dpa='-rd '${dirpath}
dbegin_time=`date +%s` 
echo decompression
echo $dbegin_time
./pigz $dpa
dend_time=`date +%s`
echo $dend_time
echo $((10#$dend_time-10#$dbegin_time))

begin_time=`date +%s`
./pigz $curcommand
end_time=`date +%s`
echo compression
echo $((10#$end_time-10#$begin_time))
echo -e $((10#$end_time-10#$begin_time)) >> result.txt

dpa='-rd '${dirpath}
dbegin_time=`date +%s` 
echo decompression
echo $dbegin_time
./pigz $dpa
dend_time=`date +%s`
echo $dend_time
echo $((10#$dend_time-10#$dbegin_time))

begin_time=`date +%s`
./pigz $curcommand
end_time=`date +%s`
echo compression
echo $((10#$end_time-10#$begin_time))
echo -e $((10#$end_time-10#$begin_time)) >> result.txt

dpa='-rd '${dirpath}
dbegin_time=`date +%s` 
echo decompression
echo $dbegin_time
./pigz $dpa
dend_time=`date +%s`
echo $dend_time
echo $((10#$dend_time-10#$dbegin_time))

echo -e "\n" >> result.txt

echo -e $count

done
done
done
done
done
done
done
done
done







echo "all tasks have been done."
