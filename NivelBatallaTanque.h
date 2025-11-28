#ifndef NIVELBATALLATANQUE_H
#define NIVELBATALLATANQUE_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include "Tanque.h"
#include "TanqueEnemigo.h"

class NivelBatallaTanque : public QGraphicsView
{
    Q_OBJECT
public:
    explicit NivelBatallaTanque(QWidget *parent = nullptr);

private:
    QGraphicsScene *scene;
    Tanque *tanqueJugador;
    TanqueEnemigo *tanqueEnemigo;   // <--- NUEVO
};

#endif // NIVELBATALLATANQUE_H


