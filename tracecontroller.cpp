#include "tracecontroller.h"
#include <QFile>       // Для чтения файлов
#include <QDir>
#include <QDebug>      // Для вывода в консоль (qDebug())
#include <QVector>     // Динамический массив Qt (как std::vector)

// Конструктор класса
TraceController::TraceController(QObject *parent)
    : QObject(parent)  // Вызываем конструктор родительского класса
{
    // Конструктор пустой, так как нам нечего инициализировать
}


bool TraceController::loadFile(const QString &path)
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

    struct StackItem {
        QString name;        // Имя функции
        uint32_t startTime;  // Время начала
        int depth;           // Глубина вложенности
        uint8_t flagOpen;
    };

    QVector<StackItem> stack;  // Стек открытых функций
    m_traceData.clear();       // Очищаем старые данные
    StackItem item;


    for (int i = 0; i < count; i++)
    {

        // пока сохраняю все имена функций

        item.name = traces[i].NameFunc;
        item.depth = depth;
        item.startTime = traces[i].timeStart;
        item.flagOpen = traces[i].flagOpen;
        stack.append(item);



        // Создаем запись для QML
        QVariantMap record;  // Словарь (ключ -> значение)
        record["name"] = item.name;           // Имя функции
        record["depth"] = item.depth;         // Уровень вложенности
        record["duration"] = item.startTime;               // Пока неизвестно
        record["indent"] = item.depth * 20;   // Отступ в пикселях
        record["color"] = (item.depth == 0) ? "#e94560" : "#ffffff";
        record["flagOpen"] = item.flagOpen;

        m_traceData.append(record);  // Добавляем в общий список

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
        else
        {
            depth--;
        }




    }
    qDebug().noquote() << m_graphLines.join("\n");

    emit traceDataChanged();

    return true;
}


QVariantList TraceController::traceData() const
{
    return m_traceData;
}
