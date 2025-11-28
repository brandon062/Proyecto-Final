#include "NivelBatallaTanque.h"
#include <QBrush>
#include <QImage>

NivelBatallaTanque::NivelBatallaTanque(QWidget *parent)
    : QGraphicsView(parent)
{
    scene = new QGraphicsScene();
    scene->setSceneRect(0, 0, 800, 600);
    setScene(scene);

    setBackgroundBrush(QBrush(QImage(":/images/bg2.png")));
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(800, 600);

    // ===== ESTRUCTURAS DEL MAPA =====
    estructura1 = new EstructuraMapa(":/images/edificio1.png");
    estructura1->setPos(80, 80);          // arriba-izquierda
    scene->addItem(estructura1);

    estructura2 = new EstructuraMapa(":/images/edificio2.png");
    estructura2->setPos(500, 60);         // arriba-derecha
    scene->addItem(estructura2);

    estructura3 = new EstructuraMapa(":/images/edificio3.png");
    estructura3->setPos(300, 360);        // zona central-baja
    scene->addItem(estructura3);

    estructura1 = new EstructuraMapa(":/images/edificio1.png");
    estructura1->setPos(80, 80);          // arriba-izquierda
    scene->addItem(estructura1);

    estructura2 = new EstructuraMapa(":/images/edificio2.png");
    estructura2->setPos(500, 60);         // arriba-derecha
    scene->addItem(estructura2);

    estructura3 = new EstructuraMapa(":/images/edificio3.png");
    estructura3->setPos(300, 360);        // zona central-baja
    scene->addItem(estructura3);

    // ===== MINAS =====
    mina1 = new Mina();
    mina1->setPos(250, 260);
    scene->addItem(mina1);

    mina2 = new Mina();
    mina2->setPos(500, 380);
    scene->addItem(mina2);

    mina3 = new Mina();
    mina3->setPos(130, 320);
    scene->addItem(mina3);

    mina4 = new Mina();
    mina4->setPos(600, 300);
    scene->addItem(mina4);

    // ===== Tanque jugador (Sherman) =====
    tanqueJugador = new Tanque();
    tanqueJugador->setFlag(QGraphicsItem::ItemIsFocusable);
    tanqueJugador->setFocus();
    tanqueJugador->setPos(400, 400);
    scene->addItem(tanqueJugador);

    // ===== Tanque enemigo (Tiger) =====
    tanqueEnemigo = new TanqueEnemigo(tanqueJugador);
    tanqueEnemigo->setPos(200, 100);  // posición inicial
    scene->addItem(tanqueEnemigo);

    show();
}

