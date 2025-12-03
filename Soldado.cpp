#include "Soldado.h"
#include "ProyectilInfanteria.h"
#include "Granada.h"
#include <QGraphicsScene>

Soldado::Soldado(QGraphicsItem *parent)
    : QObject()
    , QGraphicsPixmapItem(parent)
    , velocidadX(0.0)
    , velocidadY(0.0)
    , gravedad(0.8)
    , groundY(400.0)
    , enSuelo(false)
    , moviendoDerecha(false)
    , moviendoIzquierda(false)
    , dirDisparo(1)          // por defecto mira a la derecha
{
    timerFisica = new QTimer(this);
    connect(timerFisica, &QTimer::timeout,
            this, &Soldado::actualizarFisica);
    timerFisica->start(16);
}

void Soldado::moverDerecha(bool activo)
{
    moviendoDerecha = activo;
}

void Soldado::moverIzquierda(bool activo)
{
    moviendoIzquierda = activo;
}

void Soldado::saltar()
{
    if (enSuelo) {
        velocidadY = -15.0;   // impulso hacia arriba
        enSuelo = false;
    }
}

void Soldado::disparar()
{
    if (!scene()) return;

    ProyectilInfanteria *bala = new ProyectilInfanteria(dirDisparo);

    qreal px = x() + (dirDisparo > 0 ? boundingRect().width() : 0);
    qreal py = y() + boundingRect().height() * 0.6;
    bala->setPos(px, py);

    scene()->addItem(bala);
}

void Soldado::lanzarGranada()
{
    if (!scene()) return;

    // Crear la granada con la dirección actual y el groundY del soldado
    Granada *g = new Granada(dirDisparo, groundY);

    // Posición X: un poco delante del soldado
    qreal gx;
    if (dirDisparo > 0) {
        gx = x() + boundingRect().width() * 0.5;
    } else {
        gx = x() + boundingRect().width() * 0.1;
    }

    // Pies del soldado (coordenada Y del suelo donde pisa)
    qreal piesY = y() + boundingRect().height();

    const qreal alturaGranada = 175;

    // La granada aparece justo un poquito por encima del suelo
    qreal gy = piesY - alturaGranada - 2;

    g->setPos(gx, gy);
    scene()->addItem(g);
}




void Soldado::actualizarFisica()
{
    qreal vx = 0.0;
    if (moviendoDerecha) vx += 4.0;
    if (moviendoIzquierda) vx -= 4.0;

    velocidadY += gravedad;

    qreal nuevoX = x() + vx;
    qreal nuevoY = y() + velocidadY;

    if (nuevoY + boundingRect().height() >= groundY) {
        nuevoY = groundY - boundingRect().height();
        velocidadY = 0.0;
        enSuelo = true;
    } else {
        enSuelo = false;
    }

    setPos(nuevoX, nuevoY);
}
