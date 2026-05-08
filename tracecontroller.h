#ifndef TRACECONTROLLER_H
#define TRACECONTROLLER_H

#include <QObject>
#include <QString>
#include <QVariantList> // Список для хранения данных (как массив в Python)
#include <QVariantMap>  // Словарь (ключ-значение) для хранения свойств


struct my_trace
{
    uint32_t timeStart;
    char NameFunc[32];
    uint8_t flagOpen;
};

// Q_ONVOKABLE позволяет QML видеть наши функции
class TraceController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QVariantList traceData READ traceData  NOTIFY traceDataChanged)


public:
    explicit TraceController(QObject *parent = nullptr);

    Q_INVOKABLE bool loadFile(const QString &path);

    // геттер для передачи данных в словарь о трассировке
    QVariantList traceData() const;
signals:
    // буду вызывать после парсинга
    void traceDataChanged();

private:
    //сами данные которые будем передавать
    QVariantList m_traceData;
};

#endif // TRACECONTROLLER_H
