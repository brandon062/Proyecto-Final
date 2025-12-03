#ifndef JUGADORINFANTERIA_H
#define JUGADORINFANTERIA_H

#include "Soldado.h"
#include <QKeyEvent>
#include <QVector>
#include <QTimer>
#include <QElapsedTimer>

class JugadorInfanteria : public Soldado
{
public:
    explicit JugadorInfanteria(QGraphicsItem *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    enum AnimState {
        AnimIdle,
        AnimWalk,
        AnimShot,
        AnimGrenade
    };

    AnimState estadoAnim;

    // Frames por animación (derecha / izquierda)
    QVector<QPixmap> idleRight;
    QVector<QPixmap> idleLeft;

    QVector<QPixmap> walkRight;
    QVector<QPixmap> walkLeft;

    QVector<QPixmap> shotRight;
    QVector<QPixmap> shotLeft;

    QVector<QPixmap> grenadeRight;
    QVector<QPixmap> grenadeLeft;

    QTimer *timerAnim;
    int frameIndex;

    int shotFramesPlayed;
    int grenadeFramesPlayed;

    // cooldown de disparo (0.5s)
    QElapsedTimer relojDisparo;
    int cooldownDisparoMs;

    bool estaEnAccion() const;   // shot o grenade
    void iniciarAnimacionShot();
    void iniciarAnimacionGranada();
    void actualizarAnimacion();
    bool granadaLanzadaEnEstaAnim;

    double escalaJugador = 1.0;
};

#endif // JUGADORINFANTERIA_H


