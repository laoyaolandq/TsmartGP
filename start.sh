#!/bin/bash
configFile="config/phase/yxpointer2/gated-bound.properties"
TsmartGPRoot="/home/landq/Desktop/pointer/TsmartGP/"
analyzedRoot="/home/landq/Desktop/pointer/TsmartGP/project/"
outputPath="/home/landq/Desktop/results/"
if [ ! -d $outputPath ]; then
	mkdir -p $outputPath
else
	rm -r $outputPath
	mkdir -p $outputPath
fi
echo "BuildCapture Phase"
cp TsmartBuild.jar $analyzedRoot
cd $analyzedRoot
make clean
rm -r ".process_makefile/"
java -jar TsmartBuild.jar make
cd $TsmartGPRoot
echo "Finish"
echo "Analyze Phase"
java -jar TsmartAnalyze.jar -config=$configFile -captured=$analyzedRoot".process_makefile/" -output=$outputPath
echo "Finish"

cd "visualizer/"
java -jar bug-visualizer.jar -report=$outputPath"result.xml" -project=$analyzedRoot -web="webInspector/template/"
