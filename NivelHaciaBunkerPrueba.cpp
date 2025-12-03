#include "NivelHaciaBunkerPrueba.h"
#include "JugadorInfanteria.h"
#include "EnemigoInfanteria.h"

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
    scene->setSceneRect(0, 0, 800, 450); // un poco menos de alto para dejar HUD
    setScene(scene);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(800, 450);

    // Fondo estático de prueba
    scene->setBackgroundBrush(QBrush(QImage(":/images/fondo_bunker.png")));

    // "Suelo"
    QGraphicsRectItem *suelo = new QGraphicsRectItem(0, 400, 800, 50);
    suelo->setBrush(Qt::darkGreen);
    suelo->setPen(Qt::NoPen);
    scene->addItem(suelo);

    // Jugador
    jugador = new JugadorInfanteria();
    jugador->setPos(100, 300);
    jugador->setGroundY(400); // mismo Y del suelo
    scene->addItem(jugador);

    // Enemigo
    enemigo = new EnemigoInfanteria(jugador);
    enemigo->setPos(550, 300);
    enemigo->setGroundY(400);
    scene->addItem(enemigo);

    // HUD de vidas (corazones)
    inicializarHUDVidas();

    // Timer del nivel para actualizar HUD (y futuro update general si quieres)
    timerJuego = new QTimer(this);
    connect(timerJuego, &QTimer::timeout,
            this, &NivelHaciaBunkerPrueba::actualizarJuego);
    timerJuego->start(40); // ~25 FPS para refrescar HUD
}

void NivelHaciaBunkerPrueba::inicializarHUDVidas()
{
    for (auto *c : corazonesHUD) {
        if (scene) scene->removeItem(c);
        delete c;
    }
    corazonesHUD.clear();

    // Cargar corazón y escalarlo igual que el HUD del nivel del tanque
    QPixmap corazonOriginal(":/images/corazon.png");

    int heartW = 27;
    int heartH = 27;

    QPixmap corazon = corazonOriginal.scaled(
        heartW, heartH,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation);

    int marginLeft   = 10;
    int marginBottom = 10;

    for (int i = 0; i < 3; ++i) {
        auto *item = new QGraphicsPixmapItem(corazon);
        item->setZValue(100);

        int x = marginLeft + i * (heartW + 5);
        int y = scene->sceneRect().height() - heartH - marginBottom;

        item->setPos(x, y);
        scene->addItem(item);
        corazonesHUD.append(item);
    }

    if (jugador) actualizarHUDVidas(jugador->getVidas());
}

void NivelHaciaBunkerPrueba::actualizarHUDVidas(int vidas)
{
    // Asumimos máximo 3 vidas
    for (int i = 0; i < corazonesHUD.size(); ++i) {
        if (corazonesHUD[i]) {
            corazonesHUD[i]->setVisible(i < vidas);
        }
    }
}

void NivelHaciaBunkerPrueba::actualizarJuego()
{
    if (jugador) {
        actualizarHUDVidas(jugador->getVidas());
    }

    // Aquí más adelante puedes meter lógica general del nivel (scroll, etc)
}

