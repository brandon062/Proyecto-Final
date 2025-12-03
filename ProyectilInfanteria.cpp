#include "ProyectilInfanteria.h"
#include "JugadorInfanteria.h"
#include "EnemigoInfanteria.h"

#include <QGraphicsScene>
#include <QPixmap>
#include <QList>

ProyectilInfanteria::ProyectilInfanteria(int dir, bool delJugador, QGraphicsItem *parent)
    : QObject()
    , QGraphicsPixmapItem(parent)
    , direccion(dir)
    , velocidad(10.0)
    , timer(new QTimer(this))
    , esDelJugador(delJugador)
{
    // Sprite del proyectil (ajusta ruta/tamaño si quieres otro)
    QPixmap sprite(":/images/proyectil_tanque.png");
    if (!sprite.isNull()) {
        QPixmap esc = sprite.scaled(16, 4,
                                    Qt::KeepAspectRatio,
                                    Qt::SmoothTransformation);
        setPixmap(esc);
    }

    setZValue(5); // por encima del suelo

    connect(timer, &QTimer::timeout,
            this, &ProyectilInfanteria::mover);
    timer->start(16);
}

void ProyectilInfanteria::mover()
{
    if (!scene()) {
        deleteLater();
        return;
    }

    // 1) Revisar colisiones antes de movernos
    QList<QGraphicsItem*> colisiones = collidingItems();
    for (QGraphicsItem *item : colisiones) {

        if (esDelJugador) {
            // Bala del jugador: impacta enemigos de infantería
            if (auto enemigo = dynamic_cast<EnemigoInfanteria*>(item)) {
                enemigo->recibirDisparo();   // el enemigo maneja su animación de muerte

                deleteLater();               // destruir este proyectil
                return;
            }
        } else {
            // Bala enemiga: impacta al jugador
            if (auto jugador = dynamic_cast<JugadorInfanteria*>(item)) {
                jugador->recibirDisparo();   // resta 1 vida y anima muerte si llega a 0

                deleteLater();
                return;
            }
        }
    }

    // 2) Movimiento rectilíneo
    setPos(x() + direccion * velocidad, y());

    // 3) Eliminar si sale de la escena
    if (x() < -100 || x() > scene()->width() + 100 ||
        y() < -100 || y() > scene()->height() + 100) {
        deleteLater();
    }
}


