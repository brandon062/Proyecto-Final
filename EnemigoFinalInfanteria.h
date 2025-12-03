#ifndef ENEMIGOFINALINFANTERIA_H
#define ENEMIGOFINALINFANTERIA_H

#include "EnemigoInfanteria.h"

// Enemigo jefe final que hereda la IA de EnemigoInfanteria
class EnemigoFinalInfanteria : public EnemigoInfanteria
{
public:
    explicit EnemigoFinalInfanteria(JugadorInfanteria *objetivo,
                                    QGraphicsItem *parent = nullptr);

    void recibirDisparo() override;

private:
    int vidaRestante = 8;   // el jefe aguanta 8 hits
};

#endif // ENEMIGOFINALINFANTERIA_H


