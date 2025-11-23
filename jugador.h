#ifndef JUGADOR_H
#define JUGADOR_H

#include "Avion.h"
#include <QMediaPlayer>
#include <QAudioOutput>

class jugador : public Avion {
    Q_OBJECT
public:
    jugador(QGraphicsItem * parent = nullptr);
    void keyPressEvent(QKeyEvent * evento) override;

public slots:
    void spawn();

private:
    QMediaPlayer * proyectilsonido;
    QAudioOutput * audioProyectil;
};

#endif // JUGADOR_H


