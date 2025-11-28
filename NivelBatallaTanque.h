#ifndef NIVELBATALLATANQUE_H
#define NIVELBATALLATANQUE_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include "Tanque.h"
#include "TanqueEnemigo.h"
#include "EstructuraMapa.h"
#include "Mina.h"
class NivelBatallaTanque : public QGraphicsView
{
    Q_OBJECT
public:
    explicit NivelBatallaTanque(QWidget *parent = nullptr);

private:
    QGraphicsScene *scene;
    Tanque *tanqueJugador;
    TanqueEnemigo *tanqueEnemigo;

    // Estructuras del mapa
    EstructuraMapa *estructura1;
    EstructuraMapa *estructura2;
    EstructuraMapa *estructura3;

    // Minas
    Mina *mina1;
    Mina *mina2;
    Mina *mina3;
    Mina *mina4;
};

#endif // NIVELBATALLATANQUE_H


