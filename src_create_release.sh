#!/bin/bash
if [[ -n "$1" && -n "$2"  ]] ; then

	rm -r Qupil-$1-src
	mkdir Qupil-$1-src
	cd Qupil-$1-src
	
	# CHECKOUT Qupil SOURCECODE
	svn co http://qupil.svn.sourceforge.net/svnroot/qupil/trunk/qupil .
	
	# CLEAN SOURCES
	svn cleanup
	find . -type d -name .svn -exec rm -rf {} \;
	find . -type f -name .directory -exec rm -rf {} \;
	find . -type f -name *~ -exec rm -rf {} \;
	rm -rf debug
	rm -rf mocs
	rm -rf obj
	rm -rf old
	rm -rf qtc-gdbmacros
	rm -rf release
	rm -rf uics
	rm -rf bin
	rm -rf lib	

	cd ..
	tar -cvjf Qupil-$1-src.tar.bz2 Qupil-$1-src/
	scp Qupil-$1-src.tar.bz2 doitux,qupil@frs.sourceforge.net:/home/frs/project/q/qu/qupil/Qupil/$2/
	md5sum Qupil-$1-src.tar.bz2

else
echo "Please use 'linux-create-release.sh version sf-dir-version'"
fi
