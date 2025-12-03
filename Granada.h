#ifndef GRANADA_H
#define GRANADA_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QVector>
#include <QPixmap>

class Granada : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit Granada(int dir, qreal groundY, QGraphicsItem *parent = nullptr);

private slots:
    void actualizar();

private:
    int direccion;
    qreal velX;
    qreal velY;
    qreal gravedad;
    qreal groundY;

    QTimer *timer;

    bool enExplosion;
    int explosionFrameIndex;
    QVector<QPixmap> explosionFrames;
};

#endif // GRANADA_H


