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
    // esDelJugador: true = disparo del Sherman, false = disparo del Tiger
    explicit ProyectilTanque(const QPointF &direccion,
                             bool esDelJugador,
                             QGraphicsItem *parent = nullptr);

public slots:
    void move();

private:
    QPointF dir;        // vector dirección normalizado
    double velocidad;
    bool delJugador;
};

#endif // PROYECTILTANQUE_H




