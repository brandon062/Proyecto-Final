#include "Mina.h"
#include <QPixmap>

Mina::Mina(QGraphicsItem *parent)
    : QGraphicsPixmapItem(parent)
{
    QPixmap sprite(":/images/mina.png");
    QPixmap esc = sprite.scaled(40, 40,
                                Qt::KeepAspectRatio,
                                Qt::SmoothTransformation);
    setPixmap(esc);

    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
}
