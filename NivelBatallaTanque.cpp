#include "NivelBatallaTanque.h"
#include <QBrush>
#include <QImage>

NivelBatallaTanque::NivelBatallaTanque(QWidget *parent)
    : QGraphicsView(parent)
{
    scene = new QGraphicsScene();
    scene->setSceneRect(0, 0, 800, 600);
    setScene(scene);

    setBackgroundBrush(QBrush(QImage(":/images/nieve.png"))); // tu fondo
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(800, 600);

    // ===== Tanque jugador (Sherman) =====
    tanqueJugador = new Tanque();
    tanqueJugador->setFlag(QGraphicsItem::ItemIsFocusable);
    tanqueJugador->setFocus();
    tanqueJugador->setPos(400, 400);
    scene->addItem(tanqueJugador);

    // ===== Tanque enemigo (Tiger) =====
    tanqueEnemigo = new TanqueEnemigo(tanqueJugador);
    tanqueEnemigo->setPos(200, 100);  // posición inicial, ajusta a gusto
    scene->addItem(tanqueEnemigo);

    show();
}

