// enemigo.h
#ifndef ENEMIGO_H
#define ENEMIGO_H

#include "Avion.h"
#include <QMediaPlayer>
#include <QAudioOutput>

class enemigo : public Avion {
    Q_OBJECT
public:
    enemigo(QGraphicsItem * parent = nullptr);

public slots:
    void move();
    void disparar();

private:
    QMediaPlayer *sonidoDisparo;
    QAudioOutput *audioDisparo;
};

#endif // ENEMIGO_H



