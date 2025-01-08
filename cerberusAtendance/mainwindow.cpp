#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDateTime>
#include <QFileDialog>
#include <QTableWidgetItem>
#include <QMap>
#include <QApplication>
#include <QTimer>

MainWindow::MainWindow(const QString &databaseFile, const QString &attendanceFile, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    databaseFile(databaseFile),  // Guardar el archivo de base de datos
    attendanceFile(attendanceFile)  // Guardar el archivo de asistencia
{
    ui->setupUi(this);
    setWindowTitle("Velvet Attendance");


    // Verificamos si el archivo de base de datos fue seleccionado
    cargarBaseDeDatos(databaseFile);  // Cargar la base de datos de estudiantes

    // Configurar la tabla cuando se inicie la interfaz
    configurarTabla();

    // Conectar el campo de texto para registrar un estudiante al presionar enter
    connect(ui->idLineEdit, &QLineEdit::returnPressed, this, &MainWindow::registrarEstudiante);

    // Conectar el botón para eliminar todos los registros
    connect(ui->cleanButton, &QPushButton::clicked, this, &MainWindow::limpiarRegistros);

    // Conectar el botón para limpiar el último registro
    connect(ui->deleteLastButton, &QPushButton::clicked, this, &MainWindow::eliminarUltimoRegistro);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::actualizarReloj);
    timer->start(1000);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::configurarTabla()
{
    ui->tableRegist->setColumnCount(4); // ID, Nombre, Grupo, Hora de llegada
    QStringList headers;
    headers << "ID" << "Nombre" << "Grupo" << "Hora de Llegada";
    ui->tableRegist->setHorizontalHeaderLabels(headers);

    ui->tableRegist->setColumnWidth(0, 100);
    ui->tableRegist->setColumnWidth(1, 200);
    ui->tableRegist->setColumnWidth(2, 100);
    ui->tableRegist->setColumnWidth(3, 150);
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

        // Verificar que tenga exactamente tres campos
        if (fields.size() == 3) {
            QString id = fields[0].trimmed();
            QString nombre = fields[1].trimmed();
            QString grupo = fields[2].trimmed();

            if (!id.isEmpty() && !nombre.isEmpty() && !grupo.isEmpty()) {
                estudiantes[id] = QStringList() << nombre << grupo;
            }
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
        reply = QMessageBox::warning(this, "Registro Duplicado",
                                     "El estudiante ya ha registrado su entrada anteriormente. ¿Desea registrar nuevamente?",
                                     QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            ui->idLineEdit->clear();  // Limpiar el campo de texto
            return;  // Si el usuario elige No, salir del método
        }
    }

    // Verificar si el estudiante existe en la base de datos
    if (!estudiantes.contains(idIngresado)) {
        QMessageBox::warning(this, "Error", "Estudiante no encontrado.");
        ui->idLabel->setText("ID: ");
        ui->nameLabel->setText("Nombre: ");
        ui->groupLabel->setText("Grupo: ");
        ui->idLineEdit->clear();  // Limpiar el campo de texto
        return;  // Detener la ejecución si el ID no se encuentra
    }

    // Si el estudiante existe, obtener sus datos
    QStringList datosEstudiante = estudiantes[idIngresado];
    QString nombre = datosEstudiante[0];
    QString grupo = datosEstudiante[1];

    // Actualizar las etiquetas con los datos del estudiante
    ui->idLabel->setText("ID: " + idIngresado);
    ui->nameLabel->setText("Nombre: " + nombre);
    ui->groupLabel->setText("Grupo: " + grupo);

    // Registrar el estudiante en la tabla
    buscarEstudiantePorID(idIngresado);
    ui->idLineEdit->clear();  // Limpiar el campo de texto después de registrar

    // Registrar en el archivo CSV de asistencia en tiempo real
    QFile file(attendanceFile);
    if (file.open(QIODevice::Append)) {
        QTextStream out(&file);
        QString currentTime = QDateTime::currentDateTime().toString();

        // Escribir los datos del registro (ID, Nombre, Grupo, Hora de Llegada)
        out << idIngresado << ","
            << nombre << ","
            << grupo << ","
            << currentTime << "\n";

        file.close();
    } else {
        QMessageBox::warning(this, "Error", "No se pudo escribir en el archivo de asistencia.");
    }
}




void MainWindow::buscarEstudiantePorID(const QString &idIngresado)
{
    if (estudiantes.contains(idIngresado)) {
        QStringList datosEstudiante = estudiantes[idIngresado];
        qDebug() << "ID encontrado:" << idIngresado << ", Tamaño de la lista:" << datosEstudiante.size();

        if (datosEstudiante.size() >= 2) {
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
            QMessageBox::warning(this, "Error", "Los datos del estudiante están incompletos.");
        }
    } else {
        qDebug() << "ID no encontrado:" << idIngresado;
        QMessageBox::warning(this, "Error", "Estudiante no encontrado.");
        return;  // Detener la ejecución si el ID no se encuentra
    }
}





void MainWindow::actualizarReloj()
{
    // Actualizar el reloj en el label
    QString timeString = QTime::currentTime().toString("hh:mm:ss");
    ui->clockLabel->setText(timeString);
}

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
            ui->tableRegist->removeRow(i);  // Eliminar la fila de la tabla
        }

        ui->idLabel->setText("ID: ");
        ui->nameLabel->setText("Nombre: ");
        ui->groupLabel->setText("Grupo: ");

        // Ahora, actualizamos el archivo CSV para eliminar todos los registros
        QFile file(attendanceFile);
        if (file.open(QIODevice::WriteOnly)) {
            QTextStream out(&file);
            // Escribir solo el encabezado
            out << "ID,Nombre,Grupo,Hora de Llegada\n";
            file.close();
        }
    }
}


void MainWindow::eliminarUltimoRegistro()
{
    int rowCount = ui->tableRegist->rowCount();
    if (rowCount > 0) {
        // Eliminar la última fila de la tabla
        ui->tableRegist->removeRow(rowCount - 1);
        ui->idLabel->setText("ID: ");
        ui->nameLabel->setText("Nombre: ");
        ui->groupLabel->setText("Grupo: ");


        // Ahora, actualizamos el archivo CSV para eliminar el último registro
        QFile file(attendanceFile);
        if (file.open(QIODevice::ReadWrite)) {
            QTextStream in(&file);
            QStringList lines;

            // Leer todas las líneas del archivo
            while (!in.atEnd()) {
                lines.append(in.readLine());
            }

            // Eliminar la última línea (sin el encabezado)
            if (lines.size() > 1) {  // Si hay más de solo el encabezado
                lines.removeLast();  // Eliminar la última línea
            }

            // Volver a escribir el archivo sin el último registro
            file.resize(0);  // Limpiar el contenido del archivo
            QTextStream out(&file);
            for (const QString& line : lines) {
                out << line << "\n";  // Escribir las líneas restantes en el archivo
            }

            file.close();
        }
    } else {
        QMessageBox::warning(this, "Advertencia", "No hay registros para eliminar.");
    }
}

