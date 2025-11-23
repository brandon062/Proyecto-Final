#include "NivelBatallaAerea.h"
#include <QTimer>
#include <QGraphicsTextItem>
#include <QFont>
#include "enemigo.h"
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QImage>
#include <QBrush>
#include <QKeyEvent>
#include <QUrl>

NivelBatallaAerea::NivelBatallaAerea(QWidget *parent){
    // create the scene
    scene = new QGraphicsScene();
    scene->setSceneRect(0,0,800,600);
    setBackgroundBrush(QBrush(QImage(":/images/bg.png")));
    setScene(scene);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setFixedSize(800,600);

    finished = false;
    spawnTimer = nullptr;
    countdownTimer = nullptr;
    timerText = nullptr;

    // ====== MÚSICA DE FONDO ======
    bgMusic = new QMediaPlayer(this);
    bgAudio = new QAudioOutput(this);

    bgMusic->setAudioOutput(bgAudio);
    bgAudio->setVolume(0.2);
    bgMusic->setSource(QUrl("qrc:/sonidos/nivel1_background.mp3"));
    bgMusic->play();

    // Inicializar el nivel (jugador, score, vida, timers, etc.)
    resetLevel();

    show();
}

void NivelBatallaAerea::resetLevel()
{
    finished = false;

    // Detener timers antiguos si existen
    if (spawnTimer) {
        spawnTimer->stop();
    }
    if (countdownTimer) {
        countdownTimer->stop();
    }

    // Limpiar todos los items de la escena (jugador, enemigos, textos, explosiones...)
    if (scene) {
        scene->clear();
    }

    // ====== RECREAR JUGADOR ======
    Jugador = new jugador();
    Jugador->setPos(400,500);
    Jugador->setFlag(QGraphicsItem::ItemIsFocusable);
    Jugador->setFocus();
    scene->addItem(Jugador);

    // ====== RECREAR SCORE Y VIDA ======
    score = new Score();
    scene->addItem(score);

    vida = new Vida();
    vida->setPos(vida->x(), vida->y()+25);
    scene->addItem(vida);

    // ====== REINICIAR CONTADOR ======
    remainingTime = 60; // 60 segundos

    timerText = new QGraphicsTextItem();
    timerText->setPlainText("Tiempo: 1:00");
    timerText->setDefaultTextColor(Qt::black);
    timerText->setFont(QFont("times", 16));
    timerText->setPos(650, 0);
    scene->addItem(timerText);

    // ====== RECREAR TIMER DE ENEMIGOS ======
    if (spawnTimer) {
        spawnTimer->deleteLater();
    }
    spawnTimer = new QTimer(this);
    QObject::connect(spawnTimer, SIGNAL(timeout()), Jugador, SLOT(spawn()));
    spawnTimer->start(2000);

    // ====== RECREAR TIMER DE CUENTA REGRESIVA ======
    if (countdownTimer) {
        countdownTimer->deleteLater();
    }
    countdownTimer = new QTimer(this);
    QObject::connect(countdownTimer, &QTimer::timeout, [this](){
        if (finished) {
            countdownTimer->stop();
            return;
        }

        remainingTime--;
        if (remainingTime < 0) return;

        int minutes = remainingTime / 60;
        int seconds = remainingTime % 60;

        timerText->setPlainText(
            QString("Tiempo: %1:%2")
                .arg(minutes)
                .arg(seconds, 2, 10, QChar('0'))
            );

        if (remainingTime == 0) {
            countdownTimer->stop();
            winLevel();
        }
    });
    countdownTimer->start(1000);

    // ====== REINICIAR MÚSICA DE FONDO ======
    if (bgMusic && bgAudio) {
        bgMusic->stop();
        bgMusic->setSource(QUrl("qrc:/sonidos/nivel1_background.mp3"));
        bgAudio->setVolume(0.2);
        bgMusic->play();
    }
}

void NivelBatallaAerea::winLevel(){
    if (finished) return;
    finished = true;

    if (spawnTimer) spawnTimer->stop();
    if (countdownTimer) countdownTimer->stop();

    // parar música de fondo
    if (bgMusic) {
        bgMusic->stop();
    }

    // reproducir música de victoria
    QMediaPlayer *winMusic = new QMediaPlayer(this);
    QAudioOutput *winAudio = new QAudioOutput(this);
    winMusic->setAudioOutput(winAudio);
    winAudio->setVolume(0.6);
    winMusic->setSource(QUrl("qrc:/sonidos/winning.mp3"));
    winMusic->play();

    // Mensaje de victoria
    QGraphicsTextItem *msg = new QGraphicsTextItem();
    msg->setPlainText("Perfecto! haz retenido exitosamente los aviones alemanes,\n"
                      "ya el grupo de infanteria podra avanzar junto a los tanques.");
    msg->setDefaultTextColor(Qt::black);
    msg->setFont(QFont("times", 16));
    msg->setPos(60, 260);
    scene->addItem(msg);
}

void NivelBatallaAerea::gameOver(){
    if (finished) return;
    finished = true;

    if (spawnTimer) spawnTimer->stop();
    if (countdownTimer) countdownTimer->stop();

    // parar música de fondo
    if (bgMusic) {
        bgMusic->stop();
    }

    // eliminar sprite del jugador si sigue existiendo
    if (Jugador) {
        scene->removeItem(Jugador);
        delete Jugador;
        Jugador = nullptr;
    }

    // reproducir música de derrota
    QMediaPlayer *loseMusic = new QMediaPlayer(this);
    QAudioOutput *loseAudio = new QAudioOutput(this);
    loseMusic->setAudioOutput(loseAudio);
    loseAudio->setVolume(0.6);
    loseMusic->setSource(QUrl("qrc:/sonidos/game_over.mp3"));
    loseMusic->play();

    // Mensaje de game over + opción de jugar de nuevo
    QGraphicsTextItem *msg = new QGraphicsTextItem();
    msg->setPlainText("GAME OVER\nPulsa R para jugar de nuevo");
    msg->setDefaultTextColor(Qt::red);
    msg->setFont(QFont("times", 20));
    msg->setPos(230, 250);
    scene->addItem(msg);
}

void NivelBatallaAerea::keyPressEvent(QKeyEvent *event)
{
    // Si el juego ha terminado (victoria o derrota), solo escuchamos R
    if (finished) {
        if (event->key() == Qt::Key_R) {
            resetLevel();   // reinicia el nivel en la MISMA ventana
        }
        return;
    }

    // Si no ha terminado, enviamos las teclas al jugador
    if (Jugador) {
        Jugador->keyPressEvent(event);
    }
}




