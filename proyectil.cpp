#include "proyectil.h"
#include <QTimer>
#include <QDebug>
#include <QList>
#include <QGraphicsScene>
#include "enemigo.h"
#include "NivelBatallaAerea.h"
#include "explosion.h"

extern NivelBatallaAerea *Juego;

proyectil::proyectil(QGraphicsItem *parent): EntidadMovil(parent){
    // Cargar el sprite original del proyectil
    QPixmap sprite(":/images/proyectil.png");

    // Escalar el sprite a un tamaño especifico
    QPixmap spriteEscalado = sprite.scaled(36, 36, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Dibujar el sprite escalado
    setPixmap(spriteEscalado);

    // Crear temporizador
    QTimer *temporizador = new QTimer(this);
    connect(temporizador, SIGNAL(timeout()), this, SLOT(move()));
    temporizador->start(50);
}

void proyectil::move()
{
    // Si el proyectil colisiona con un enemigo, destruir ambos
    QList<QGraphicsItem *> colliding_items = collidingItems();
    for(int i = 0, n = colliding_items.size(); i < n; ++i){
        if (typeid(*(colliding_items[i])) == typeid(enemigo)){

            // Incrementar score
            Juego->score->incrementar();

            // crear explosión en la posición del enemigo
            Explosion *boom = new Explosion();
            boom->setPos(colliding_items[i]->x(), colliding_items[i]->y());
            scene()->addItem(boom);

            // eliminar enemigo
            scene ()->removeItem(colliding_items[i]);
            delete colliding_items[i];

            // eliminar proyectil
            scene()->removeItem(this);
            delete this;
            return;

        }
    }

    // Movimiento hacia arriba
    setPos(x(), y() - 10);

    // Si sale de la pantalla, destruirlo
    if (pos().y() < 0){
        scene()->removeItem(this);
        delete this;
    }
}
