#ifndef VEHICULO_H
#define VEHICULO_H

#include "EntidadMovil.h"

// Clase intermedia para Tanque, Avion.
class Vehiculo : public EntidadMovil {
    Q_OBJECT
public:
    Vehiculo(QGraphicsItem *parent = nullptr)
        : EntidadMovil(parent) {}
};

#endif // VEHICULO_H

