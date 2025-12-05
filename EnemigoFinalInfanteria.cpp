#include "EnemigoFinalInfanteria.h"
#include <QGraphicsScene>
#include <QTimer>
#include <QTransform>

EnemigoFinalInfanteria::EnemigoFinalInfanteria(JugadorInfanteria *objetivo,
                                               QGraphicsItem *parent)
    : EnemigoInfanteria(objetivo, parent)
{
    QSize size(120, 140);

    idleRight.clear();
    idleLeft.clear();
    walkRight.clear();
    walkLeft.clear();
    shotRight.clear();
    shotLeft.clear();
    deadRight.clear();
    deadLeft.clear();

    // Idle: idle_1_final.png ... idle_9_final.png
    for (int i = 1; i <= 7; ++i) {
        QPixmap p(QString(":/images/idle_%1_final.png").arg(i));
        if (!p.isNull()) {
            QPixmap e = p.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            idleRight.append(e);
            idleLeft.append(e.transformed(QTransform().scale(-1, 1)));
        }
    }

    // Walk: walk_1_final.png ... walk_8_final.png
    for (int i = 1; i <= 8; ++i) {
        QPixmap p(QString(":/images/walk_%1_final.png").arg(i));
        if (!p.isNull()) {
            QPixmap e = p.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            walkRight.append(e);
            walkLeft.append(e.transformed(QTransform().scale(-1, 1)));
        }
    }

    // Shot: shot_1_final.png ... shot_4_final.png
    for (int i = 1; i <= 4; ++i) {
        QPixmap p(QString(":/images/shot_%1_final.png").arg(i));
        if (!p.isNull()) {
            QPixmap e = p.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            shotRight.append(e);
            shotLeft.append(e.transformed(QTransform().scale(-1, 1)));
        }
    }

    // Dead: dead_1_final.png ... dead_5_final.png
    for (int i = 1; i <= 5; ++i) {
        QPixmap p(QString(":/images/dead_%1_final.png").arg(i));
        if (!p.isNull()) {
            QPixmap e = p.scaled(size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            deadRight.append(e);
            deadLeft.append(e.transformed(QTransform().scale(-1, 1)));
        }
    }

    // Frame inicial
    if (!idleRight.isEmpty())
        setPixmap(idleRight[0]);
}

void EnemigoFinalInfanteria::recibirDisparo()
{
    if (enemigoMuerto)
        return;

    // Sonido de daño (heredado)
    if (damagePlayer) {
        if (damagePlayer->playbackState() == QMediaPlayer::PlayingState)
            damagePlayer->setPosition(0);
        else
            damagePlayer->play();
    }

    vidaRestante--;

    // Mientras tenga vida, no muere aun
    if (vidaRestante > 0)
        return;

    enemigoMuerto = true;

    moverIzquierda(false);
    moverDerecha(false);

    iniciarAnimacionMuerte();

    if (deathPlayer)
        deathPlayer->play();

    // Borrarlo después de la animación
    QTimer::singleShot(4000, this, [this](){
        if (scene())
            scene()->removeItem(this);
        delete this;
    });
}

