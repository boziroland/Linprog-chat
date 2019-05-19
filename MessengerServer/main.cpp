#include <QCoreApplication>
#include "server.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Server server;
/*
    while(server.isListening()){
        std::cout << "dasfdas" << std::endl;
    }
*/
    return a.exec();
}
