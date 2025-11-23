#include "jugador.h"
#include <QKeyEvent>
#include <QGraphicsScene>
#include "proyectil.h"
#include "enemigo.h"
#include "NivelBatallaAerea.h"
extern juego *Juego;
// MUY IMPORTANTE para el sonido:
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>

jugador::jugador(QGraphicsItem *parent): Avion(parent){
    // Inicializar sonido del proyectil
    proyectilsonido = new QMediaPlayer;
    audioProyectil  = new QAudioOutput;

    proyectilsonido->setAudioOutput(audioProyectil);
    audioProyectil->setVolume(0.7);  // volumen del disparo (0.0 - 1.0)
    proyectilsonido->setSource(QUrl("qrc:/sonidos/disparo_avion.mp3"));

    // --- Sprite del jugador (ESCALADO) ---
    QPixmap spriteJugador(":/images/avion_jugador.png");

    // Ajusta estos valores al tamaño que quieras para el avión
    QPixmap spriteEscalado = spriteJugador.scaled(100, 100,Qt::KeepAspectRatio,Qt::SmoothTransformation);

    setPixmap(spriteEscalado);
}

void jugador::keyPressEvent(QKeyEvent *evento)
{
    // Si el juego ya terminó (victoria o derrota)
    if (Juego->isFinished()) {
        if (evento->key() == Qt::Key_R) {
            // Reiniciar el juego: destruimos el actual y creamos uno nuevo
            delete Juego;
            Juego = new juego();
            Juego->show();
        }
        return; // no dejamos mover al jugador si el juego terminó
    }

    int ancho = pixmap().width();
    int alto  = pixmap().height();

    // Límite inferior y superior
    int limiteSuperior = 350;   // <-- Ajusta este valor si quieres
    int limiteInferior = 600 - alto;  // que no se salga de abajo

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

        proyectil *Proyectil = new proyectil();
        Proyectil->setPos(x() + ancho/2 - Proyectil->pixmap().width()/2, y());
        scene()->addItem(Proyectil);

        if (proyectilsonido->playbackState() == QMediaPlayer::PlayingState) {
            proyectilsonido->setPosition(0);
        } else {
            proyectilsonido->play();
        }
    }
}

void jugador::spawn()
{
    // crear un enemigo
    enemigo *Enemigo = new enemigo();
    scene()->addItem(Enemigo);
}


