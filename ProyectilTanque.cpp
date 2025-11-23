#include "ProyectilTanque.h"
#include <QTimer>
#include <QGraphicsScene>
#include <QtMath>   // qSqrt

ProyectilTanque::ProyectilTanque(const QPointF &direccion, QGraphicsItem *parent)
    : QObject()
    , QGraphicsPixmapItem(parent)
    , dir(direccion)
    , velocidad(8.0)          // velocidad del proyectil
{
    // Normalizar el vector dirección
    double len = qSqrt(dir.x()*dir.x() + dir.y()*dir.y());
    if (len > 0.0001) {
        dir.setX(dir.x() / len);
        dir.setY(dir.y() / len);
    } else {
        dir = QPointF(0, -1);   // por si acaso
    }

    // Sprite del proyectil
    QPixmap sprite(":/images/proyectil_tanque.png");
    QPixmap esc = sprite.scaled(10, 10,
                                Qt::KeepAspectRatio,
                                Qt::SmoothTransformation);
    setPixmap(esc);
    setTransformOriginPoint(boundingRect().center());

    // Timer para mover el proyectil
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &ProyectilTanque::move);
    timer->start(16); // ~60 FPS
}

void ProyectilTanque::move()
{
    // Mover en la dirección indicada
    setPos(x() + dir.x() * velocidad,
           y() + dir.y() * velocidad);

    if (!scene()) return;

    // Si sale de la pantalla, destruirlo
    if (pos().x() < -50 || pos().x() > 850 ||
        pos().y() < -50 || pos().y() > 650) {
        scene()->removeItem(this);
        delete this;
    }
}




