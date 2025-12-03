#ifndef ENEMIGOINFANTERIA_H
#define ENEMIGOINFANTERIA_H

#include "Soldado.h"
#include <QVector>
#include <QPixmap>
#include <QElapsedTimer>

class JugadorInfanteria;
class QTimer;

class EnemigoInfanteria : public Soldado
{
    Q_OBJECT

public:
    explicit EnemigoInfanteria(JugadorInfanteria *objetivo,
                               QGraphicsItem *parent = nullptr);

    enum EstadoIA {
        Patrulla,
        Alerta,
        Persecucion,
        Busqueda
    };

    virtual void recibirDisparo();
    bool estaMuerto() const { return enemigoMuerto; }

protected slots:
    void actualizarIA();
    void actualizarAnimacion();

protected:
    // Acceso para EnemigoFinalInfanteria
    QVector<QPixmap> idleRight, idleLeft;
    QVector<QPixmap> walkRight, walkLeft;
    QVector<QPixmap> shotRight, shotLeft;
    QVector<QPixmap> deadRight, deadLeft;

    bool enemigoMuerto = false;

    void iniciarAnimacionMuerte();

    // Animación
    enum AnimState {
        AnimIdle,
        AnimWalk,
        AnimShot,
        AnimDead
    };
    AnimState estadoAnim = AnimIdle;

    int frameIndex = 0;
    int shotFramesPlayed = 0;

    void actualizarEstadoMovimiento();

    // IA
    JugadorInfanteria *jugador;
    EstadoIA estado;

    QTimer *timerIA = nullptr;
    QTimer *timerAnim = nullptr;

    qreal xPatrullaInicio = 0;
    qreal xPatrullaFin = 0;

    QPointF ultimaPosicionVista;
    int tiempoBusqueda = 0;

    bool jugadorVisible() const;
    qreal distanciaAJugador() const;

    void comportamientoPatrulla();
    void comportamientoAlerta();
    void comportamientoPersecucion();
    void comportamientoBusqueda();
    void cambiarEstado(EstadoIA nuevo);

    // DISPARO
    QElapsedTimer relojDisparo;
    int cooldownMs = 800;

    // Sonidos
    QMediaPlayer *damagePlayer = nullptr;
    QAudioOutput *damageAudio = nullptr;

    QMediaPlayer *deathPlayer = nullptr;
    QAudioOutput *deathAudio = nullptr;
};

#endif // ENEMIGOINFANTERIA_H





