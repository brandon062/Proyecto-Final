#include "Granada.h"
#include <QGraphicsScene>
#include <QPixmap>
#include <QDebug>

// ================================
//   TAMAÑOS AJUSTABLES
// ================================
static const int GRANADA_W   = 150;   // ancho de la granada en el aire
static const int GRANADA_   = 150;   // alto de la granada en el aire
static const int EXPLOSION_W = 160;   // ancho de cada frame de explosión
static const int EXPLOSION_H = 160;   // alto de cada frame de explosión

Granada::Granada(int dir, qreal gY, QGraphicsItem *parent)
    : QObject()
    , QGraphicsPixmapItem(parent)
    , direccion(dir)
    , velX(6.0 * dir)
    , velY(-12.0)
    , gravedad(0.7)
    , groundY(gY)
    , timer(new QTimer(this))
    , enExplosion(false)
    , explosionFrameIndex(0)
{
    // Cargar frames de explosión (explosion_1.png ... explosion_9.png)
    for (int i = 1; i <= 9; ++i) {
        QString path = QString(":/images/explosion_%1.png").arg(i);
        QPixmap p(path);
        if (!p.isNull())
            explosionFrames.append(p);
        else
            qDebug() << "NO CARGÓ explosion frame:" << path;
    }

    // Usaremos el primer frame como sprite de granada en el aire
    if (!explosionFrames.isEmpty()) {
        QPixmap esc = explosionFrames[0].scaled(
            GRANADA_W, GRANADA_,
            Qt::KeepAspectRatio,
            Qt::SmoothTransformation);
        setPixmap(esc);
    }

    setZValue(5);

    connect(timer, &QTimer::timeout, this, &Granada::actualizar);
    timer->start(40);
}

void Granada::actualizar()
{
    if (!scene()) {
        deleteLater();
        return;
    }

    // ============================
    //  FASE DE EXPLOSIÓN
    // ============================
    if (enExplosion) {
        if (explosionFrameIndex < explosionFrames.size()) {

            QPixmap esc = explosionFrames[explosionFrameIndex].scaled(
                EXPLOSION_W, EXPLOSION_H,
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation);

            setPixmap(esc);
            explosionFrameIndex++;
        } else {
            deleteLater();
        }
        return;
    }

    // ============================
    //  FASE EN EL AIRE (PARÁBOLA)
    // ============================

    velY += gravedad;

    qreal nx = x() + velX;
    qreal ny = y() + velY;
    qreal h  = boundingRect().height();

    // Tocar suelo: cuando la parte de abajo llega a groundY
    if (velY > 0 && ny + h >= groundY) {
        ny = groundY - h;       // ajusta para que justo toque el suelo
        setPos(nx, ny);

        // Activar explosión
        enExplosion = true;
        explosionFrameIndex = 0;
        velX = 0;
        velY = 0;
        return;
    }

    setPos(nx, ny);

    // Si se sale de la pantalla por abajo o lados, la eliminamos
    if (x() < -100 || x() > scene()->width() + 100 ||
        y() > scene()->height() + 100) {
        deleteLater();
    }
}





