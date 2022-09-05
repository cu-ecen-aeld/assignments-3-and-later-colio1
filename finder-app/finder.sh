#!/bin/sh
# Tester script for assignment 1 and assignment 2

if [ $# -lt 2 ]
then
	echo "Error: No arguments"
	exit 1
else
	FILESDIR=$1
	SEARCHSTR=$2
fi


# Searching directory FILESDIR for files containing string SEARCHSTR
NUM_FILES_MATCHED=$(grep -rlc $SEARCHSTR $FILESDIR/* | wc -l)
NUM_FILES_SEARCHED=$(grep -cr $SEARCHSTR $FILESDIR/* | cut -d: -f2 | wc -w)
LINES_PER_FILE_MATCHED=$(grep -cr $SEARCHSTR $FILESDIR/* | cut -d: -f2)
# Increment counter
for i in $(seq 1 2 $((NUM_FILES_SEARCHED*2)))
do
	NUM_LINES=$((NUM_LINES+$(echo $LINES_PER_FILE_MATCHED| cut -c ${i}-$((i+1)))))
done
echo "The number of files are $NUM_FILES_MATCHED and the number of matching lines are $NUM_LINES"
exit 0
