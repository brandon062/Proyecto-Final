#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>

class NivelBatallaAerea;
class NivelBatallaTanque;
class NivelHaciaBunkerPrueba;

class QLabel;
class QWidget;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnJugar_clicked();

    void abrirNivel1();
    void abrirNivel2();
    void abrirNivel3();

    void onNivel1Terminado(bool ganado);
    void onNivel2Terminado(bool ganado);
    void onNivel3Terminado(bool ganado);

private:
    Ui::MainWindow *ui;

    // Música del menú
    QMediaPlayer *menuMusic = nullptr;
    QAudioOutput *menuAudio = nullptr;

    // Pantalla de carga
    QWidget *pantallaCarga = nullptr;
    QLabel  *labelCarga    = nullptr;

    // Niveles
    NivelBatallaAerea      *nivel1 = nullptr;
    NivelBatallaTanque     *nivel2 = nullptr;
    NivelHaciaBunkerPrueba *nivel3 = nullptr;

    void configurarFondoMenu();
    void configurarMusicaMenu();

    void mostrarPantallaCarga(const QString &texto);
    void ocultarPantallaCarga();
};

#endif // MAINWINDOW_H


