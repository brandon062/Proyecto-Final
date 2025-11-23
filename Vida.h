#ifndef VIDA_H
#define VIDA_H
#include <QGraphicsTextItem>

class Vida: public QGraphicsTextItem{
public:
    Vida(QGraphicsItem * parent=0);
    void decrementar();
    int getVida();
private:
    int vida;
};


#endif // VIDA_H
