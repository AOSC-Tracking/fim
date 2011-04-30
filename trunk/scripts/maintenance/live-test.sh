#!/bin/sh
# A simplistic and incomplete test to verify if fim honours its 'live' configuration. 
FIM=src/fim
FH=.fim_history
rm $FH
export HOME=`pwd` 
#$FIM --help
$FIM media/* -c quit
#ls -ltr $FH
test ! -f .fim_history
return $?
