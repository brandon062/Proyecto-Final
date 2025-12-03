#include "NivelHaciaBunkerPrueba.h"
#include "JugadorInfanteria.h"
#include "EnemigoInfanteria.h"
#include <QBrush>
#include <QImage>
#include <QGraphicsRectItem>

NivelHaciaBunkerPrueba::NivelHaciaBunkerPrueba(QWidget *parent)
    : QGraphicsView(parent)
    , scene(new QGraphicsScene(this))
    , jugador(nullptr)
    , enemigo(nullptr)
{
    scene->setSceneRect(0, 0, 800, 450); // un poco menos de alto para dejar HUD si luego quieres
    setScene(scene);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(800, 450);

    // Fondo estático de prueba
    scene->setBackgroundBrush(QBrush(QImage(":/images/fondo_bunker.png")));

    // "Suelo"
    QGraphicsRectItem *suelo = new QGraphicsRectItem(0, 400, 800, 50);
    suelo->setBrush(Qt::darkGreen);
    suelo->setPen(Qt::NoPen);
    scene->addItem(suelo);

    // Jugador
    jugador = new JugadorInfanteria();
    jugador->setPos(100, 300);
    jugador->setGroundY(400); // mismo Y del suelo
    scene->addItem(jugador);

    // Enemigo
    enemigo = new EnemigoInfanteria(jugador);
    enemigo->setPos(550, 300);
    enemigo->setGroundY(400);
    scene->addItem(enemigo);
}
