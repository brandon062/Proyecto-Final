#ifndef ESTRUCTURAMAPA_H
#define ESTRUCTURAMAPA_H

#include <QGraphicsPixmapItem>
#include <QString>

class EstructuraMapa : public QGraphicsPixmapItem
{
public:
    explicit EstructuraMapa(const QString &rutaSprite,
                            QGraphicsItem *parent = nullptr);
};

#endif // ESTRUCTURAMAPA_H

