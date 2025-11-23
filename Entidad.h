#ifndef ENTIDAD_H
#define ENTIDAD_H

#include <QGraphicsPixmapItem>
#include <QObject>

// Clase base de todos los objetos del mundo del diagrama.
class Entidad : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT
public:
    Entidad(QGraphicsItem *parent = nullptr)
        : QObject(), QGraphicsPixmapItem(parent) {}

    virtual ~Entidad() {}
};

#endif // ENTIDAD_H
