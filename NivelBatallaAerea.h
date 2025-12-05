#ifndef NIVELBATALLAAEREA_H
#define NIVELBATALLAAEREA_H

#include <QGraphicsView>
#include <QWidget>
#include <QGraphicsScene>
#include <QTimer>
#include <QGraphicsTextItem>
#include <QMediaPlayer>
#include <QAudioOutput>

#include "jugador.h"
#include "Score.h"
#include "Vida.h"

class NivelBatallaAerea: public QGraphicsView{

    Q_OBJECT
public:
    NivelBatallaAerea(QWidget * parent = 0);

    void gameOver();
    void winLevel();
    void resetLevel();                 // <-- reinicia el nivel
    bool isFinished() const { return finished; }

    void keyPressEvent(QKeyEvent *event) override;  // <-- capturamos la R aquí

    QGraphicsScene * scene;
    jugador * Jugador;
    Score * score;
    Vida * vida;

    QTimer *spawnTimer;
    QTimer *countdownTimer;
    QGraphicsTextItem *timerText;
    int remainingTime;
    bool finished;

    QMediaPlayer *bgMusic;
    QAudioOutput *bgAudio;

signals:
    void nivelCompletado(bool ganado);  // true = victoria, false = derrota
};


#endif // NIVELBATALLAAEREA_H


