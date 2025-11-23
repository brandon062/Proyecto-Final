#ifndef MIRECT_H
#define MIRECT_H
#include <QGraphicsRectItem>
#include <QObject>
class MiRect: public QObject,public QGraphicsRectItem {
    Q_OBJECT
public:
    void keyPressEvent (QKeyEvent * evento);
public slots:
    void spawn();
};

#endif // MIRECT_H
