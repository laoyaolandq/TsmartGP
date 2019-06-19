pa="-11 -p 8  -r /home/landq/Desktop/files/all"
begin_time=`date +%s` 
echo compression
echo $begin_time
#./pigz $pa
end_time=`date +%s`
echo $end_time
echo $((10#$end_time-10#$begin_time))

dpa="-rd /home/landq/Desktop/files/all"
dbegin_time=`date +%s` 
echo decompression
echo $dbegin_time
./pigz $dpa
dend_time=`date +%s`
echo $dend_time
echo $((10#$dend_time-10#$dbegin_time))
