#ifndef PROYECTILENEMIGO_H
#define PROYECTILENEMIGO_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QGraphicsItem>

class proyectilEnemigo : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    proyectilEnemigo(QGraphicsItem *parent = nullptr);

public slots:
    void move();
};

#endif // PROYECTILENEMIGO_H

