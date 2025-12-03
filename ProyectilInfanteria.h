#ifndef PROYECTILINFANTERIA_H
#define PROYECTILINFANTERIA_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>

class ProyectilInfanteria : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit ProyectilInfanteria(int dir, QGraphicsItem *parent = nullptr);

private slots:
    void mover();

private:
    int direccion;   // 1 derecha, -1 izquierda
    qreal velocidad;
    QTimer *timer;
};

#endif // PROYECTILINFANTERIA_H

