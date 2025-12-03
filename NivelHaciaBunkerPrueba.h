#ifndef NIVELHACIABUNKERPRUEBA_H
#define NIVELHACIABUNKERPRUEBA_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QVector>
#include <QGraphicsPixmapItem>
#include <QTimer>

class JugadorInfanteria;
class EnemigoInfanteria;

class NivelHaciaBunkerPrueba : public QGraphicsView
{
    Q_OBJECT
public:
    explicit NivelHaciaBunkerPrueba(QWidget *parent = nullptr);

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
    qreal sceneWidth  = 2400;    // ancho del mundo (scroll horizontal)
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
};

#endif // NIVELHACIABUNKERPRUEBA_H



