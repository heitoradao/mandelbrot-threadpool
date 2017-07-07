#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QList>
#include "jobresult.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    if (translator.load("pt_BR")) {
        a.installTranslator(&translator);
    }

    qRegisterMetaType<JobResult>("JobResult");
    qRegisterMetaType<QList<JobResult> >("QList<JobResult>");

    MainWindow w;
    w.show();

    return a.exec();
}
