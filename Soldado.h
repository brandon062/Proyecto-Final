#ifndef SOLDADO_H
#define SOLDADO_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QElapsedTimer>

class Granada;
class ProyectilInfanteria;

class Soldado : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit Soldado(QGraphicsItem *parent = nullptr);

    void setGroundY(qreal gy) { groundY = gy; }

    // Control de movimiento horizontal
    void moverDerecha(bool activo);
    void moverIzquierda(bool activo);

    // Acciones básicas
    void saltar();
    virtual void disparar();
    virtual void lanzarGranada();

    // Utilidad
    bool estaEnElSuelo() const { return enSuelo; }

protected:
    qreal velocidadX;
    qreal velocidadY; // Altura del suelo
    qreal gravedad;
    qreal groundY;
    bool enSuelo;

    bool moviendoDerecha;
    bool moviendoIzquierda;

    int dirDisparo;         // +1 derecha, -1 izquierda

    QTimer *timerFisica;

protected slots:
    void actualizarFisica();
};

#endif // SOLDADO_H

