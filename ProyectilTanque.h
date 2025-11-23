#ifndef PROYECTILTANQUE_H
#define PROYECTILTANQUE_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QPointF>

class ProyectilTanque : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    // direccion: vector que indica hacia dónde viaja la bala
    explicit ProyectilTanque(const QPointF &direccion, QGraphicsItem *parent = nullptr);

public slots:
    void move();

private:
    QPointF dir;    // vector dirección
    double velocidad;
};

#endif // PROYECTILTANQUE_H



