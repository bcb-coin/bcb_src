#ifndef CSKINSET_H_
#define CSKINSET_H_

#include <QString>
#include <QWidget>

class CSkinSet
{
public:
    CSkinSet();
    ~CSkinSet();

    // 加载qss样式文件
    void loadQssConfig(QWidget *obj);

    // 获取qss样式文件路径
    QString getQssFilePath(const QString &objName);

};

#endif /*CSKINSET_H_*/
