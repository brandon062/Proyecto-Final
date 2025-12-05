#include "jugador.h"
#include <QKeyEvent>
#include <QGraphicsScene>
#include "proyectil.h"
#include "enemigo.h"
#include "NivelBatallaAerea.h"
extern NivelBatallaAerea *Juego;
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>

jugador::jugador(QGraphicsItem *parent): Avion(parent)
{
    // Inicializar sonido del proyectil
    proyectilsonido = new QMediaPlayer;
    audioProyectil  = new QAudioOutput;

    proyectilsonido->setAudioOutput(audioProyectil);
    audioProyectil->setVolume(0.7);
    proyectilsonido->setSource(QUrl("qrc:/sonidos/disparo_avion.mp3"));

    // Sprite del jugador (escalado)
    QPixmap spriteJugador(":/images/avion_jugador.png");
    QPixmap spriteEscalado = spriteJugador.scaled(
        100, 100,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    setPixmap(spriteEscalado);
}

void jugador::keyPressEvent(QKeyEvent *evento)
{
    // Bloquear movimiento cuando el nivel terminó
    if (Juego && Juego->isFinished()) {
        return;
    }

    int ancho = pixmap().width();
    int alto  = pixmap().height();

    int limiteSuperior = 350;
    int limiteInferior = 600 - alto;

    if (evento->key() == Qt::Key_Left) {
        if (pos().x() > 0)
            setPos(x() - 10, y());
    }
    else if (evento->key() == Qt::Key_Right) {
        if (pos().x() + ancho < 800)
            setPos(x() + 10, y());
    }
    else if (evento->key() == Qt::Key_Up) {
        if (pos().y() > limiteSuperior)
            setPos(x(), y() - 10);
    }
    else if (evento->key() == Qt::Key_Down) {
        if (pos().y() < limiteInferior)
            setPos(x(), y() + 10);
    }
    else if (evento->key() == Qt::Key_Space) {

        // Crear proyectil con referencia al nivel
        proyectil *Proyectil = new proyectil();
        Proyectil->setPos(
            x() + ancho/2 - Proyectil->pixmap().width()/2,
            y()
            );
        scene()->addItem(Proyectil);

        // sonido disparo
        if (proyectilsonido->playbackState() == QMediaPlayer::PlayingState) {
            proyectilsonido->setPosition(0);
        } else {
            proyectilsonido->play();
        }
    }
}

void jugador::spawn()
{
    enemigo *Enemigo = new enemigo();
    scene()->addItem(Enemigo);
}


