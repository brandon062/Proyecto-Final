#ifndef NIVELBATALLATANQUE_H
#define NIVELBATALLATANQUE_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QTimer>
#include <QGraphicsTextItem>
#include <QGraphicsPixmapItem>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QKeyEvent>

#include "Tanque.h"
#include "TanqueEnemigo.h"
#include "EstructuraMapa.h"
#include "Mina.h"

class NivelBatallaTanque : public QGraphicsView
{
    Q_OBJECT
public:
    explicit NivelBatallaTanque(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void verificarProgreso();
    void cargarNivel2();
    void cargarNivel3();

    void actualizarVidasUI(int vidasRestantes);
    void manejarGameOver();
    void onBgStatusChanged(QMediaPlayer::MediaStatus status);

private:
    // configuración de escenas
    void configurarNivel1();
    void configurarNivel2();
    void configurarNivel3();
    void mostrarPantallaCargando();

    // UI vidas
    void crearVidasUI();

    QGraphicsScene *scene;
    Tanque *tanqueJugador;

    TanqueEnemigo *tanqueEnemigo1;
    TanqueEnemigo *tanqueEnemigo2;
    TanqueEnemigo *tanqueEnemigo3;

    EstructuraMapa *estructura1;
    EstructuraMapa *estructura2;
    EstructuraMapa *estructura3;

    Mina *mina1;
    Mina *mina2;
    Mina *mina3;
    Mina *mina4;

    // Iconos de vidas
    QGraphicsPixmapItem *vidaIcon1;
    QGraphicsPixmapItem *vidaIcon2;
    QGraphicsPixmapItem *vidaIcon3;

    QTimer *timerProgreso;
    int nivelActual;
    bool enTransicion;

    QGraphicsTextItem *mensajeAvanzar;
    bool mensajeMostrado;

    // Game Over
    bool enGameOver;
    QMediaPlayer *gameOverPlayer;
    QAudioOutput *gameOverAudio;

    // Música de fondo
    QMediaPlayer *bgPlayer;
    QAudioOutput *bgAudio;
    int bgTrackIndex;  // 1 o 2: fondo1 / fondo2

    // Música de misión completada
    QMediaPlayer *missionPlayer;
    QAudioOutput *missionAudio;
    bool misionCompletada;

    // Helpers
    void detenerMusicaFondo();
    void mostrarMisionCompletada();

    int vidasGlobal;  // vidas compartidas entre niveles
};

#endif // NIVELBATALLATANQUE_H





