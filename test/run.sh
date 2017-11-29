#!/bin/bash

BIN_DIR=$1
IN_DIR=$2
OUT_DIR=$3

RHO_BINARY=$BIN_DIR/rho

ALL_FILES=0
CORRECT=0
for IN_FILE in $IN_DIR/*.json; do
	let "ALL_FILES++"
	echo "Running test $ALL_FILES: $IN_FILE."
	OUT_FILE=$OUT_DIR/$(basename $IN_FILE)
	$RHO_BINARY < $IN_FILE > $OUT_FILE.tmp 2>&1
	DIFF=$(diff $OUT_FILE $OUT_FILE.tmp)
	if [[ -z $DIFF ]]; then
		let "CORRECT++"
	else
		echo "$DIFF"
		echo -e "Test $IN_FILE \e[5m\e[91mFAILED\e[39m\e[25m!"
	fi
done

if [[ $ALL_FILES == $CORRECT ]]; then
	echo -e "All $ALL_FILES tests \e[92mOK\e[39m!"
else
	echo -e "Passed only $CORRECT/$ALL_FILES tests!"
fi
