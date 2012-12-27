#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTcpSocket>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_pushButton_clicked();
    void on_Sck_connected();
    void on_Sck_readyRead();
    void displayError(QAbstractSocket::SocketError socketErro);

private:
    Ui::MainWindow *ui;
    QTcpSocket *Sck;
};

#endif // MAINWINDOW_H
