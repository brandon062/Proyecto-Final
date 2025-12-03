#ifndef NIVELHACIABUNKERPRUEBA_H
#define NIVELHACIABUNKERPRUEBA_H

#include <QGraphicsView>
#include <QGraphicsScene>

class JugadorInfanteria;
class EnemigoInfanteria;

class NivelHaciaBunkerPrueba : public QGraphicsView
{
    Q_OBJECT
public:
    explicit NivelHaciaBunkerPrueba(QWidget *parent = nullptr);

private:
    QGraphicsScene *scene;
    JugadorInfanteria *jugador;
    EnemigoInfanteria *enemigo;
};

#endif // NIVELHACIABUNKERPRUEBA_H

