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
    // Constructor con parámetros
    MainWindow(const QString &databaseFile, const QString &attendanceFile, QWidget *parent = nullptr);

    ~MainWindow();

private slots:
    void onFilesSelected(const QString &dbPath, const QString &attendancePath);
    void registrarEstudiante();
    void limpiarRegistros();
    void seleccionarBaseDeDatos();
    void eliminarUltimoRegistro();
    void actualizarReloj();

private:
    Ui::MainWindow *ui;

    void configurarTabla();
    void cargarBaseDeDatos(const QString& fileName);
    void buscarEstudiantePorID(const QString& idIngresado);
    bool estudianteYaRegistrado(const QString &idIngresado);

    QString databaseFile;
    QString attendanceFile;

    // Estructura para almacenar los estudiantes
    QMap<QString, QStringList> estudiantes;  // ID -> [Nombre, Grupo]
};

#endif // MAINWINDOW_H
