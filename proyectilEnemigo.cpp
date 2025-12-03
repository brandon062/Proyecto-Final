#include "proyectilEnemigo.h"
#include <QTimer>
#include <QGraphicsScene>
#include <QList>
#include "jugador.h"
#include "NivelBatallaAerea.h"
#include "explosion.h"
extern juego *Juego;

proyectilEnemigo::proyectilEnemigo(QGraphicsItem *parent)
    : QObject(), QGraphicsPixmapItem(parent)
{
    QPixmap sprite(":/images/proyectil_tanque.png");

    // Escalar el sprite enemigo
    QPixmap spriteEscalado = sprite.scaled(10, 10,Qt::KeepAspectRatio,Qt::SmoothTransformation);
    setPixmap(spriteEscalado);

    // Timer para mover la bala
    QTimer *temporizador = new QTimer(this);
    connect(temporizador, SIGNAL(timeout()), this, SLOT(move()));
    temporizador->start(50);
}

void proyectilEnemigo::move()
{
    // Revisar colisión con el jugador
    QList<QGraphicsItem *> colliding_items = collidingItems();
    for (int i = 0, n = colliding_items.size(); i < n; ++i) {
        if (typeid(*(colliding_items[i])) == typeid(jugador)) {

            // Quitar vida
            // explosion en jugador
            Explosion *boom = new Explosion();
            boom->setPos(colliding_items[i]->x(), colliding_items[i]->y());
            scene()->addItem(boom);

            // restar vida
            Juego->vida->decrementar();

            // borrar bala
            scene()->removeItem(this);
            delete this;
            return;
        }
    }

    // Mover hacia abajo
    setPos(x(), y() + 10);

    // Si sale de la pantalla, destruir
    if (pos().y() > 600) {
        scene()->removeItem(this);
        delete this;
    }
}

