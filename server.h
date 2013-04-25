#ifndef SERVER_H
#define SERVER_H

#include <QtGui>

class Server : public QDialog
{
    Q_OBJECT
    
public:
    Server(QWidget *parent = 0);
    ~Server();

private:
    QSharedMemory sharedMemory;     // shared memory
    QLabel *label;                  // info label
    QList<qreal> dataList;          // dataList keeps generated data
    QTimer *timer;                  // timer, controls data sending per second
    void detach();                  // detach function
    qint64 dataNum;                 // counter

private slots:
    void changeState(bool);         // event handler when user pressed start button
    void startServer();             // start server, write data into shared memory
    void terminateServer();         // terminate server
};

#endif // SERVER_H
