#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QFile>
#include <QTextStream>
#include <QMap>
#include <QStringList>  // Para usar QStringList

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void registrarEstudiante();
    void exportarRegistros();
    void limpiarRegistros();
    void seleccionarBaseDeDatos();
    void eliminarUltimoRegistro();

private:
    Ui::MainWindow *ui;

    void configurarTabla();
    void cargarBaseDeDatos(const QString& fileName);
    void buscarEstudiantePorID(const QString& idIngresado);
    bool estudianteYaRegistrado(const QString &idIngresado);

    // Estructura para almacenar los estudiantes
    QMap<QString, QStringList> estudiantes;  // ID -> [Nombre, Grupo]
};

#endif // MAINWINDOW_H
