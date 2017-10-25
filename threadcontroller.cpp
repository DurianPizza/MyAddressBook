#include "threadcontroller.h"
#include "stdafx.h"

// usage: the thread controller

// function: called when Controller's signal onTryToConnectServer
//           has been emitted
void ServerConnectionManager::TryToConnectToServer()
{
   // connect to server
   global::connectToDatabase = global::OpenDatabase();
   emit connectFinish();
}

Controller::Controller()
{
    ServerConnectionManager* manager = new ServerConnectionManager();
    manager->moveToThread(&workerThread);

    connect(&workerThread, &QThread::finished, manager, &QObject::deleteLater);

    connect(this, &Controller::onTryToConnectServer, manager, &ServerConnectionManager::TryToConnectToServer);
    connect(manager, &ServerConnectionManager::connectFinish, this, &Controller::onConnectFinish);
    workerThread.start();
}

Controller::~Controller()
{
    workerThread.quit();
    workerThread.wait();
}

// function: called when the thread emit a signal connectFinish
//           to give feedback to main thread
void Controller::onConnectFinish()
{
    if(global::loginHasPressed){
        global::loginHasPressed = false;
        emit onLogInFeedback();
    }
    else if(global::registerHasPressed){
        global::registerHasPressed = false;
        emit onRegisterFeedback();
    }
}
