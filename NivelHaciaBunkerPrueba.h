#ifndef NIVELHACIABUNKERPRUEBA_H
#define NIVELHACIABUNKERPRUEBA_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVector>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>

class JugadorInfanteria;
class EnemigoInfanteria;

class NivelHaciaBunkerPrueba : public QGraphicsView
{
    Q_OBJECT
public:
    explicit NivelHaciaBunkerPrueba(QWidget *parent = nullptr);

signals:
    void nivelCompletado(bool ganado);  // true = victoria, false = derrota
protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void actualizarJuego();      // lógica general del nivel (incluye HUD y cámara)

private:
    QGraphicsScene *scene;
    JugadorInfanteria *jugador;
    EnemigoInfanteria *enemigo;
    EnemigoInfanteria *enemigoFinal;  // jefe

    QTimer *timerJuego;          // timer del nivel

    // HUD de vidas
    QVector<QGraphicsPixmapItem*> corazonesHUD;

    // Dimensiones del mundo y de la vista
    qreal sceneWidth  = 3200;    // ancho del mundo (scroll horizontal)
    qreal sceneHeight = 450;     // alto del mundo
    qreal viewWidth   = 800;     // ancho de la ventana
    qreal viewHeight  = 450;     // alto de la ventana

    void inicializarHUDVidas();
    void actualizarHUDVidas(int vidas);

    // Cámara side-scroller
    void actualizarCamara();

    // Reposicionar HUD para que quede pegado a la pantalla al hacer scroll
    void actualizarHUDPosicion();
    qreal camXActual = 0.0;      // posición X actual de la cámara
    qreal camLerpFactor = 0.08;  // qué tan rápido sigue al jugador (0.0–1.0)

    // --- Sistema de Game Over ---
    bool juegoTerminado = false;
    QGraphicsTextItem *textoGameOver = nullptr;
    QTimer *timerGameOver = nullptr;

    void mostrarGameOver();
    void reiniciarNivel();

    QMediaPlayer *gameOverPlayer = nullptr;
    QAudioOutput *gameOverAudio  = nullptr;

    QMediaPlayer *musicaFondo = nullptr;
    QAudioOutput *audioFondo = nullptr;
    // Para alternar canciones
    bool reproduciendoFondo3 = true;

    // Helper para montar TODA la escena (se usara para reiniciar)
    void inicializarNivel();

    // Estado de victoria
    bool juegoGanado = false;
    QGraphicsTextItem *textoVictoria = nullptr;
    void mostrarVictoria();

    // Sonido de victoria
    QMediaPlayer *victoriaPlayer = nullptr;
    QAudioOutput *victoriaAudio  = nullptr;
};

#endif // NIVELHACIABUNKERPRUEBA_H



