#include "Score.h"
#include <QFont>
Score::Score(QGraphicsItem *parent): QGraphicsTextItem(parent){
    //Se inicializa el puntaje en 0
    score = 0;

    // dibujar el texto
    setPlainText(QString("Score: ") + QString:: number(score));
    setDefaultTextColor(Qt::blue);
    setFont(QFont("times",16));
}

void Score::incrementar(){
    score++;
    setPlainText(QString("Score: ") + QString:: number(score));
}

int Score::getScore(){
    return score;
}
