#ifndef FILESELECTOR_H
#define FILESELECTOR_H

#include <QDialog>

namespace Ui {
class fileSelector;
}

class fileSelector : public QDialog
{
    Q_OBJECT

public:
    explicit fileSelector(QWidget *parent = nullptr);
    ~fileSelector();
    QString getDatabaseFileName() const;    // Función para obtener la ruta de la base de datos
    QString getAttendanceFileName() const;  // Función para obtener la ruta del archivo de asistencia


private slots:
    void seleccionarBaseDeDatos();
    void seleccionarArchivoDeAsistencia();
    void on_startButton_clicked();


private:
    Ui::fileSelector *ui;
};

#endif // FILESELECTOR_H
