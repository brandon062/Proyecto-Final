// EntidadMovil.h
#ifndef ENTIDADMOVIL_H
#define ENTIDADMOVIL_H

#include "Entidad.h"

// Entidad que se mueve (tiene velocidad).
class EntidadMovil : public Entidad {
    Q_OBJECT
public:
    EntidadMovil(QGraphicsItem *parent = nullptr)
        : Entidad(parent), velocidadX(0), velocidadY(0) {}

    double velocidadX;
    double velocidadY;
};

#endif // ENTIDADMOVIL_H

