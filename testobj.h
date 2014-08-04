
#include <iostream>
using namespace std;

#include <QObject>

#include "libupnpp/cdircontent.hxx"
#include "dirreader.h"

class TestObj: public QObject {
    Q_OBJECT;
public:
    TestObj(QObject *parent, DirReader* reader)
        : QObject(parent), m_reader(reader)
        {}
public slots:
    virtual void onSliceAvailable(const UPnPDirContent *dc)
    {
        cerr << "Got slice: " << dc->m_containers.size() << 
            " containers " << dc->m_items.size() << " items " << endl;
    }
    virtual void initRead()
    {
        m_reader->start();
    }
    virtual void onDone(int status)
    {
        cerr << "readdir done, status: " << status << endl;
        m_reader->wait();
        cerr << "wait done: " << endl;
        delete m_reader;
    }
public:    
    DirReader* m_reader;
};
