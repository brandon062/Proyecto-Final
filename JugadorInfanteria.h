#ifndef JUGADORINFANTERIA_H
#define JUGADORINFANTERIA_H

#include "Soldado.h"
#include <QKeyEvent>
#include <QVector>
#include <QTimer>
#include <QElapsedTimer>
#include <QMediaPlayer>
#include <QAudioOutput>

class JugadorInfanteria : public Soldado
{
public:
    explicit JugadorInfanteria(QGraphicsItem *parent = nullptr);
    void recibirDisparo();
    int getVidas() const { return vidas; }
    bool estaMuerto() const { return vidas <= 0; }


protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    enum AnimState {
        AnimIdle,
        AnimWalk,
        AnimShot,
        AnimGrenade,
        AnimDead
    };

    int vidas = 3;

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

    QVector<QPixmap> deadRight;
    QVector<QPixmap> deadLeft;

    QTimer *timerAnim;
    int frameIndex;

    int shotFramesPlayed;
    int grenadeFramesPlayed;

    // cooldown de disparo (0.5s)
    QElapsedTimer relojDisparo;
    int cooldownDisparoMs;

    //cooldown de granada
    QElapsedTimer relojGranada;
    int cooldownGranadaMs = 7000;   // 7 segundos

    bool estaEnAccion() const;   // shot o grenade
    void iniciarAnimacionShot();
    void iniciarAnimacionGranada();
    void actualizarAnimacion();
    bool granadaLanzadaEnEstaAnim;

    bool muerteProcesada = false;

    //helper
    void iniciarAnimacionMuerte();

    void detenerTodosLosSonidos();

    double escalaJugador = 1.0;

    // Sonidos del jugador infantería
    QMediaPlayer *throwPlayer   = nullptr;  // lanzar granada
    QAudioOutput *throwAudio    = nullptr;

    QMediaPlayer *damagePlayer  = nullptr;  // recibir daño
    QAudioOutput *damageAudio   = nullptr;

    QMediaPlayer *deathPlayer   = nullptr;  // muerte jugador
    QAudioOutput *deathAudio    = nullptr;
};

#endif // JUGADORINFANTERIA_H


