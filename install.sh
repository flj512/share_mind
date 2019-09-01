#! /bin/bash


function check_error()
{
	if [ "$?" != "0" ];then
		echo $1 fail!
		exit 1
	fi
}

user_name=$(sed -n '3p' config/cfg)

echo "create daemon user..."
if [ "$user_name" == "" ];then
	echo "user_name is empty,installation stopped"
	exit 1
fi

id -u $user_name >/dev/null 2>&1
if [ ! "$?" == "0" ];then
	sudo useradd -d /noexist -s /bin/false $user_name
	check_error "create user fail!!!!"
fi

echo "install server..."
INSTALL_PATH=/opt/share_mind
sudo killall -e $INSTALL_PATH/bin/book-download 1>/dev/null 2>&1
sudo killall -e $INSTALL_PATH/bin/book-share-mind 1>/dev/null 2>&1
sudo rm -r /opt/share_mind
sudo mkdir -p $INSTALL_PATH
sudo cp -R bin $INSTALL_PATH
sudo cp -R config $INSTALL_PATH

sudo chown $user_name:$user_name $INSTALL_PATH/config/cfg
sudo chmod 400 $INSTALL_PATH/config/cfg

echo "copy books..."
BOOK_PATH=/var/books/
if [ ! -d $BOOK_PATH ];then
	sudo mkdir -p $BOOK_PATH
	sudo chown $user_name:$user_name $BOOK_PATH
fi
sudo cp zips/* $BOOK_PATH
sudo chown -R $user_name:$user_name $BOOK_PATH
sudo chmod 700 $BOOK_PATH

echo "start server ..."
LOG_PATH=/var/log/book-share-mind
if [ ! -d $LOG_PATH ];then
	sudo mkdir -p $LOG_PATH
	sudo chown -R $user_name:$user_name $LOG_PATH
fi
sudo $INSTALL_PATH/bin/book-share-mind
sudo $INSTALL_PATH/bin/book-download

