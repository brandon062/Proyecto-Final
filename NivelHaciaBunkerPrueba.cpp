#include "NivelHaciaBunkerPrueba.h"
#include "JugadorInfanteria.h"
#include "EnemigoInfanteria.h"
#include "EnemigoFinalInfanteria.h"

#include <QBrush>
#include <QImage>
#include <QGraphicsRectItem>
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QDebug>

NivelHaciaBunkerPrueba::NivelHaciaBunkerPrueba(QWidget *parent)
    : QGraphicsView(parent)
    , scene(new QGraphicsScene(this))
    , jugador(nullptr)
    , enemigo(nullptr)
    , timerJuego(nullptr)
{
    // Dimensiones del mundo
    scene->setSceneRect(0, 0, sceneWidth, sceneHeight);
    setScene(scene);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(viewWidth, viewHeight);

    // Fondo estático
    scene->setBackgroundBrush(QBrush(QImage(":/images/bg3.png")));

    // "Suelo" a lo largo de todo el mundo
    qreal groundY = 400;
    QGraphicsRectItem *suelo = new QGraphicsRectItem(0, groundY, sceneWidth, sceneHeight - groundY);
    suelo->setBrush(Qt::black);
    suelo->setPen(Qt::NoPen);
    scene->addItem(suelo);

    // Jugador
    jugador = new JugadorInfanteria();
    jugador->setPos(100, 300);
    jugador->setGroundY(groundY);
    scene->addItem(jugador);

    // Inicializar posición de cámara en torno al jugador
    qreal jugadorCenterX = jugador->x() + jugador->boundingRect().width() / 2.0;
    qreal halfViewW      = viewWidth / 2.0;
    qreal minCenterX     = halfViewW;
    qreal maxCenterX     = sceneWidth - halfViewW;

    if (jugadorCenterX < minCenterX) jugadorCenterX = minCenterX;
    if (jugadorCenterX > maxCenterX) jugadorCenterX = maxCenterX;

    camXActual = jugadorCenterX;

    // Enemigo normal
    EnemigoInfanteria *enemigo1 = new EnemigoInfanteria(jugador);
    enemigo1->setPos(600, groundY - enemigo1->boundingRect().height());
    scene->addItem(enemigo1);

    EnemigoFinalInfanteria *boss = new EnemigoFinalInfanteria(jugador);
    qreal bossX = sceneWidth - 200; // rincón derecho
    qreal bossY = groundY - boss->boundingRect().height();
    boss->setPos(bossX, bossY);
    scene->addItem(boss);

    // HUD de vidas (corazones)
    inicializarHUDVidas();
    actualizarHUDPosicion();

    // Cámara inicial centrada cerca del jugador (sin salirse de los límites)
    actualizarCamara();

    // Timer del nivel para actualizar cámara y HUD (y futura lógica general)
    timerJuego = new QTimer(this);
    connect(timerJuego, &QTimer::timeout,
            this, &NivelHaciaBunkerPrueba::actualizarJuego);
    timerJuego->start(40); // ~25 FPS para refrescar HUD y cámara
}

void NivelHaciaBunkerPrueba::inicializarHUDVidas()
{
    // Limpiar si hubiera algo previo
    for (auto *c : corazonesHUD) {
        if (scene) {
            scene->removeItem(c);
        }
        delete c;
    }
    corazonesHUD.clear();

    // Cargar corazón y escalarlo para que se vea pequeño
    QPixmap corazonOriginal(":/images/corazon.png");
    if (corazonOriginal.isNull()) {
        qDebug() << "No se pudo cargar :/images/corazon.png";
        return;
    }

    int heartW = 22;
    int heartH = 22;

    QPixmap corazon = corazonOriginal.scaled(
        heartW, heartH,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation);

    // 3 corazones, su posición real se ajusta en actualizarHUDPosicion()
    for (int i = 0; i < 3; ++i) {
        auto *item = new QGraphicsPixmapItem(corazon);
        item->setZValue(100); // por encima de todo
        scene->addItem(item);
        corazonesHUD.append(item);
    }

    if (jugador) {
        actualizarHUDVidas(jugador->getVidas());
    }
}

void NivelHaciaBunkerPrueba::actualizarHUDVidas(int vidas)
{
    // Se asume máximo 3 vidas
    for (int i = 0; i < corazonesHUD.size(); ++i) {
        if (corazonesHUD[i]) {
            corazonesHUD[i]->setVisible(i < vidas);
        }
    }
}

void NivelHaciaBunkerPrueba::actualizarCamara()
{
    if (!jugador) return;

    // Centro deseado según el jugador
    qreal jugadorCenterX = jugador->x() + jugador->boundingRect().width() / 2.0;

    qreal halfViewW  = viewWidth / 2.0;
    qreal minCenterX = halfViewW;
    qreal maxCenterX = sceneWidth - halfViewW;

    // Clampeamos el target
    qreal targetX = jugadorCenterX;
    if (targetX < minCenterX) targetX = minCenterX;
    if (targetX > maxCenterX) targetX = maxCenterX;

    // LERP suave: acercar camXActual hacia targetX
    camXActual = camXActual + (targetX - camXActual) * camLerpFactor;

    // Y fija en Y
    qreal camCenterY = sceneHeight / 2.0;

    centerOn(camXActual, camCenterY);
}


void NivelHaciaBunkerPrueba::actualizarHUDPosicion()
{
    if (corazonesHUD.isEmpty())
        return;

    // Medidas del corazón (todos tienen el mismo tamaño)
    int heartW = corazonesHUD[0]->pixmap().width();
    int heartH = corazonesHUD[0]->pixmap().height();

    int marginLeft   = 10;
    int marginBottom = 10;

    // Reposicionar los corazones en coordenadas de escena
    // pero anclados a la esquina inferior izquierda de la VISTA
    for (int i = 0; i < corazonesHUD.size(); ++i) {

        int viewportX = marginLeft + i * (heartW + 5);
        int viewportY = viewport()->height() - heartH - marginBottom;

        // Convertimos coordenadas de viewport a coordenadas de escena
        QPointF scenePos = mapToScene(QPoint(viewportX, viewportY));
        corazonesHUD[i]->setPos(scenePos);
    }
}

void NivelHaciaBunkerPrueba::actualizarJuego()
{
    // Actualizar cámara (side-scroller)
    actualizarCamara();

    // Mantener el HUD pegado a la pantalla aunque la cámara se mueva
    actualizarHUDPosicion();

    // Actualizar HUD de vidas según el estado actual del jugador
    if (jugador) {
        actualizarHUDVidas(jugador->getVidas());
    }

}


