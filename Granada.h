#ifndef GRANADA_H
#define GRANADA_H

#include <QObject>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QVector>
#include <QPixmap>
#include <QMediaPlayer>
#include <QAudioOutput>

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

    //  para no aplicar daño varias veces
    bool danioAplicado = false;

    // helper que aplica daño en radio
    void aplicarDanioExplosion();

    QMediaPlayer *explosionPlayer = nullptr;
    QAudioOutput *explosionAudio  = nullptr;
};

#endif // GRANADA_H


