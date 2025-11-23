#include "NivelBatallaTanque.h"
#include <QBrush>
#include <QImage>

NivelBatallaTanque::NivelBatallaTanque(QWidget *parent)
    : QGraphicsView(parent)
{
    // Crear escena
    scene = new QGraphicsScene();
    scene->setSceneRect(0, 0, 800, 600);
    setScene(scene);

    // Fondo de nieve (usa la imagen que quieras)
    setBackgroundBrush(Qt::darkGreen);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(800, 600);

    // ====== Agregar tanque jugador ======
    tanqueJugador = new Tanque();
    tanqueJugador->setFlag(QGraphicsItem::ItemIsFocusable);
    tanqueJugador->setFocus();
    tanqueJugador->setPos(400, 300);

    scene->addItem(tanqueJugador);

    show();
}
