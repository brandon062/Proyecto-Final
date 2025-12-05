#include "enemigo.h"
#include <QTimer>
#include <QGraphicsScene>
#include <QObject>
#include <QList>
#include <stdlib.h>
#include "NivelBatallaAerea.h"
#include "proyectilEnemigo.h"
#include "jugador.h"
#include "explosion.h"
extern NivelBatallaAerea *Juego;

enemigo::enemigo(QGraphicsItem * parent): Avion(parent){
    // posición aleatoria en X
    int random_number = rand() % 700;
    setPos(random_number, 0);

    // cargar sprite original
    QPixmap sprite(":/images/enemigo.png");

    // ROTAR 180° para que apunte hacia abajo
    QTransform transform;
    transform.rotate(180);
    QPixmap spriteRotado = sprite.transformed(transform, Qt::SmoothTransformation);

    // ESCALAR
    QPixmap spriteEscalado = spriteRotado.scaled(
        100, 100,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation);

    setPixmap(spriteEscalado);

    // ------ SONIDO DE DISPARO ------
    sonidoDisparo = new QMediaPlayer(this);
    audioDisparo  = new QAudioOutput(this);
    sonidoDisparo->setAudioOutput(audioDisparo);
    audioDisparo->setVolume(0.6);

    sonidoDisparo->setSource(QUrl("qrc:/sonidos/disparo_avion.mp3"));
    // --------------------------------

    // timer para movimiento
    QTimer *temporizador = new QTimer(this);
    connect(temporizador, SIGNAL(timeout()), this, SLOT(move()));
    temporizador->start(50);

    // timer para DISPARAR cada 2 segundos
    QTimer *timerDisparo = new QTimer(this);
    connect(timerDisparo, SIGNAL(timeout()), this, SLOT(disparar()));
    timerDisparo->start(2000);
}

void enemigo::move()
{
    // Mover hacia abajo
    setPos(x(), y() + 5);

    // ---- detectar colisión con el jugador ----
    QList<QGraphicsItem*> colliding_items = collidingItems();
    for (int i = 0, n = colliding_items.size(); i < n; ++i) {
        if (typeid(*(colliding_items[i])) == typeid(jugador)) {

            // Quitar vida
            Explosion *boom = new Explosion();
            boom->setPos(colliding_items[i]->x(), colliding_items[i]->y());
            scene()->addItem(boom);

            // restar vida
            Juego->vida->decrementar();

            // borrar bala
            scene()->removeItem(this);
            delete this;
            return;
        }
    }

    // destruir enemigo si sale de pantalla
    if (pos().y() > 600) {
        Juego->vida->decrementar();
        scene()->removeItem(this);
        delete this;
    }
}

void enemigo::disparar()
{
    if (!scene()) return;

    // reproducir sonido
    if (sonidoDisparo->playbackState() == QMediaPlayer::PlayingState) {
        sonidoDisparo->setPosition(0);
    } else {
        sonidoDisparo->play();
    }

    // crear bala enemiga
    proyectilEnemigo *bala = new proyectilEnemigo();

    int anchoEnemigo = pixmap().width();
    int altoEnemigo  = pixmap().height();
    int anchoBala    = bala->pixmap().width();

    bala->setPos(
        x() + anchoEnemigo/2 - anchoBala/2,
        y() + altoEnemigo
        );

    scene()->addItem(bala);
}






