#ifndef PROYECTILINFANTERIA_H
#define PROYECTILINFANTERIA_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>

class ProyectilInfanteria : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    // dir: 1 derecha, -1 izquierda
    // esDelJugador: true = bala del jugador, false = bala de enemigo
    explicit ProyectilInfanteria(int dir, bool esDelJugador, QGraphicsItem *parent = nullptr);

private slots:
    void mover();

private:
    int   direccion;   // 1 derecha, -1 izquierda
    qreal velocidad;
    QTimer *timer;
    bool esDelJugador;
};

#endif // PROYECTILINFANTERIA_H
