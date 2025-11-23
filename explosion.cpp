#include "explosion.h"
#include <QGraphicsScene>
#include <QPixmap>

#define ESCALA_ANCHO 100   // ajusta el tamaño aquí
#define ESCALA_ALTO  100

Explosion::Explosion(QGraphicsItem *parent)
    : QObject(), QGraphicsPixmapItem(parent)
{
    frame = 0;

    // ----- SONIDO DE EXPLOSION -----
    explosionSound = new QMediaPlayer(this);
    explosionAudio = new QAudioOutput(this);

    explosionSound->setAudioOutput(explosionAudio);
    explosionAudio->setVolume(0.8);  // volumen de la explosión

    // ruta en recursos (.qrc) – ajústala si tu prefix es distinto
    explosionSound->setSource(QUrl("qrc:/sonidos/explosion.mp3"));

    // reproducir una vez al crear la explosión
    explosionSound->play();
    // --------------------------------

    // Timer de animación
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
    timer->start(80); // velocidad de animación (ms por frame)

    // Frame 1 escalado
    QPixmap img(":/images/explosion1.png");
    setPixmap(img.scaled(ESCALA_ANCHO, ESCALA_ALTO,
                         Qt::KeepAspectRatio,
                         Qt::SmoothTransformation));
}

void Explosion::nextFrame()
{
    frame++;

    QPixmap img;

    switch (frame)
    {
    case 1:
        img.load(":/images/explosion2.png");
        break;

    case 2:
        img.load(":/images/explosion3.png");
        break;

    case 3:
        img.load(":/images/explosion4.png");
        break;

    case 4:
        // terminar animación
        if (scene()) {
            scene()->removeItem(this);
        }
        delete this;
        return;
    }

    setPixmap(img.scaled(ESCALA_ANCHO, ESCALA_ALTO,Qt::KeepAspectRatio,Qt::SmoothTransformation));
}


