#!/bin/sh

#make CXXFLAGS="-O6 -pg"
make
#rm src/FbiStuff.cpp

FIM=src/fim


#CMD='50{auto;next}quit;'
CMD='10{*2;*2;*2;next;display;}quit'
#CMD='1{*2;*2;*2;*2;next;display;}quit'
#CMD='10{*5;*"1.5";*"1.5";*"1.5";next;display;}quit'
#CMD='20next;quit;'

# the following seems not only incorrect, but harmful:
#$FIM media/*{jpg,png,gif,bmp} -c '$CMD'

FILES="media/*{jpg,png,gif,bmp}"
#FILES="~/pictures/*/*{jpg,png,gif,bmp}"
#FILES='/home/dez/pictures/*/*'

if true ; then
	# the following is good:
#	$FIM ~/pictures/*/* -c "$CMD"
#	$FIM $FILES -c "$CMD"
	$FIM media/*{jpg,png,gif,bmp} -c "$CMD"
	gprof $FIM | less
else
	valgrind --tool=cachegrind $FIM $FILES -c "$CMD"
fi


