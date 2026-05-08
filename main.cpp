#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QQmlContext>
#include "tracecontroller.h"



int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // создал класс для связи
    TraceController controller;

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("traceParser", &controller);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("my_parsing_trace", "Main");


    qDebug() << "sizeof(my_trace) =" << sizeof(my_trace);
    printf("test os \n");
    //parcTrace("trace_data.bin");


    return QCoreApplication::exec();
}
#include "main.moc"   // если класс объявлен прямо в main.cpp