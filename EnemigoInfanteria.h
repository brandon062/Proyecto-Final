#ifndef ENEMIGOINFANTERIA_H
#define ENEMIGOINFANTERIA_H

#include "Soldado.h"
#include <QPointF>
#include <QPixmap>
#include <QMediaPlayer>
#include <QAudioOutput>

class JugadorInfanteria;

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

    void recibirDisparo();


protected slots:
    void actualizarIA();
    void actualizarAnimacionEnemigo();

private:
    JugadorInfanteria *jugador;
    EstadoIA estadoActual;
    QTimer *timerIA;

    qreal xPatrullaInicio;
    qreal xPatrullaFin;

    QPointF ultimaPosicionVista;
    int tiempoBusqueda;

    QPixmap spriteDer;
    QPixmap spriteIzq;

    void cambiarEstado(EstadoIA nuevo);
    void comportamientoPatrulla();
    void comportamientoAlerta();
    void comportamientoPersecucion();
    void comportamientoBusqueda();

    bool jugadorVisible() const;
    qreal distanciaAJugador() const;

    enum AnimEnemyState {
        AnimIdleEnemy,
        AnimWalkEnemy,
        AnimShotEnemy,
        AnimDeadEnemy
    };

    AnimEnemyState estadoAnimEnemigo;

    //helper
    void actualizarEstadoAnimPorMovimiento();

    QVector<QPixmap> idleRightEnemy;
    QVector<QPixmap> idleLeftEnemy;

    QVector<QPixmap> walkRightEnemy;
    QVector<QPixmap> walkLeftEnemy;

    QVector<QPixmap> shotRightEnemy;
    QVector<QPixmap> shotLeftEnemy;

    QTimer *timerAnimEnemy;
    int frameEnemyIndex;
    int shotEnemyFramesPlayed;

    QElapsedTimer relojDisparoEnemigo;
    int cooldownDisparoEnemigoMs = 500;

    bool enemigoMuerto = false;

    QVector<QPixmap> deadRightEnemy;
    QVector<QPixmap> deadLeftEnemy;

    void iniciarAnimacionMuerteEnemigo();

    QMediaPlayer *damagePlayerEnemy = nullptr;
    QAudioOutput *damageAudioEnemy  = nullptr;

    QMediaPlayer *deathPlayerEnemy  = nullptr;
    QAudioOutput *deathAudioEnemy   = nullptr;
};

#endif // ENEMIGOINFANTERIA_H


