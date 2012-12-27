#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QTextStream>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>

#include <QtSql>
#include <QMessageBox>
#include <QDateTime>
#include <QDate>
#include <QTime>

#include <QtXml>

#include <QMessageBox>
#include <QTextCodec>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    Sck = new QTcpSocket(this);
    connect(Sck, SIGNAL(connected()),this, SLOT(on_Sck_connected()));
    connect(Sck, SIGNAL(readyRead()),this, SLOT(on_Sck_readyRead()));
    connect(Sck, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError(QAbstractSocket::SocketError)));

    Sck->connectToHost("jabber.ru",5222);
    Sck->write("<?xml version=\'1.0\'?><stream:stream to=\'jabber.ru\' xmlns=\'jabber:client\' xmlns:stream=\'http://etherx.jabber.org/streams\' version=\'1.0\'>");

    QString *msg_auth = new QString("AHZpY3RvcnVfYmFzdEBqYWJiZXIucnUAdnByaW5jZXNz");
    msg_auth->prepend("<auth xmlns=\"urn:ietf:params:xml:ns:xmpp-sasl\" mechanism=\"PLAIN\">");
    msg_auth->append("</auth>");

    Sck->write(msg_auth->toUtf8());

    Sck->waitForReadyRead(500);

    Sck->write("<?xml version=\'1.0\'?><stream:stream to=\'jabber.ru\' xmlns=\'jabber:client\' xmlns:stream=\'http://etherx.jabber.org/streams\' version=\'1.0\'>");

    Sck->waitForReadyRead(500);

    Sck->write("<iq type=\"set\" id=\"bind_1\">"
    "<bind xmlns=\"urn:ietf:params:xml:ns:xmpp-bind\">"
    "<resource>Drandulet</resource>"
    "</bind>"
    "</iq>");

    Sck->waitForReadyRead(500);


    Sck->write("<iq type=\'set\' id=\'bind_2\'><session xmlns=\'urn:ietf:params:xml:ns:xmpp-session\'/></iq>");

    Sck->waitForReadyRead(500);

    Sck->write("<presence><show></show></presence>");

    Sck->write("<presence>"
               "<priority>5</priority>"
               "</presence>");

    Sck->waitForReadyRead(500);

    Sck->write("<iq from=\'test_0_1@jabber.ru/Drandulet\' type=\'get\' id=\'roster_1\'> <query xmlns=\'jabber:iq:roster\'/> </iq>");

    Sck->waitForReadyRead(1000);

}

void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("Fortune Client"),
                                 tr("The following error occurred: %1.")
                                 .arg(Sck->errorString()));
    }

}

void MainWindow::on_Sck_readyRead()
{

    //QMessageBox::information(0,"Information","Server message accepted");

    QFile fileOut("1.txt");
    fileOut.open(QIODevice::Text | QIODevice::Append);

    QTextStream out(&fileOut);

    QString str = Sck->readAll();

    out<<str<<endl;
    out<<"\r\n"<<endl;
    out<<"\r\n"<<endl;

   // ui->textEdit->append(str+"\r\n\r\n");

    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Windows-1251"));

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("MessDB.db");

    if (!db.open())
        QMessageBox::warning(0,"Warning",db.lastError().text());

    int count = 0;

    QSqlQuery query1("select id from history",db);

    if(query1.isActive())
    {
        while(query1.next())
            count = query1.value(0).toString().toInt();
    }
    else
    {
        query1.exec("DROP TABLE History;");

        query1.exec("CREATE TABLE History (ID integer PRIMARY KEY NOT NULL UNIQUE, message varchar(8000) NOT NULL, IdSender varchar(8000) NOT NULL, IdReceiver varchar(8000) NOT NULL, SendTime datetime NOT NULL);");

        query1.exec("insert into history (id, message, idsender, idreceiver, sendTime)values(1,\"Сообщение №1\",\"306\",\"678\",\"2012-12-01 12:34:56\");");

        query1.exec("insert into history (id, message, idsender, idreceiver, sendTime)values(2,\"Сообщение №2\",\"676\",\"678\",\"2012-12-01 12:34:56\");");

        query1.exec("insert into history (id, message, idsender, idreceiver, sendTime)values(3,\"Сообщение №3\",\"346\",\"678\",\"2012-12-01 12:34:56\");");

        query1.exec("select id from history");

        while(query1.next())
            count = query1.value(0).toString().toInt();


    }

    if(str.contains("from"))
    {
        QMessageBox::information(0,"Information","Get message");

        count++;

        int a1 = str.indexOf("<message from=") + 15;
        int a2 = str.indexOf("@jabber.ru");
\
        QString from ="";
        for(int i=a1;i<a2;i++)
            from+=str[i];

        from="<b>"+ from+ "</b>";

        ui->textEdit->append(from+"\r\n\r\n");

        query1.exec("insert into history (id, message, idsender, idreceiver, sendTime)values("+ QString::number(count) +",\""+ from +"\",\"786\",\"567\",\"2012-12-01 12:34:56\");");


    }

    if(str.contains("message"))
    {
        QMessageBox::information(0,"Information","Get message");

        count++;

        int a1 = str.indexOf("<body>") + 6;
        int a2 = str.indexOf("</body>");
\
        QString message ="";
        for(int i=a1;i<a2;i++)
            message+=str[i];
        ui->textEdit->append(message+"\r\n\r\n");

        query1.exec("insert into history (id, message, idsender, idreceiver, sendTime)values("+ QString::number(count) +",\""+ message +"\",\"786\",\"567\",\"2012-12-01 12:34:56\");");


    }

    if(str.contains("stamp"))
    {
        QMessageBox::information(0,"Information","Get message");

        count++;

        int a1 = str.indexOf("stamp=") + 6;
        int a2 = str.indexOf("'>Offline Storage");
\
        QString stamp ="";
        for(int i=a1;i<a2;i++)
            stamp+=str[i];
        ui->textEdit->append(stamp+"\r\n\r\n");

        query1.exec("insert into history (id, message, idsender, idreceiver, sendTime)values("+ QString::number(count) +",\""+ stamp +"\",\"786\",\"567\",\"2012-12-01 12:34:56\");");


    }
    db.close();
}

void MainWindow::on_Sck_connected()
{
    //QMessageBox::information(0,"Information","Connected");
}

