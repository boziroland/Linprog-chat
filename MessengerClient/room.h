#ifndef ROOM_H
#define ROOM_H

#include <QObject>

class Room : public QObject
{
    Q_OBJECT
public:
    explicit Room(QString name, QObject *parent = nullptr);
    bool getIfSubscribedTo() const;
    void subscribeTo();
    void unsubscribeFrom();

    QString getRoomName() const;
    void addToLog(QString str);

    std::vector<QString> getLog() const;

private:
    QString roomName;
    std::vector<QString> log;
    bool isSubscribedTo;

signals:

public slots:
};

#endif // ROOM_H
