#!/bin/bash

TESTS="ill.bin na1.bin fd.bin na2.bin ka1.bin js.bin ka2.bin c.bin s.bin j.bin l.bin g.bin bn.bin all.bin"

EMAS=emas
EMDAS=../build/emdas

function check()
{
	RES=$1
	MSG=$2
	if [ $RES -ne 0 ] ; then
		echo "Error: $MSG"
		exit 1
	fi
}

for TEST in $TESTS ; do
	echo Processing test: $TEST
	declare -i RES=0
	./mkbin.py $TEST
	check $? "Error preparing test $TEST"

	$EMDAS -c mx16 -nl -nc -na -o $TEST.1.asm $TEST
	check $? "Disassembly error: $TEST -> $TEST.1.asm"
	$EMDAS -c mx16 -na -o $TEST.2.asm $TEST
	check $? "Disassembly error: $TEST -> $TEST.2.asm"

	$EMAS -c mx16 -O raw -o $TEST.1.bin $TEST.1.asm
	check $? "Assembly error: $TEST.1.asm -> $TEST.1.bin"
	$EMAS -c mx16 -O raw -o $TEST.2.bin $TEST.2.asm
	check $? "Assembly error: $TEST.2.asm -> $TEST.2.bin"

	$EMDAS -c mx16 -nl -nc -na -o $TEST.3.asm $TEST.1.bin
	check $? "Disassembly error: $TEST.1.bin -> $TEST.3.asm"
	$EMDAS -c mx16 -nl -nc -na -o $TEST.4.asm $TEST.2.bin
	check $? "Disassembly error: $TEST.2.bin -> $TEST.4.asm"

	cmp $TEST $TEST.1.bin
	check $? "Compare error: $TEST $TEST.1.bin"
	cmp $TEST $TEST.2.bin
	check $? "Compare error: $TEST $TEST.2.bin"

	diff $TEST.1.asm $TEST.3.asm
	check $? "Compare error: $TEST.1.asm $TEST.3.asm"
	diff $TEST.1.asm $TEST.4.asm
	check $? "Compare error: $TEST.1.asm $TEST.4.asm"

	rm -f $TEST*
done
