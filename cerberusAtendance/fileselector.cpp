#include "fileselector.h"
#include "ui_fileselector.h"
#include "mainwindow.h"  // Incluir la definición de MainWindow
#include <QMessageBox>
#include <QFileDialog>

fileSelector::fileSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::fileSelector)
{
    ui->setupUi(this);
        setWindowTitle("Velvet Attendance Setup");

    ui->dbLineEdit->setReadOnly(true);
    ui->attendanceLineEdit->setReadOnly(true);

    // Conectar los botones con las funciones correspondientes
    connect(ui->dbButton, &QPushButton::clicked, this, &fileSelector::seleccionarBaseDeDatos);
    connect(ui->attendanceButton, &QPushButton::clicked, this, &fileSelector::seleccionarArchivoDeAsistencia);
}

void fileSelector::seleccionarBaseDeDatos()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Seleccionar Base de Datos", "", "Archivos CSV (*.csv)");
    if (!fileName.isEmpty()) {
        ui->dbLineEdit->setText(fileName);  // Actualiza el campo de texto con la ruta seleccionada
    }
}

void fileSelector::seleccionarArchivoDeAsistencia()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Crear Archivo de Asistencia", "", "Archivos CSV (*.csv)");
    if (!fileName.isEmpty()) {
        ui->attendanceLineEdit->setText(fileName);  // Actualiza el campo de texto con la ruta seleccionada
    }
}

QString fileSelector::getDatabaseFileName() const
{
    return ui->dbLineEdit->text();  // Retorna el contenido del campo de la base de datos
}

QString fileSelector::getAttendanceFileName() const
{
    return ui->attendanceLineEdit->text();  // Retorna el contenido del campo del archivo de asistencia
}

void fileSelector::on_startButton_clicked()
{
    // Paso 1: Validar que ambos campos no estén vacíos
    if (ui->dbLineEdit->text().isEmpty() || ui->attendanceLineEdit->text().isEmpty()) {
        QMessageBox::warning(this, "Error", "Debe seleccionar ambos archivos antes de continuar.");
        return;
    }

    // Paso 2: Retornar QDialog::Accepted para indicar que se inició correctamente
    accept();  // Cierra el diálogo y retorna QDialog::Accepted
}

fileSelector::~fileSelector()
{
    delete ui;
}
