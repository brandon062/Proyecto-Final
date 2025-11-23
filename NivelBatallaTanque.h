#ifndef NIVELBATALLATANQUE_H
#define NIVELBATALLATANQUE_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>

#include "Tanque.h"   // el tanque jugador

class NivelBatallaTanque : public QGraphicsView
{
    Q_OBJECT
public:
    NivelBatallaTanque(QWidget *parent = nullptr);

private:
    QGraphicsScene *scene;
    Tanque *tanqueJugador;
};

#endif

