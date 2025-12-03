#include "ProyectilInfanteria.h"
#include <QGraphicsScene>
#include <QPixmap>

ProyectilInfanteria::ProyectilInfanteria(int dir, QGraphicsItem *parent)
    : QObject()
    , QGraphicsPixmapItem(parent)
    , direccion(dir)
    , velocidad(10.0)
{
    QPixmap sprite(":/images/proyectil_tanque.png");
    if (!sprite.isNull()) {
        QPixmap esc = sprite.scaled(16, 4, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        setPixmap(esc);
    }

    setZValue(5); // por encima del suelo

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout,
            this, &ProyectilInfanteria::mover);
    timer->start(16);
}

void ProyectilInfanteria::mover()
{
    setPos(x() + direccion * velocidad, y());

    if (!scene() ||
        x() < -100 || x() > scene()->width() + 100) {
        deleteLater();
    }
}

