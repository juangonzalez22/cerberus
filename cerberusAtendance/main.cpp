#include "mainwindow.h"
#include "fileselector.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication::setWindowIcon(QIcon(":/icons/icon.ico"));  // Establece el ícono global para la aplicación
    QApplication a(argc, argv);

    // Crear y mostrar la ventana de selección de archivos
    fileSelector selector;
    if (selector.exec() == QDialog::Accepted) {
        // Obtener los nombres de archivo seleccionados
        QString databaseFileName = selector.getDatabaseFileName();
        QString attendanceFileName = selector.getAttendanceFileName();

        // Crear y mostrar la ventana principal con los archivos seleccionados
        MainWindow w(databaseFileName, attendanceFileName);
        w.show();

        return a.exec();  // Iniciar el bucle de eventos principal
    }

    return 0;  // Si se cierra la ventana de selección sin iniciar, terminar el programa
}
