#ifndef TANQUE_H
#define TANQUE_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QTimer>

class Tanque : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit Tanque(QGraphicsItem *parent = nullptr);

    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

    void recibirImpacto();
    int getVidas() const { return vidas; }
    void setVidas(int v);
    void setCongelado(bool c);

signals:
    void vidasCambiaron(int vidasRestantes);
    void jugadorMurio();

public slots:
    void actualizarFisica();
    void disparar();

private slots:
    void recargar();   // se llama cuando termina el tiempo de recarga

private:
    // Estado físico del chasis
    double anguloCuerpo;       // grados (0 = tanque mirando hacia arriba)
    double velocidad;

    // Parámetros de movimiento
    double aceleracion;
    double friccion;
    double velocidadMaxima;
    double velocidadReversaMax;
    double velocidadGiro;

    // Torreta
    QGraphicsPixmapItem *torreta;
    double anguloTorretaRel;   // ángulo RELATIVO al chasis

    // Flags de entrada (teclas)
    bool girarIzquierda;
    bool girarDerecha;
    bool avanzar;
    bool retroceder;
    bool girarTorretaIzq;
    bool girarTorretaDer;

    // Sonidos
    QMediaPlayer *motorLoopPlayer;
    QAudioOutput *motorLoopAudio;

    QMediaPlayer *drivingPlayer;
    QAudioOutput *drivingAudio;

    QMediaPlayer *turretPlayer;
    QAudioOutput *turretAudio;

    QMediaPlayer *firePlayer;
    QAudioOutput *fireAudio;

    QMediaPlayer *repairPlayer;
    QAudioOutput *repairAudio;

    // Recarga de disparo
    bool puedeDisparar;
    QTimer *reloadTimer;

    // Vidas del jugador
    int vidas;
    bool destruido;
    bool congelado;
};

#endif // TANQUE_H






