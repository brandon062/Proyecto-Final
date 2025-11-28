#ifndef TANQUEENEMIGO_H
#define TANQUEENEMIGO_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>

class Tanque;  // forward declaration del tanque jugador

class TanqueEnemigo : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit TanqueEnemigo(Tanque *objetivo, QGraphicsItem *parent = nullptr);

public slots:
    void actualizarIA();
    void disparar();

private slots:
    void recargar();   // recarga del disparo (8s)

private:
    Tanque *objetivo;              // puntero al tanque jugador

    // Estado del chasis
    double anguloCuerpo;           // grados
    double velocidad;              // velocidad actual
    double velocidadMaxima;
    double velocidadGiro;

    // Torreta
    QGraphicsPixmapItem *torreta;
    double anguloTorretaRel;       // relativo al chasis
    double velocidadGiroTorreta;

    // Timers
    QTimer *iaTimer;
    QTimer *disparoTimer;
    QTimer *reloadTimer;

    // Disparo / recarga
    bool puedeDisparar;

    // Sonido de disparo enemigo
    QMediaPlayer *firePlayer;
    QAudioOutput *fireAudio;
};

#endif // TANQUEENEMIGO_H


