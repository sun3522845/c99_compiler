#!/bin/sh

sub()
{
    TMP=/tmp/$$.log
    mv $1 $TMP
    cat $TMP | sed -e "s/$2/$3/g" >$1
    rm -f $TMP
}

files=`find . -name '*.h' -or -name '*.cc'`
for file in $files; do
    sub $file "declarer" "declarator"
done
