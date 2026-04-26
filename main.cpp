#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QFile>
#include <QDebug>
#include <QDir>
#include <QQmlContext>

struct my_trace
{
    uint32_t timeStart;
    char NameFunc[32];
    uint8_t flagOpen;
};

bool parcTrace(const QString path)
{
    qDebug() << "Current path:" << QDir::currentPath();
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Cannot open file:" << file.errorString();
        return false;
    }
    // далее читаем количество структур
    int32_t count = 0;
    if (file.read((char*)&count, sizeof(int32_t)) != sizeof(int32_t))
    {
        qDebug() << "Error reading count";
        file.close();
        return false;
    }

    qDebug() << "Total traces in file:" << count;
    qDebug() << "Expected bytes:" << sizeof(my_trace) * count;
    qDebug() << "File size:" << file.size();

    // Читаем все структуры
    QByteArray data = file.read(sizeof(my_trace) * count);
    qDebug() << "Read bytes:" << data.size();

    if (data.size() != (int)(sizeof(my_trace) * count))
    {
        qDebug() << "Error reading traces, expected" << sizeof(my_trace) * count
                 << "bytes, got" << data.size();
        file.close();
        return false;
    }

    file.close();

    // Приводим к массиву структур
    my_trace *traces = (my_trace*)data.data();

    // Для отладки выведем все структуры
    for (int i=0;i<count;i++)
    {
        qDebug() << "First trace - timeStart:" << traces[i].timeStart
                 << "NameFunc:" << traces[i].NameFunc
                 << "flagOpen:" << traces[i].flagOpen;
    }

    QStringList m_graphLines;
    m_graphLines.clear();
    // рисуем в консоль
    int depth = 0;

    for (int i = 0; i < count; i++)
    {
        if (traces[i].flagOpen == 0)  // открытие функции
        {
            QString line;

            // Строим отступы
            for (int j = 0; j < depth; j++)
            {
                line += QString(" ").repeated(depth+3)+"│ \n";
            }

            // Добавляем ветку
            if (depth > 0) {
                line += QString(" ").repeated(depth+3)+"├─";
            } else {
                line += "───";
            }

            // Информация о функции
            line += "■ ";
            line += traces[i].NameFunc;
            line += " [";
            line += QString::number(traces[i].timeStart);
            line += " мс]";

            m_graphLines.append(line);

            depth++;
        }
        else  // закрытие функции
        {
            depth--;
        }
    }
    qDebug().noquote() << m_graphLines.join("\n");

    return true;
}

// Q_ONVOKABLE позволяет QML видеть наши функции
class TraceController : public QObject {
    Q_OBJECT
public:
    explicit TraceController(QObject *parent = nullptr) : QObject(parent) {}

    Q_INVOKABLE bool loadFile(const QString &path) {
        qDebug() << "Loading file:" << path;
        return parcTrace(path);
    }
};

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