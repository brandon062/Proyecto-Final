#ifndef PROYECTIL_H
#define PROYECTIL_H
#include <QGraphicsItem>
#include "EntidadMovil.h"

class proyectil : public EntidadMovil {
    Q_OBJECT
public:
    proyectil(QGraphicsItem *parent = nullptr);
public slots:
    void move();
};


#endif // PROYECTIL_H
