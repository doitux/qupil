#!/bin/bash
if [[ -n "$1" && -n "$2" ]] ; then

	rm -r ./Qupil-$1*
	mkdir ./Qupil-$1
	cp qupil.exe ./Qupil-$1/
	cp -R ./data ./Qupil-$1/
	rm ./Qupil-$1/data/.directory
	rm ./Qupil-$1/data/sounds/.directory
	zip -r Qupil-$1-windows.zip ./Qupil-$1

	sed -i -e '/<project>/,/<\/project>/ s|<version>[0-9a-z.]\{1,\}</version>|<version>'$1'</version>|g' qupil_bitrock_windows.xml
	$2/bin/builder build ./qupil_bitrock_windows.xml windows

	mv $2/output/Qupil-$1-windows-installer.exe .
	md5sum Qupil-$1-windows-installer.exe
	md5sum Qupil-$1-windows.zip
else
echo "Please use 'win-create-release.sh version full-bitrock-path'"
fi

