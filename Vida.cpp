
#include "Vida.h"
#include <QFont>
#include "NivelBatallaAerea.h"

extern juego *Juego;

Vida::Vida(QGraphicsItem *parent): QGraphicsTextItem(parent){
    vida = 3;

    setPlainText(QString("Salud: ") + QString:: number(vida));
    setDefaultTextColor(Qt::red);
    setFont(QFont("times",16));
}

void Vida::decrementar(){
    vida--;
    setPlainText(QString("Salud: ") + QString:: number(vida));

    if (vida <= 0){
        Juego->gameOver();
    }
}

int Vida::getVida(){
    return vida;
}

