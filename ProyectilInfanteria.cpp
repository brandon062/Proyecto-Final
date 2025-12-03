#include "ProyectilInfanteria.h"
#include "JugadorInfanteria.h"
#include "EnemigoInfanteria.h"

#include <QGraphicsScene>
#include <QPixmap>
#include <QList>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsItem>

ProyectilInfanteria::ProyectilInfanteria(int dir, bool delJugador, QGraphicsItem *parent)
    : QObject()
    , QGraphicsPixmapItem(parent)
    , direccion(dir)
    , velocidad(10.0)
    , timer(new QTimer(this))
    , esDelJugador(delJugador)
{
    // Sprite del proyectil
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

    // 1) Revisar colisiones
    QList<QGraphicsItem*> colisiones = collidingItems();
    for (QGraphicsItem *item : colisiones) {

        if (esDelJugador) {
            // Bala del jugador: impacta enemigos
            if (auto enemigo = dynamic_cast<EnemigoInfanteria*>(item)) {
                enemigo->recibirDisparo();
                deleteLater();
                return;
            }
        } else {
            // Bala enemiga: impacta al jugador
            if (auto jugador = dynamic_cast<JugadorInfanteria*>(item)) {
                jugador->recibirDisparo();
                deleteLater();
                return;
            }
        }
    }

    // 2) Movimiento recto
    setPos(x() + direccion * velocidad, y());

    // 3) Eliminar si sale del área visible (para balas del jugador)
    QRectF projRect = sceneBoundingRect();

    // Si no hay ninguna vista asociada, se usa el mundo completo como fallback
    QGraphicsView *view = scene()->views().isEmpty()
                              ? nullptr
                              : scene()->views().first();

    if (view) {
        // Rectángulo visible actualmente en pantalla
        QRectF visibleRect =
            view->mapToScene(view->viewport()->rect()).boundingRect();

        if (esDelJugador) {
            // Bala del jugador: se elimina cuando sale de la pantalla visible
            if (!visibleRect.intersects(projRect)) {
                deleteLater();
                return;
            }
        } else {
            // Bala enemiga: podemos seguir usando el mundo completo
            QRectF worldRect = scene()->sceneRect();
            if (!worldRect.intersects(projRect)) {
                deleteLater();
                return;
            }
        }
    } else {
        // Si por alguna razón no hay view, usamos el mundo como fallback
        QRectF worldRect = scene()->sceneRect();
        if (!worldRect.intersects(projRect)) {
            deleteLater();
            return;
        }
    }
}



