#!/bin/bash

TESTS="ill.bin na1.bin fd.bin na2.bin ka1.bin js.bin ka2.bin c.bin s.bin j.bin l.bin g.bin bn.bin all.bin"

EMAS=../../emas/build/src/emas
EMDAS=../build/src/emdas

for TEST in $TESTS ; do
	echo Processing test: $TEST
	declare -i RES=0
	./mkbin.py $TEST
	RES=$((RES+$?))

	$EMDAS -c mx16 -nl -nc -na -o $TEST.1.asm $TEST
	RES=$((RES+$?))
	$EMDAS -c mx16 -na -o $TEST.2.asm $TEST
	RES=$((RES+$?))

	$EMAS -c mx16 -O raw -o $TEST.1.bin $TEST.1.asm
	RES=$((RES+$?))
	$EMAS -c mx16 -O raw -o $TEST.2.bin $TEST.2.asm
	RES=$((RES+$?))

	$EMDAS -c mx16 -nl -nc -na -o $TEST.3.asm $TEST.1.bin
	RES=$((RES+$?))
	$EMDAS -c mx16 -nl -nc -na -o $TEST.4.asm $TEST.2.bin
	RES=$((RES+$?))

	cmp $TEST $TEST.1.bin
	RES=$((RES+$?))
	cmp $TEST $TEST.2.bin
	RES=$((RES+$?))

	diff $TEST.1.asm $TEST.3.asm
	RES=$((RES+$?))
	diff $TEST.1.asm $TEST.4.asm
	RES=$((RES+$?))

	if [ $RES -ne 0 ] ; then
		echo Error processing: $TEST
		exit 1
	fi

	rm -f $TEST $TEST.1.asm $TEST.2.asm $TEST.3.asm $TEST.4.asm $TEST.1.bin $TEST.2.bin
done
