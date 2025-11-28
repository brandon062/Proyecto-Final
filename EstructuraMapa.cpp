#include "EstructuraMapa.h"
#include <QPixmap>

EstructuraMapa::EstructuraMapa(const QString &rutaSprite, QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
{
    QPixmap sprite(rutaSprite);
    QPixmap esc = sprite.scaled(140, 140,
                                Qt::KeepAspectRatio,
                                Qt::SmoothTransformation);
    setPixmap(esc);
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
}
