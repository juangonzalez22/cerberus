#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDateTime>
#include <QFileDialog>
#include <QTableWidgetItem>
#include <QMap>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Configurar la tabla cuando se inicie la interfaz
    configurarTabla();

    // Conectar el botón para seleccionar base de datos
    connect(ui->selectDatabaseButton, &QPushButton::clicked, this, &MainWindow::seleccionarBaseDeDatos);

    // Conectar el campo de texto para registrar un estudiante al presionar enter
    connect(ui->idLineEdit, &QLineEdit::returnPressed, this, &MainWindow::registrarEstudiante);

    // Conectar el botón para eliminar todos los registros.
    connect(ui->cleanButton, &QPushButton::clicked, this, &MainWindow::limpiarRegistros);

    // Conectar el botón para exportar registros
    connect(ui->exportButton, &QPushButton::clicked, this, &MainWindow::exportarRegistros);

    //Conectar el botón para limpiar el último registro
    connect(ui->deleteLastButton, &QPushButton::clicked, this, &MainWindow::eliminarUltimoRegistro);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::configurarTabla()
{
    // Establecer el número de columnas (ya lo hemos hecho antes)
    ui->tableRegist->setColumnCount(4); // ID, Nombre, Grupo, Hora de llegada

    // Establecer los encabezados
    QStringList headers;
    headers << "ID" << "Nombre" << "Grupo" << "Hora de Llegada";
    ui->tableRegist->setHorizontalHeaderLabels(headers);

    // Ajustar el ancho de las columnas
    ui->tableRegist->setColumnWidth(0, 100);  // Columna de ID
    ui->tableRegist->setColumnWidth(1, 200);  // Columna de Nombre
    ui->tableRegist->setColumnWidth(2, 100);  // Columna de Grupo
    ui->tableRegist->setColumnWidth(3, 150);  // Columna de Hora de llegada

    // Opcional: establecer el comportamiento para que las columnas se ajusten al contenido si es necesario
    ui->tableRegist->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    ui->tableRegist->setEditTriggers(QAbstractItemView::NoEditTriggers);
}


void MainWindow::cargarBaseDeDatos(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Error", "No se pudo abrir el archivo.");
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(',');

        // Asegúrate de que cada línea tenga los tres campos necesarios
        if (fields.size() == 3) {
            QString id = fields[0];
            QString nombre = fields[1];
            QString grupo = fields[2];

            // Guardamos los datos en el QMap
            estudiantes[id] = QStringList() << nombre << grupo;
        }
    }

    file.close();
}

bool MainWindow::estudianteYaRegistrado(const QString &idIngresado)
{
    for (int i = 0; i < ui->tableRegist->rowCount(); ++i) {
        if (ui->tableRegist->item(i, 0)->text() == idIngresado) {
            return true;  // El estudiante ya está registrado
        }
    }
    return false;  // El estudiante no está registrado
}

void MainWindow::registrarEstudiante()
{
    QString idIngresado = ui->idLineEdit->text();

    // Verificar si el estudiante ya está registrado en la tabla
    if (estudianteYaRegistrado(idIngresado)) {
        // Preguntar al usuario si desea registrar nuevamente
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Registro Duplicado",
                                      "El estudiante ya ha registrado su entrada anteriormente. ¿Desea registrar nuevamente?",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            ui->idLineEdit->clear();  // Limpiar el campo de texto
            return;  // Si el usuario elige No, salir del método
        }
    }

    // Si el estudiante no está registrado o el usuario eligió registrar nuevamente
    buscarEstudiantePorID(idIngresado);
    ui->idLineEdit->clear();  // Limpiar el campo de texto después de registrar
}
void MainWindow::buscarEstudiantePorID(const QString& idIngresado)
{
    if (estudiantes.contains(idIngresado)) {
        QStringList datosEstudiante = estudiantes[idIngresado];
        QString nombre = datosEstudiante[0];
        QString grupo = datosEstudiante[1];

        // Mostrar los datos en la tabla
        int row = ui->tableRegist->rowCount();
        ui->tableRegist->insertRow(row);
        ui->tableRegist->setItem(row, 0, new QTableWidgetItem(idIngresado));
        ui->tableRegist->setItem(row, 1, new QTableWidgetItem(nombre));
        ui->tableRegist->setItem(row, 2, new QTableWidgetItem(grupo));
        ui->tableRegist->setItem(row, 3, new QTableWidgetItem(QDateTime::currentDateTime().toString()));
    } else {
        QMessageBox::warning(this, "Error", "Estudiante no encontrado.");
    }
}

void MainWindow::exportarRegistros()
{
    // Abrir el cuadro de diálogo para seleccionar la ubicación y el nombre del archivo
    QString fileName = QFileDialog::getSaveFileName(this, "Guardar archivo CSV", "", "Archivos CSV (*.csv)");

    // Verificar que el usuario haya seleccionado un nombre de archivo
    if (fileName.isEmpty()) {
        return;  // Si no se seleccionó archivo, simplemente salimos
    }

    // Abrir el archivo seleccionado para escribir
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        out << "ID,Nombre,Grupo,Hora de Llegada\n";
        // Escribir los datos de la tabla en el archivo CSV
        for (int i = 0; i < ui->tableRegist->rowCount(); ++i) {
            out << ui->tableRegist->item(i, 0)->text() << ","
                << ui->tableRegist->item(i, 1)->text() << ","
                << ui->tableRegist->item(i, 2)->text() << ","
                << ui->tableRegist->item(i, 3)->text() << "\n";
        }

        file.close();
        QMessageBox::information(this, "Éxito", "Registros exportados correctamente.");
    } else {
        QMessageBox::warning(this, "Error", "No se pudo guardar el archivo.");
    }
}


// Función para limpiar todos los registros de la tabla con confirmación
void MainWindow::limpiarRegistros()
{
    // Mostrar mensaje de confirmación
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirmar limpieza",
                                                              "¿Está seguro que quiere eliminar todos los registros? Esta acción no se puede revertir. Asegúrese de que ha guardado lo importante.",
                                                              QMessageBox::Yes | QMessageBox::No);

    // Si el usuario presiona "Sí", borrar los registros
    if (reply == QMessageBox::Yes) {
        int rows = ui->tableRegist->rowCount();
        for (int i = rows - 1; i >= 0; --i) {
            ui->tableRegist->removeRow(i);  // Eliminar la fila
        }
    }
}

void MainWindow::seleccionarBaseDeDatos()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Seleccionar archivo CSV", "", "Archivos CSV (*.csv)");
    if (!fileName.isEmpty()) {
        cargarBaseDeDatos(fileName);
        ui->fileLabel->setText(fileName);  // Mostrar el nombre del archivo seleccionado en el label
    }
}

void MainWindow::eliminarUltimoRegistro()
{
    int rowCount = ui->tableRegist->rowCount();
    if (rowCount > 0) {
        ui->tableRegist->removeRow(rowCount - 1);  // Eliminar la última fila
    } else {
        QMessageBox::warning(this, "Advertencia", "No hay registros para eliminar.");
    }
}
