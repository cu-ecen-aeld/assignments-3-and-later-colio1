#!/bin/sh
# Tester script for assignment 1 and assignment 2

if [ $# -lt 2 ]
then
	echo "Error: No arguments"
	exit 1
else
	WRITEFILE=$1
	WRITESTR=$2
fi

#Create file with string
DIR="$(dirname "${WRITEFILE}")"
if [ -d "$DIR" ] 
then
	echo $(echo $WRITESTR > $WRITEFILE)
else
	echo $(mkdir -p $DIR)
	echo $(echo $WRITESTR > $WRITEFILE)
fi
exit 0