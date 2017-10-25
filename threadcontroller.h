// created by Brian at 2017.6.15
// * 管理和服务器的连接

#ifndef THREADCONTROLLER_H
#define THREADCONTROLLER_H

#include <QObject>
#include <QThread>

/// 管理和服务器连接的类
class ServerConnectionManager : public QObject
{
      Q_OBJECT

  public slots:
    /// 尝试连接服务器，当和服务器断开连接时或者发起连接时调用
      void TryToConnectToServer();

  signals:
      /// 槽TryToConnectToServer执行完成后发出这个信号
      void connectFinish();
};

class Controller : public QObject
{
    Q_OBJECT
    QThread workerThread;
public:
    Controller();
    ~Controller();

signals:
   /// 和ServerConnectionManager的槽TryToConnectToServer绑定在一起
   /// 要连接数据库就发送这个信号
    void onTryToConnectServer();

private slots:
    void onConnectFinish();

    /// 发给主线程的信号，相当于一个中介
signals:
    void onLogInFeedback();
    void onRegisterFeedback();
};

#endif // THREADCONTROLLER_H
