#ifndef EXPLOSION_H
#define EXPLOSION_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>
#include <QMediaPlayer>
#include <QAudioOutput>

class Explosion : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    Explosion(QGraphicsItem *parent = nullptr);

public slots:
    void nextFrame();

private:
    int frame;
    QTimer *timer;

    QMediaPlayer *explosionSound;
    QAudioOutput *explosionAudio;
};

#endif // EXPLOSION_H


