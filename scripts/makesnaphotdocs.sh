#! /bin/bash

if [ -n "$1" ]
then
  version=$1
else
  echo "This script takes one argument- a branch name."
  exit
fi

tempDir="/tmp/cutelee-snapshot"

if [ ! -d "$tempDir" ]
then
  mkdir $tempDir
fi

echo Creating $tempDir/cutelee-$version.tar.gz

git archive --format=tar --prefix=cutelee-$version/ $version | gzip > $tempDir/cutelee-$version.tar.gz

rm -rf $tempDir/cutelee-$version/
echo Unpacking to $tempDir/cutelee-$version
tar -C $tempDir -xvf $tempDir/cutelee-$version.tar.gz

if [ "$?" -ne "0" ]
then
  echo "Error creating tar archive."
  exit
fi

oldDir=$PWD

echo Creating build directory
mkdir -p $tempDir/cutelee-$version/build && cd $tempDir/cutelee-$version/build

cp $oldDir/scripts/qttags.tag $tempDir/cutelee-$version/scripts
cp $oldDir/scripts/libstdc++.tag $tempDir/cutelee-$version/scripts
cp $oldDir/scripts/libstdc++-longnames.tag $tempDir/cutelee-$version/scripts

echo Building
cmake -DCMAKE_INSTALL_PREFIX=../prefix ..

qch_version_string=`echo $version | sed 's/\.//g'`

echo "Creating Docs"
make docs

mkdir doxtmp
mv apidox doxtmp/$version-snapshot
mv doxtmp apidox
tar -czf $oldDir/apidox.tar.gz apidox

cd $oldDir
