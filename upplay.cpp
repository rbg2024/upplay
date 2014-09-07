/* Copyright (C) 2014 J.F.Dockes
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <stdio.h>
#include <unistd.h>

#include <string>
#include <iostream>
using namespace std;

#include <QApplication>
#include <QObject>
#include <QTimer>

#include <libupnpp/upnpplib.hxx>
#include <libupnpp/log.hxx>

#include "application.h"

using namespace UPnPClient;

static const char *thisprog;

static int    op_flags;
#define OPT_h     0x4 
#define OPT_c     0x20

static const char usage [] =
    "\n"
    ;
static void
Usage(void)
{
    FILE *fp = (op_flags & OPT_h) ? stdout : stderr;
    fprintf(fp, "%s: Usage: %s", thisprog, usage);
    exit((op_flags & OPT_h)==0);
}


int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("Upmpd.org");
    QCoreApplication::setApplicationName("upplay");

    string a_config;

    QStringList params;
    for(int i = 1; i < argc; i++){
        QString param(argv[i]);
        params.push_back(param);
    }

    thisprog = argv[0];
    argc--; argv++;

    while (argc > 0 && **argv == '-') {
        (*argv)++;
        if (!(**argv))
            Usage();
        while (**argv)
            switch (*(*argv)++) {
            case 'c':   op_flags |= OPT_c; if (argc < 2)  Usage();
                a_config = *(++argv);
                argc--; goto b1;
            default: Usage();
            }
    b1: argc--; argv++;
    }

    if (argc > 0)
        Usage();

    if (upnppdebug::Logger::getTheLog("/tmp/upplay.log") == 0) {
        cerr << "Can't initialize log" << endl;
        return 1;
    }

    LibUPnP *mylib = LibUPnP::getLibUPnP();
    if (!mylib) {
        cerr << "Can't get LibUPnP" << endl;
        return 1;
    }
    if (!mylib->ok()) {
        cerr << "Lib init failed: " <<
            mylib->errAsString("main", mylib->getInitError()) << endl;
        return 1;
    }
    //mylib->setLogFileName("/tmp/libupnp.log");

    QFont font("DejaVu Sans", 9, 55,  false);
	font.setHintingPreference(QFont::PreferNoHinting);
	int strategy =  (QFont::PreferDefault | QFont::PreferQuality);
	font.setStyleStrategy((QFont::StyleStrategy) strategy  );
    app.setFont(font);

    Application application(&app, params.size(), 0);
    if(!application.is_initialized()) 
        return 1;

    return app.exec();
}
