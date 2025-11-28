#include "ProyectilTanque.h"
#include <QTimer>
#include <QGraphicsScene>
#include <QtMath>
#include <QList>

#include "Tanque.h"
#include "TanqueEnemigo.h"

// Explosión del nivel 1: ajusta el include según tu proyecto
#include "explosion.h"

ProyectilTanque::ProyectilTanque(const QPointF &direccion, bool esDelJugador, QGraphicsItem *parent)
    : QObject()
    , QGraphicsPixmapItem(parent)
    , dir(direccion)
    , velocidad(8.0)
    , delJugador(esDelJugador)
{
    // Normalizar
    double len = qSqrt(dir.x()*dir.x() + dir.y()*dir.y());
    if (len > 0.0001) {
        dir.setX(dir.x() / len);
        dir.setY(dir.y() / len);
    } else {
        dir = QPointF(0, -1);
    }

    QPixmap sprite(":/images/proyectil_tanque.png");
    QPixmap esc = sprite.scaled(10, 10,
                                Qt::KeepAspectRatio,
                                Qt::SmoothTransformation);
    setPixmap(esc);
    setTransformOriginPoint(boundingRect().center());

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ProyectilTanque::move);
    timer->start(16);
}

void ProyectilTanque::move()
{
    // Movimiento
    setPos(x() + dir.x() * velocidad,
           y() + dir.y() * velocidad);

    if (!scene())
        return;

    // === COLISIONES ===
    QList<QGraphicsItem *> coll = collidingItems();
    for (QGraphicsItem *item : coll) {
        if (delJugador) {
            // Proyectil del jugador golpea al enemigo
            TanqueEnemigo *enemigo = dynamic_cast<TanqueEnemigo*>(item);
            if (enemigo) {
                // Centro del enemigo
                QPointF centro = enemigo->mapToScene(enemigo->boundingRect().center());

                // Crear explosión y centrarla en el tanque
                Explosion *exp = new Explosion();
                scene()->addItem(exp);
                exp->setPos(
                    centro.x() - exp->boundingRect().width()  / 2.0,
                    centro.y() - exp->boundingRect().height() / 2.0
                    );

                scene()->removeItem(enemigo);
                delete enemigo;

                scene()->removeItem(this);
                delete this;
                return;
            }
        } else {
            // Proyectil enemigo golpea al jugador
            Tanque *jugador = dynamic_cast<Tanque*>(item);
            if (jugador) {
                // Centro del jugador
                QPointF centro = jugador->mapToScene(jugador->boundingRect().center());

                // Crear explosión y centrarla en el tanque
                Explosion *exp = new Explosion();
                scene()->addItem(exp);
                exp->setPos(
                    centro.x() - exp->boundingRect().width()  / 2.0,
                    centro.y() - exp->boundingRect().height() / 2.0
                    );

                jugador->recibirImpacto();

                scene()->removeItem(this);
                delete this;
                return;
            }
        }
    }

    // === Borrar si sale de pantalla ===
    if (pos().x() < -50 || pos().x() > 850 ||
        pos().y() < -50 || pos().y() > 650) {
        scene()->removeItem(this);
        delete this;
    }
}





