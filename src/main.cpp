#include <QApplication>
#include "view.h"

int main(int argc, char **argv)
{
    QApplication a(argc, argv);
    View v;
    v.setWindowTitle(QStringLiteral("chrome://dino Qt/C++"));
    v.show();
    a.exec();
}
