#include "server.h"
#include <ctime>

Server::Server(QWidget *parent)
    : QDialog(parent), sharedMemory("WatchServer")

    /* "WatchServer" is the key of shared memory
     * the client (Watcher) will use this key
     * to allocate this specific memory area
     *     DON'T CHANGE IT UNLESS YOU KNOW
     *          WHAT YOU ARE DOING
     */

{
    //UI Settings
    QPushButton *start = new QPushButton("Start");
    start->setCheckable(true);
    QVBoxLayout *layout = new QVBoxLayout;
    label = new QLabel("Press Start Button to start (or terminate) server.");

    layout->addWidget(start);
    layout->addWidget(label);
    this->setLayout(layout);
    this->setFixedSize(sizeHint());

    //Initialize
    timer = new QTimer;
    dataNum = 0;

    //Event handler setting
    connect (start,SIGNAL(toggled(bool)),this,SLOT(changeState(bool)));
}

void Server::changeState(bool state)
{
    //change server running state when user toggled button
    if (state)
    {
        //start running, write data into shared memory per second via timer
        connect (timer,SIGNAL(timeout()),this,SLOT(startServer()));
        timer->start(1000);
    }
    else
    {
        //stop running
        disconnect (timer,SIGNAL(timeout()),this,SLOT(startServer()));
        timer->stop();
        terminateServer();
        label->setText("Press Start Button to start (or terminate) server.");
        dataNum = 0;
    }
}

void Server::startServer()
{
    //detach the old one
    if (sharedMemory.isAttached())
        detach();

    //generate random numbers into dataList
    qsrand(time(NULL));
    QList<qreal> dataList;
    dataList.clear();
    double temp;
    for (int i = 0; i < 100; i++)
    {
        temp = qrand() % 80 - 40;
        dataList.append(temp);
    }

    //create SharedMemory
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);
    QDataStream out(&buffer);
    out << dataList;
    int size = buffer.size();

    if (!sharedMemory.create(size)) {
        label->setText("Error: " + sharedMemory.errorString());
        return;
    }
    sharedMemory.lock();
    char *to = (char*)sharedMemory.data();
    const char *from = buffer.data().data();
    memcpy(to, from, qMin(sharedMemory.size(), size));
    sharedMemory.unlock();

    //counter, display info
    dataNum++;
    label->setText(QString("Sending No.%1 group of data.").arg(QString::number(dataNum)));
}

void Server::terminateServer()
{
    //detach shared memory
    if (sharedMemory.isAttached())
        detach();
}

void Server::detach()
{
    if (!sharedMemory.detach())
        label->setText("Error: " + sharedMemory.errorString());
}

Server::~Server()
{
    
}
