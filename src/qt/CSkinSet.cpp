#include "CSkinSet.h"

#include <QFile>
#include <QTextStream>

#define USERESOURCEQSS

CSkinSet::CSkinSet()
{
}

CSkinSet::~CSkinSet()
{
}

void CSkinSet::loadQssConfig(QWidget *obj)
{
    QFile file(getQssFilePath(obj->objectName()));
    if (file.open(QFile::ReadOnly)) {
        QTextStream filetext(&file);
        QString stylesheet = filetext.readAll();
        if (!stylesheet.isEmpty())
            obj->setStyleSheet(stylesheet);
        file.close();
    }
}

QString CSkinSet::getQssFilePath(const QString &objName)
{
#ifdef USERESOURCEQSS
    return QString(":/res/skin/" + objName + QString(".qss"));
#else
    return QString("res/skin/" + objName + QString(".qss"));
#endif
}
