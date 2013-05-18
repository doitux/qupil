#!/bin/bash
if [[ -n "$1" && -n "$2" && -n "$3"  && -n "$4"  ]] ; then

	SCRIPTHOME=`pwd`

	rm -r linux_package
	mkdir linux_package
	cd linux_package
	
	# CHECKOUT Qupil SOURCECODE
	svn co http://qupil.svn.sourceforge.net/svnroot/qupil/trunk/qupil ./qupil-src
	cd qupil-src
	
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
	
	# MAKE SOME STATIC BUILD CHANGES IN PRO_FILES
	sed "s/# QMAKE_CXXFLAGS += -ffunction-sections -fdata-sections/QMAKE_CXXFLAGS += -ffunction-sections -fdata-sections/g" -i qupil.pro
	sed "s/# QMAKE_LFLAGS += -Wl,--gc-sections/QMAKE_LFLAGS += -Wl,--gc-sections/g" -i qupil.pro
	sed -i 's:\(LIB_DIRS = \$\${PREFIX}/lib\) \\:\1 /usr/lib \\:g' qupil.pro
	
	# B U I L D 
	$3/bin/qmake qupil.pro PREFIX=$3
	make
	
	# COPY LIBS
	cd ..
	mkdir libs
	cp /usr/lib/libstdc++.so.6 libs/
	cp /usr/lib/libmikmod.so.2 libs/
	cp /usr/lib/libSDL-1.2.so.0 libs/
	cp /usr/lib/libSDL_mixer-1.2.so.0 libs/
	cp /lib/libkeyutils.so.1 libs/
	cp /usr/lib/libldap-2.4.so.2 libs/
	cp /usr/lib/liblber-2.4.so.2 libs/
	cp /usr/lib/libgssapi_krb5.so.2 libs/
	cp /usr/lib/libkrb5.so.3 libs/
	cp /usr/lib/libk5crypto.so.3 libs/
	cp /usr/lib/libkrb5support.so.0 libs/
	cp /usr/lib/libicudata.so.40 libs/
	cp /usr/lib/libicui18n.so.40 libs/
	cp /usr/lib/libicuuc.so.40 libs/
	cp /lib/libbz2.so.1 libs/
	cd libs/
	
	# BUILD BITROCK PACKAGE
	$4/bin/builder build $SCRIPTHOME/qupil_bitrock_linux.xml
	
	# INSTALL BITROCK PACKAGE (remove uninstall)
	cd $4/output/
	rm -r Qupil-$1/
	$4/output/Qupil-$1-linux-installer.run --mode unattended --prefix $4/output/Qupil-$1/
	sleep 10
	cd $4/output/Qupil-$1/
	rm uninstall
	cd ..
	rm Qupil-$1-linux.tar.bz2
	tar -cvjf Qupil-$1-linux.tar.bz2 Qupil-$1/
	
	scp Qupil-$1-linux-installer.run Qupil-$1-linux.tar.bz2 doitux,qupil@frs.sourceforge.net:/home/frs/project/q/qu/qupil/Qupil/$2/
	md5sum Qupil-$1-linux-installer.run
	md5sum Qupil-$1-linux.tar.bz2

else
echo "Please use 'linux-create-release.sh version sf-dir-version PathToQt PathToInstallbuilder'"
fi
