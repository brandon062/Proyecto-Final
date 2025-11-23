#include "MiRect.h"
#include <QKeyEvent>
#include "proyectil.h"
#include <QGraphicsScene>
#include "enemigo.h"
void MiRect::keyPressEvent(QKeyEvent *evento){
    if (evento ->key() == Qt::Key_Left){
        if (pos().x() >0)
        setPos(x()-10, y());
    }
    else if (evento ->key() == Qt::Key_Right){
        if (pos().x() + 100 < 800)
        setPos(x()+10, y());
    }
    else if (evento ->key() == Qt::Key_Space){
        // Se crea el Proyectil
        proyectil * Proyectil = new proyectil();
        Proyectil -> setPos(x(), y());
        scene()->addItem(Proyectil);
    }
}

void MiRect::spawn(){
    //crear un enemigo
    enemigo * Enemigo = new enemigo();
    scene()->addItem(Enemigo);
}
