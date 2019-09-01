#!/bin/bash

VERSION=0.0.1
PACKAGE_NAME=share_mind_$VERSION

function check_error()
{
	if [ "$?" != "0" ];then
		echo $1 fail!
		exit 1
	fi
}

if [ -d $PACKAGE_NAME ];then
	rm -r $PACKAGE_NAME
fi
mkdir -p $PACKAGE_NAME/bin
mkdir -p $PACKAGE_NAME/config

echo "Install dependency ...."
sudo apt-get install libmysqlclient-dev

echo "building server...."
cd server/build
make clean
make -j4
check_error "build server"
cp book-share-mind ../../$PACKAGE_NAME/bin

echo "building book download"
cd ../../book-download/build
make clean
make -j4
check_error "build book download"
cp book-download ../../$PACKAGE_NAME/bin

cd ../../

cp cfg $PACKAGE_NAME/config
cp install.sh $PACKAGE_NAME
cp -R resources/db $PACKAGE_NAME
cp -R resources/zips $PACKAGE_NAME

tar czvf $PACKAGE_NAME.tar.gz $PACKAGE_NAME

