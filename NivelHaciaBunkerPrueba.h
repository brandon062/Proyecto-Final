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
    void actualizarJuego();      // para ir actualizando el HUD de vidas

private:
    QGraphicsScene *scene;
    JugadorInfanteria *jugador;
    EnemigoInfanteria *enemigo;

    QTimer *timerJuego;          // timer del nivel

    // HUD de vidas
    QVector<QGraphicsPixmapItem*> corazonesHUD;

    void inicializarHUDVidas();
    void actualizarHUDVidas(int vidas);
};

#endif // NIVELHACIABUNKERPRUEBA_H


