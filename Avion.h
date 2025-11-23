#ifndef AVION_H
#define AVION_H

#include "Vehiculo.h"

// Avión genérico (enemigo o jugador).
class Avion : public Vehiculo {
    Q_OBJECT
public:
    Avion(QGraphicsItem *parent = nullptr)
        : Vehiculo(parent) {}
};

#endif // AVION_H

