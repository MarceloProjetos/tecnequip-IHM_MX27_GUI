#!/bin/sh

[ "$1" = "" ] && exit 1

echo Alterando HostName para $1

tmpfile=`tempfile`

cp /etc/hosts $tmpfile
sed -e "2s/\(1\.1\).*/\1 $1/" $tmpfile > /etc/hosts
rm $tmpfile

echo $1 > /etc/hostname

