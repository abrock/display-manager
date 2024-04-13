#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <stdlib.h>
#include <stdio.h>

#include <opencv2/highgui.hpp>

#include <QObject>
#include <QLabel>
#include <QMouseEvent>

class DisplayManager : public QObject{
    Q_OBJECT

public:
    DisplayManager();

    Q_INVOKABLE void setImageFile(QString const& str);

    Q_INVOKABLE void setMaskFile(QString const& str);
};

#endif // DISPLAYMANAGER_H
