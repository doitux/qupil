#!/bin/bash
if [[ -n "$1" && -n "$2" ]] ; then

	rm -r ./Qupil-$1*
	mkdir ./Qupil-$1
	cp release/qupil.exe ./Qupil-$1/
	cp release/qupil.exe .
	cp -R ./data ./Qupil-$1/
	find ./Qupil-$1/ -name ".svn" | xargs rm -Rf
	zip -r Qupil-$1-windows.zip ./Qupil-$1

	/home/buildlinux64/bitrock-installbuilder/installbuilder-7.2.5/bin/builder build /home/buildlinux64/slave/qupil-release-win32/build/qupil_bitrock_windows.xml windows

	mv /home/buildlinux64/bitrock-installbuilder/installbuilder-7.2.5/output/Qupil-$1-windows-installer.exe .
	scp Qupil-$1-windows-installer.exe Qupil-$1-windows.zip doitux,qupil@frs.sourceforge.net:/home/frs/project/q/qu/qupil/Qupil/$2/
	#scp Qupil-$1-windows.zip doitux,qupil@frs.sourceforge.net:/home/frs/project/q/qu/qupil/Qupil/$2/
	md5sum Qupil-$1-windows-installer.exe
	rm Qupil-$1-windows-installer.exe
	md5sum Qupil-$1-windows.zip
	rm Qupil-$1-windows.zip

else
echo "Please use 'win-create-release.sh version sf-dir-version'"
fi

