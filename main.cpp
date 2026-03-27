#include "orcamento_ti.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("fusion");
    orcamento_ti w;
    w.showMaximized();
    return a.exec();
}
