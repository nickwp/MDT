#!/bin/bash

export MDTROOT=`pwd`

if [[ -z "${LD_LIBRARY_PATH}" ]]; then
	export LD_LIBRARY_PATH=$MDTROOT
else
	export LD_LIBRARY_PATH=${MDTROOT}:${LD_LIBRARY_PATH}
fi

rm -f *.so 

cd cpp
make clean
make all
cd ..
`ls -d ./cpp/* | grep .so | xargs -l ln -s`

#cd pywrap
#make clean
#make all
#cd ..
#`ls -d ./pywrap/* | grep .so | xargs -l ln -s`
