#ifndef BUILDTIMETABLEDOC_H
#define BUILDTIMETABLEDOC_H

#include <QtCore>
#include <QtSql>
#include "configfile.h"

#include "docviewerdialogimpl.h"

class BuildTimeTableDoc : public QObject
{
    Q_OBJECT
public:
    BuildTimeTableDoc(ConfigFile*);

    void run();

private:

    ConfigFile *myConfig;
    DocViewerDialogImpl *myDocViewerDialog;

};

#endif // BUILDTIMETABLEDOC_H
