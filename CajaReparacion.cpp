#include "CajaReparacion.h"
#include <QPixmap>

CajaReparacion::CajaReparacion(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
{
    QPixmap sprite(":/images/caja_reparacion.png");
    QPixmap esc = sprite.scaled(30, 30,
                                Qt::KeepAspectRatio,
                                Qt::SmoothTransformation);
    setPixmap(esc);

    // No marcamos flags de colisión sólida (no la tratamos como obstáculo)
}
