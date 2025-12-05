#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QTimer>

#include "NivelBatallaAerea.h"
#include "NivelBatallaTanque.h"
#include "NivelHaciaBunkerPrueba.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setFixedSize(1045, 1000);

    configurarFondoMenu();
    configurarMusicaMenu();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ------------------------
// Fondo y música del menú
// ------------------------
void MainWindow::configurarFondoMenu()
{
    this->setStyleSheet(
        "QMainWindow {"
        "  background-image: url(:/images/menu.png);"
        "  background-repeat: no-repeat;"
        "  background-position: center;"
        "}"
        );
}

void MainWindow::configurarMusicaMenu()
{
    menuMusic = new QMediaPlayer(this);
    menuAudio = new QAudioOutput(this);

    menuMusic->setAudioOutput(menuAudio);
    menuAudio->setVolume(0.7);

    menuMusic->setSource(QUrl("qrc:/sonidos/menu.mp3"));
    menuMusic->setLoops(QMediaPlayer::Infinite);
    menuMusic->play();
}

// ------------------------
// Pantalla de carga genérica
// ------------------------
void MainWindow::mostrarPantallaCarga(const QString &texto)
{
    if (!pantallaCarga) {
        pantallaCarga = new QWidget(nullptr); // ventana independiente
        pantallaCarga->setWindowFlags(Qt::FramelessWindowHint);
        pantallaCarga->setAttribute(Qt::WA_DeleteOnClose, false);
        pantallaCarga->setStyleSheet("background-color: black;");

        QVBoxLayout *layout = new QVBoxLayout(pantallaCarga);
        layout->setAlignment(Qt::AlignCenter);

        labelCarga = new QLabel(pantallaCarga);
        labelCarga->setStyleSheet("color: white;");
        QFont font("Arial", 24, QFont::Bold);
        labelCarga->setFont(font);

        layout->addWidget(labelCarga);

        pantallaCarga->setLayout(layout);
        pantallaCarga->setFixedSize(800, 600);
    }

    labelCarga->setText(texto);
    pantallaCarga->show();
    pantallaCarga->raise();
    pantallaCarga->activateWindow();
}

void MainWindow::ocultarPantallaCarga()
{
    if (pantallaCarga)
        pantallaCarga->hide();
}

// ------------------------
// Botón Jugar → Cargar Nivel 1
// ------------------------
void MainWindow::on_btnJugar_clicked()
{
    // Parar música de menú
    if (menuMusic)
        menuMusic->stop();

    // Ocultar menú (no se destruye, se usa como "manager")
    this->hide();

    // Mostrar pantalla de carga 1
    mostrarPantallaCarga("Cargando nivel 1...");

    // A los 3 segundos abrimos el Nivel 1
    QTimer::singleShot(3000, this, &MainWindow::abrirNivel1);
}

// ------------------------
// Apertura de cada nivel
// ------------------------
void MainWindow::abrirNivel1()
{
    ocultarPantallaCarga();

    if (!nivel1) {
        nivel1 = new NivelBatallaAerea;
        // Conectar señal de victoria/derrota
        connect(nivel1, &NivelBatallaAerea::nivelCompletado,
                this, &MainWindow::onNivel1Terminado);
    }
    nivel1->show();
}

void MainWindow::abrirNivel2()
{
    ocultarPantallaCarga();

    if (!nivel2) {
        nivel2 = new NivelBatallaTanque;
        connect(nivel2, &NivelBatallaTanque::nivelCompletado,
                this, &MainWindow::onNivel2Terminado);
    }
    nivel2->show();
}

void MainWindow::abrirNivel3()
{
    ocultarPantallaCarga();

    if (!nivel3) {
        nivel3 = new NivelHaciaBunkerPrueba;
        connect(nivel3, &NivelHaciaBunkerPrueba::nivelCompletado,
                this, &MainWindow::onNivel3Terminado);
    }
    nivel3->show();
}

// ------------------------
// Respuesta cuando termina cada nivel
// ------------------------
void MainWindow::onNivel1Terminado(bool ganado)
{
    if (!nivel1)
        return;

    // Con esto es suficiente para que el jugador ya no vea el nivel 1
    nivel1->hide();

    if (!ganado) {
        this->show();
        if (menuMusic)
            menuMusic->play();
        return;
    }

    //  Si GANÓ: mostrar pantalla negra de "Cargando nivel 2"
    mostrarPantallaCarga("Cargando nivel 2...");

    // Esperar 5 s de pantalla negra y luego abrir el nivel 2
    QTimer::singleShot(5000, this, &MainWindow::abrirNivel2);
}


void MainWindow::onNivel2Terminado(bool ganado)
{
    if (!nivel2)
        return;

    nivel2->hide();

    if (!ganado) {
        this->show();
        if (menuMusic)
            menuMusic->play();
        return;
    }

    mostrarPantallaCarga("Cargando nivel 3...");
    QTimer::singleShot(5000, this, &MainWindow::abrirNivel3);
}


void MainWindow::onNivel3Terminado(bool ganado)
{
    if (!nivel3)
        return;

    nivel3->hide();

    this->show();
    if (menuMusic)
        menuMusic->play();
}



