#!/bin/bash
#script for creating cpp/h files with standard heading
if [ "$1" == "" ] ; then
  echo "Using $0 <module_name> [<package_name> [<author>]]" >&2
  exit
fi

module_name=$1
package_name=$2
author=$3

if [ "$package_name" == "" ] ; then 
  package_name=`basename "$module_name"`
fi

if [ "$author" == "" ] ; then 
  author=$USER
fi

h_file=${module_name}.h
c_file=${module_name}.cpp

module_name=`basename "$module_name"`
if [ -f $h_file ] ; then
  echo "File $h_file exists and can't overwrited" >&2
  exit
fi
if [ -f $c_file ] ; then
  echo "File $c_file exists and can't overwrited" >&2
  exit
fi
   
time=`date "+%d.%m.%Y %k:%M:%S"`
year=`date "+%Y"`
macro=`basename "$h_file" | tr "." "_" | tr [:lower:] [:upper:]`"_"`date +%s`

cat > $h_file << EOF
/*----------------------------------------------------------------------------*/
/**
* @pkg $package_name
*/
/**
* Short description of $package_name.
*
* Long description of $package_name.<br>
* (C) DDA team, Kiev, Ukraine $year.<br>
* started $time<br>
* @pkgdoc $package_name
* @author $author
* @version 0.01 
*/
/*----------------------------------------------------------------------------*/
EOF
cat $h_file > $c_file
echo "#ifndef $macro" >> $h_file
echo "#define $macro" >> $h_file
echo /*----------------------------------------------------------------------------*/ >> $h_file
echo >> $h_file
echo /*----------------------------------------------------------------------------*/ >> $h_file
echo "#endif /*$macro*/" >> $h_file
echo >> $h_file
echo "#include \"$h_file\"" >> $c_file
echo /*----------------------------------------------------------------------------*/ >> $c_file
echo >> $c_file
