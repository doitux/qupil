/***************************************************************************
 *   Copyright (C) 2006 by Felix Hammer   *
 *   f.hammer@web.de   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "configfile.h"
#include "tinyxml.h"

#define MODUS 0711

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#endif

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <fstream>

#include <sys/types.h>
#include <sys/stat.h>

#include <QApplication>
#include <QtCore>
#include <QObject>

using namespace std;


ConfigFile::ConfigFile(char *argv0, bool readonly)
: QObject(), noWriteAccess(readonly)
{
    myArgv0 = argv0;

    // !!!! Revisionsnummer der Configdefaults !!!!!
    configRev = 30;
    // !!!! Revision of data structure !!!!!
    string dataStructureRev = "2";

    // Pfad und Dateinamen setzen
#ifdef _WIN32
    const char *appDataPath = getenv("AppData");
    if (appDataPath && appDataPath[0] != 0) {
        configFileName = appDataPath;
    } else {
        const int MaxPathSize = 1024;
        char curDir[MaxPathSize + 1];
        curDir[0] = 0;
        _getcwd(curDir, MaxPathSize);
        curDir[MaxPathSize] = 0;
        configFileName = curDir;
        // Testen ob das Verzeichnis beschreibbar ist
        ofstream tmpFile;
        const char *tmpFileName = "qupil_test.tmp";
        tmpFile.open((configFileName + "\\" + tmpFileName).c_str());
        if (tmpFile) {
            // Erfolgreich, Verzeichnis beschreibbar.
            // Datei wieder loeschen.
            tmpFile.close();
            remove((configFileName + "\\" + tmpFileName).c_str());
        } else {
            // Fehlgeschlagen, Verzeichnis nicht beschreibbar
            curDir[0] = 0;
            GetTempPathA(MaxPathSize, curDir);
            curDir[MaxPathSize] = 0;
            configFileName = curDir;
        }
    }
    //define app-dir
    configFileName += "\\qupil\\";
    //define data-dir
    dataDir = configFileName;
    dataDir += "db\\";
    //define cache-dir

    //create directories on first start of app
    mkdir(configFileName.c_str());
    mkdir(dataDir.c_str());


#else
    //define app-dir
    const char *homePath = getenv("HOME");
    if(homePath) {
        configFileName = homePath;
        configFileName += "/.qupil/";
        //define data-dir
        dataDir = configFileName;
        dataDir += "db/";
        //create directories on first start of app
        mkdir(configFileName.c_str(), MODUS) ;
        mkdir(dataDir.c_str(), MODUS);
    }

#endif

    string userDir = configFileName.c_str();

    ostringstream tempIntToString;
    tempIntToString << configRev;

    configList.push_back(ConfigInfo("ConfigRevision", CONFIG_TYPE_INT, tempIntToString.str()));
    configList.push_back(ConfigInfo("AppDataDir", CONFIG_TYPE_STRING, getDataPathStdString(myArgv0)));
    configList.push_back(ConfigInfo("UserDir", CONFIG_TYPE_STRING, userDir));
    configList.push_back(ConfigInfo("Language", CONFIG_TYPE_STRING, getDefaultLanguage()));
    configList.push_back(ConfigInfo("CurrentDataStructureRevision", CONFIG_TYPE_INT, "0"));
    configList.push_back(ConfigInfo("ThisVersionDataStructureRevision", CONFIG_TYPE_INT, dataStructureRev));
    configList.push_back(ConfigInfo("ReleaseString", CONFIG_TYPE_STRING, "1.1"));
    configList.push_back(ConfigInfo("CopyrightTimeString", CONFIG_TYPE_STRING, "2006-2010"));

    list<string> genreList;
    genreList.push_back(QString(tr("Tune")).toUtf8().constData());
    genreList.push_back(QString(tr("Baroque")).toUtf8().constData());
    genreList.push_back(QString(tr("Classical")).toUtf8().constData());
    genreList.push_back(QString(tr("Romantic")).toUtf8().constData());
    genreList.push_back(QString(tr("Modernism")).toUtf8().constData());
    genreList.push_back(QString(tr("New music")).toUtf8().constData());
    genreList.push_back(QString(tr("Musical")).toUtf8().constData());
    genreList.push_back(QString(tr("Jazz")).toUtf8().constData());
    genreList.push_back(QString(tr("Rock")).toUtf8().constData());
    genreList.push_back(QString(tr("Pop")).toUtf8().constData());
    genreList.push_back(QString(tr("Irish Folk")).toUtf8().constData());
    configList.push_back(ConfigInfo("PalPiecesGenreList", CONFIG_TYPE_STRING_LIST, "PiecesGenre", genreList));

    list<string> locationList;
    locationList.push_back(QString(tr("Room 20")).toUtf8().constData());
    locationList.push_back(QString(tr("Room 16")).toUtf8().constData());
    locationList.push_back(QString(tr("Auditorium")).toUtf8().constData());
    configList.push_back(ConfigInfo("LessonLocationList", CONFIG_TYPE_STRING_LIST, "LessonLocation", locationList));

    list<string> instrumentList;
    instrumentList.push_back(QString(tr("Violin")).toUtf8().constData());
    instrumentList.push_back(QString(tr("Viola")).toUtf8().constData());
    instrumentList.push_back(QString(tr("Cello")).toUtf8().constData());
    instrumentList.push_back(QString(tr("Double Bass")).toUtf8().constData());
    configList.push_back(ConfigInfo("PupilInstrumentList", CONFIG_TYPE_STRING_LIST, "Instrument", instrumentList));

    list<string> instrumentSizeList;
    instrumentSizeList.push_back("4/4");
    instrumentSizeList.push_back("3/4");
    instrumentSizeList.push_back("1/2");
    instrumentSizeList.push_back("1/4");
    instrumentSizeList.push_back("1/8");
    instrumentSizeList.push_back("1/16");
    configList.push_back(ConfigInfo("PupilInstrumentSizeList", CONFIG_TYPE_STRING_LIST, "InstrumentSize", instrumentSizeList));

    configList.push_back(ConfigInfo("BirthdayReminder", CONFIG_TYPE_INT, "1"));
    configList.push_back(ConfigInfo("LessonEndMsg", CONFIG_TYPE_INT, "1"));
    configList.push_back(ConfigInfo("LessonEndMsgSoundVolume", CONFIG_TYPE_INT, "7"));
    configList.push_back(ConfigInfo("MinutesToLessonEndForMsg", CONFIG_TYPE_INT, "3"));
    configList.push_back(ConfigInfo("MsgSoundFilePath", CONFIG_TYPE_STRING, getDataPathStdString(myArgv0)+"/sounds/ahem.ogg"));
    configList.push_back(ConfigInfo("RemSoundFilePath", CONFIG_TYPE_STRING, getDataPathStdString(myArgv0)+"/sounds/reminder.wav"));
    configList.push_back(ConfigInfo("RemSoundVolume", CONFIG_TYPE_INT, "7"));

    configList.push_back(ConfigInfo("TimeTableDayBColor", CONFIG_TYPE_STRING, "110,60,90"));
    configList.push_back(ConfigInfo("TimeTableDayTColor", CONFIG_TYPE_STRING, "255,255,255"));
    configList.push_back(ConfigInfo("TimeTableLessonBColor", CONFIG_TYPE_STRING, "194,255,76"));
    configList.push_back(ConfigInfo("TimeTableLessonTColor", CONFIG_TYPE_STRING, "0,0,0"));
    configList.push_back(ConfigInfo("TimeTablePupilBColor", CONFIG_TYPE_STRING, "255,255,255"));
    configList.push_back(ConfigInfo("TimeTablePupilTColor", CONFIG_TYPE_STRING, "0,0,0"));
    configList.push_back(ConfigInfo("SaveNotesPiecesForAllPupil", CONFIG_TYPE_INT, "1"));
    configList.push_back(ConfigInfo("RecitalIntervalCheckerOnlySolo", CONFIG_TYPE_INT, "1"));

    configList.push_back(ConfigInfo("RecitalModerationDuration", CONFIG_TYPE_INT, "4"));
    configList.push_back(ConfigInfo("RecitalBetweenPiecesDuration", CONFIG_TYPE_INT, "2"));

    configList.push_back(ConfigInfo("WindowFullScreenSave", CONFIG_TYPE_INT, "0"));
    configList.push_back(ConfigInfo("WindowHeightSave", CONFIG_TYPE_INT, "637"));
    configList.push_back(ConfigInfo("WindowWidthSave", CONFIG_TYPE_INT, "1000"));

    //fill tempList firstTime
    configBufferList = configList;

    if(!noWriteAccess) {
        configFileName += "config.xml";

        //Prüfen ob Configfile existiert --> sonst anlegen
        TiXmlDocument doc(configFileName);
        if(!doc.LoadFile()) {
            myConfigState = NONEXISTING;
            updateConfig(myConfigState);
        } else {
            //Check if config revision and AppDataDir is ok. Otherwise --> update()
            int tempRevision = 0;
            string tempAppDataPath ("");

            TiXmlHandle docHandle( &doc );
            TiXmlElement* confRevision = docHandle.FirstChild( "Qupil" ).FirstChild( "Configuration" ).FirstChild( "ConfigRevision" ).ToElement();
            if ( confRevision ) {
                confRevision->QueryIntAttribute("value", &tempRevision );
            }
            TiXmlElement* confAppDataPath = docHandle.FirstChild( "Qupil" ).FirstChild( "Configuration" ).FirstChild( "AppDataDir" ).ToElement();
            if ( confAppDataPath ) {
                const char *tmpStr = confAppDataPath->Attribute("value");
                if (tmpStr) tempAppDataPath = tmpStr;
                //if appdatapath changes directly update it here not in UpdateConfig()
                if(tempAppDataPath != getDataPathStdString(myArgv0)) {
                    confAppDataPath->SetAttribute("value", getDataPathStdString(myArgv0));
                    doc.SaveFile( configFileName );
                }
            }

            if (tempRevision < configRev) { /*löschen()*/
                myConfigState = OLD;
                updateConfig(myConfigState) ;
            }
        }

        fillBuffer();
    }
}


ConfigFile::~ConfigFile()
{
}


void ConfigFile::fillBuffer()
{

// 	boost::recursive_mutex::scoped_lock lock(m_configMutex);

    size_t i;
    string tempString1("");
    string tempString2("");

    TiXmlDocument doc(configFileName);

    if(doc.LoadFile()) {
        TiXmlHandle docHandle( &doc );

        for (i=0; i<configBufferList.size(); i++) {

            TiXmlElement* conf = docHandle.FirstChild( "Qupil" ).FirstChild( "Configuration" ).FirstChild( configList[i].name ).ToElement();

            if ( conf ) {

                const char *tmpStr1 = conf->Attribute("value");
                if (tmpStr1) tempString1 = tmpStr1;
                configBufferList[i].defaultValue = tempString1;

                const char *tmpStr2 = conf->Attribute("type");
                if (tmpStr2) {
                    tempString2 = tmpStr2;
                    if(tempString2 == "list") {

                        list<string> tempStringList2;

                        TiXmlElement* confList = docHandle.FirstChild( "Qupil" ).FirstChild( "Configuration" ).FirstChild( configList[i].name ).FirstChild().ToElement();

                        for( ; confList; confList=confList->NextSiblingElement()) {
                            tempStringList2.push_back(confList->Attribute("value"));
                        }

                        configBufferList[i].defaultListValue = tempStringList2;
                    }
                }
            } else {
                qDebug("Could not find the root element in the config file!");
            }
        }
    }
}

void ConfigFile::writeBuffer() const
{

// 	boost::recursive_mutex::scoped_lock lock(m_configMutex);

    //write buffer to disc if enabled
    if(!noWriteAccess) {
        TiXmlDocument doc;
        TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "UTF-8", "");
        doc.LinkEndChild( decl );

        TiXmlElement * root = new TiXmlElement( "Qupil" );
        doc.LinkEndChild( root );

        TiXmlElement * config;
        config = new TiXmlElement( "Configuration" );
        root->LinkEndChild( config );

        size_t i;

        for (i=0; i<configBufferList.size(); i++) {
            TiXmlElement *tmpElement = new TiXmlElement(configBufferList[i].name);
            config->LinkEndChild( tmpElement );
            tmpElement->SetAttribute("value", configBufferList[i].defaultValue);

            if(configBufferList[i].type == CONFIG_TYPE_INT_LIST || configBufferList[i].type == CONFIG_TYPE_STRING_LIST) {

                tmpElement->SetAttribute("type", "list");
                list<string> tempList = configBufferList[i].defaultListValue;
                list<string>::iterator it;
                for(it = tempList.begin(); it != tempList.end(); it++) {

                    TiXmlElement *tmpSubElement = new TiXmlElement(configBufferList[i].defaultValue);
                    tmpElement->LinkEndChild( tmpSubElement );
                    tmpSubElement->SetAttribute("value", *it);
                }
            }
        }
        doc.SaveFile( configFileName );
    }
}

void ConfigFile::updateConfig(ConfigState myConfigState)
{

// 	boost::recursive_mutex::scoped_lock lock(m_configMutex);

    size_t i;

    if(myConfigState == NONEXISTING) {

        //Create a new ConfigFile!
        TiXmlDocument doc;
        TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "UTF-8", "");
        doc.LinkEndChild( decl );

        TiXmlElement * root = new TiXmlElement( "Qupil" );
        doc.LinkEndChild( root );

        TiXmlElement * config;
        config = new TiXmlElement( "Configuration" );
        root->LinkEndChild( config );

        for (i=0; i<configList.size(); i++) {
            TiXmlElement *tmpElement = new TiXmlElement(configList[i].name);
            config->LinkEndChild( tmpElement );
            tmpElement->SetAttribute("value", stringToUtf8(configList[i].defaultValue));

            if(configList[i].type == CONFIG_TYPE_INT_LIST || configList[i].type == CONFIG_TYPE_STRING_LIST) {

                tmpElement->SetAttribute("type", "list");
                list<string> tempList = configList[i].defaultListValue;
                list<string>::iterator it;
                for(it = tempList.begin(); it != tempList.end(); it++) {

                    TiXmlElement *tmpSubElement = new TiXmlElement(configList[i].defaultValue);
                    tmpElement->LinkEndChild( tmpSubElement );
                    tmpSubElement->SetAttribute("value", *it);
                }
            }
        }

        doc.SaveFile( configFileName );
    }

    if(myConfigState == OLD) {

        TiXmlDocument oldDoc(configFileName);

        //load the old one
        if(oldDoc.LoadFile()) {

            string tempString1("");
            string tempString2("");

            TiXmlDocument newDoc;

            //Create the new one
            TiXmlDeclaration * decl = new TiXmlDeclaration( "1.0", "UTF-8", "");
            newDoc.LinkEndChild( decl );

            TiXmlElement * root = new TiXmlElement( "Qupil" );
            newDoc.LinkEndChild( root );

            TiXmlElement * config;
            config = new TiXmlElement( "Configuration" );
            root->LinkEndChild( config );

            //change configRev and AppDataPath
            TiXmlElement * confElement0 = new TiXmlElement( "ConfigRevision" );
            config->LinkEndChild( confElement0 );
            confElement0->SetAttribute("value", configRev);

            TiXmlHandle oldDocHandle( &oldDoc );

            for (i=0; i<configList.size(); i++) {

                TiXmlElement* oldConf = oldDocHandle.FirstChild( "Qupil" ).FirstChild( "Configuration" ).FirstChild( configList[i].name ).ToElement();

                if ( oldConf && !(configList[i].name == "ThisVersionDataStructureRevision") && !(configList[i].name == "ReleaseString") && !(configList[i].name == "CopyrightTimeString") ) {
                    // if element is already there --> take over the saved values

                    if(configList[i].name != "ConfigRevision" && configList[i].name != "AppDataDir") {
                        // dont update ConfigRevision and AppDataDir becaus it was already set ^

                        TiXmlElement *tmpElement = new TiXmlElement(configList[i].name);
                        config->LinkEndChild( tmpElement );

                        const char *tmpStr1 = oldConf->Attribute("value");
                        if (tmpStr1) tempString1 = tmpStr1;
                        tmpElement->SetAttribute("value", tempString1);

                        //for lists copy elements
                        const char *tmpStr2 = oldConf->Attribute("type");
                        if (tmpStr2) {
                            tempString2 = tmpStr2;
                            if(tempString2 == "list") {

                                list<string> tempStringList2;

                                TiXmlElement* oldConfList = oldDocHandle.FirstChild( "Qupil" ).FirstChild( "Configuration" ).FirstChild( configList[i].name ).FirstChild().ToElement();

                                for( ; oldConfList; oldConfList=oldConfList->NextSiblingElement()) {
                                    tempStringList2.push_back(oldConfList->Attribute("value"));
                                }

                                tmpElement->SetAttribute("type", "list");
                                list<string> tempList = tempStringList2;
                                list<string>::iterator it;
                                for(it = tempList.begin(); it != tempList.end(); it++) {

                                    TiXmlElement *tmpSubElement = new TiXmlElement(tempString1);
                                    tmpElement->LinkEndChild( tmpSubElement );
                                    tmpSubElement->SetAttribute("value", *it);
                                }
                            }
                        }
                    }
                } else {
                    // if element is not there --> set it with defaultValue
                    TiXmlElement *tmpElement = new TiXmlElement(configList[i].name);
                    config->LinkEndChild( tmpElement );
                    tmpElement->SetAttribute("value", stringToUtf8(configList[i].defaultValue));

                    if(configList[i].type == CONFIG_TYPE_INT_LIST || configList[i].type == CONFIG_TYPE_STRING_LIST) {

                        tmpElement->SetAttribute("type", "list");
                        list<string> tempList = configList[i].defaultListValue;
                        list<string>::iterator it;
                        for(it = tempList.begin(); it != tempList.end(); it++) {

                            TiXmlElement *tmpSubElement = new TiXmlElement(configList[i].defaultValue);
                            tmpElement->LinkEndChild( tmpSubElement );
                            tmpSubElement->SetAttribute("value", *it);
                        }
                    }
                }
            }
            newDoc.SaveFile( configFileName );
        } else {
            qDebug("Cannot update config file: Unable to load configuration.");
        }
    }
}

string ConfigFile::readConfigString(string varName) const
{
// 	boost::recursive_mutex::scoped_lock lock(m_configMutex);

    size_t i;
    string tempString("");

    for (i=0; i<configBufferList.size(); i++) {

        if (configBufferList[i].name == varName) {
            tempString = configBufferList[i].defaultValue;
        }
    }

    return tempString;
}

int ConfigFile::readConfigInt(string varName) const
{
// 	boost::recursive_mutex::scoped_lock lock(m_configMutex);

    size_t i;
    string tempString("");
    int tempInt=0;

    for (i=0; i<configBufferList.size(); i++) {

        if (configBufferList[i].name == varName) {
            tempString = configBufferList[i].defaultValue;
        }
    }

    istringstream isst;
    isst.str (tempString);
    isst >> tempInt;

    return tempInt;
}

list<string> ConfigFile::readConfigStringList(string varName) const
{
// 	boost::recursive_mutex::scoped_lock lock(m_configMutex);

    size_t i;
    list<string> tempStringList;

    for (i=0; i<configBufferList.size(); i++) {

        if (configBufferList[i].name == varName) {
            tempStringList = configBufferList[i].defaultListValue;
        }
    }

    return tempStringList;
}


list<int> ConfigFile::readConfigIntList(string varName) const
{
// 	boost::recursive_mutex::scoped_lock lock(m_configMutex);

    size_t i;
    list<string> tempStringList;
    list<int> tempIntList;

    for (i=0; i<configBufferList.size(); i++) {

        if (configBufferList[i].name == varName) {
            tempStringList = configBufferList[i].defaultListValue;
        }
    }

    istringstream isst;
    string tempString;
    int tempInt;
    list<string>::iterator it;
    for(it = tempStringList.begin(); it != tempStringList.end(); it++) {

        isst.str(*it);
        isst >> tempInt;
        tempIntList.push_back(tempInt);
        isst.str("");
        isst.clear();
    }

    return tempIntList;
}


void ConfigFile::writeConfigInt(string varName, int varCont)
{
// 	boost::recursive_mutex::scoped_lock lock(m_configMutex);

    size_t i;
    string tempString;
    ostringstream intToString;

    for (i=0; i<configBufferList.size(); i++) {

        if (configBufferList[i].name == varName) {
            intToString << varCont;
            configBufferList[i].defaultValue = intToString.str();
        }
    }
}


void ConfigFile::writeConfigIntList(string varName, list<int> varCont)
{
// 	boost::recursive_mutex::scoped_lock lock(m_configMutex);

    size_t i;
    ostringstream intToString;
    list<string> stringList;

    for (i=0; i<configBufferList.size(); i++) {

        if (configBufferList[i].name == varName) {
            string tempString;
            list<int>::iterator it;
            for(it = varCont.begin(); it != varCont.end(); it++) {

                intToString << (*it);
                stringList.push_back(intToString.str());
                intToString.str("");
                intToString.clear();
            }

            configBufferList[i].defaultListValue = stringList;
        }
    }
}

void ConfigFile::writeConfigStringList(string varName, list<string> varCont)
{
// 	boost::recursive_mutex::scoped_lock lock(m_configMutex);

    size_t i;
    list<string> stringList;

    for (i=0; i<configBufferList.size(); i++) {

        if (configBufferList[i].name == varName) {

            configBufferList[i].defaultListValue = varCont;
        }
    }
}

void ConfigFile::writeConfigString(string varName, string varCont)
{
// 	boost::recursive_mutex::scoped_lock lock(m_configMutex);

    size_t i;
    for (i=0; i<configBufferList.size(); i++) {
        if (configBufferList[i].name == varName) {
            configBufferList[i].defaultValue = varCont;
        }
    }

}

std::string ConfigFile::getDataPathStdString(const char * /*argv0*/)
{
    QString path(QCoreApplication::instance()->applicationDirPath());

#ifdef _WIN32
    path += "/data/";
#else
#ifdef __APPLE__
    if (QRegExp("Contents/MacOS/?$").indexIn(path) != -1) {
        // pointing into an macosx application bundle
        path += "/../Resources/data/";
    } else {
        path += "/data/";
    }
#else //Unix
    if (QRegExp("pokerth/?$").indexIn(path) != -1) {
        // there is an own application directory
        path += "/data/";
    } else if (QRegExp("usr/games/bin/?$").indexIn(path) != -1) {
        // we are in /usr/games/bin (like gentoo linux does)
        path += "/../../share/games/pokerth/data/";
    } else if (QRegExp("usr/games/?$").indexIn(path) != -1) {
        // we are in /usr/games (like Debian linux does)
        path += "/../share/games/pokerth/";
    } else if (QRegExp("bin/?$").indexIn(path) != -1) {
        // we are in a bin directory. e.g. /usr/bin
        path += "/../share/pokerth/data/";

    } else {
        path += "/data/";
    }
#endif
#endif
    return (QDir::cleanPath(path) + "/").toUtf8().constData();
}

std::string ConfigFile::stringToUtf8(const std::string &myString)
{

    QString tmpString = QString::fromStdString(myString);
    std::string myUtf8String = tmpString.toUtf8().constData();

    return myUtf8String;
}

std::string ConfigFile::stringFromUtf8(const std::string &myString)
{
    QString tmpString = QString::fromUtf8(myString.c_str());

    return tmpString.toStdString();
}

std::string ConfigFile::getDefaultLanguage()
{
    return QLocale::system().name().toStdString();
}
