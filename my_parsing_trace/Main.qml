import QtQuick

import QtQuick.Controls
import QtQuick.Window
import QtQuick.Layouts
// для добавления файлов
import QtQuick.Dialogs

ApplicationWindow {
    id: window
    width: 900
    height: 700
    visible: true
    title: "Trace Visualizer"
    color: "#1a1a2e"



    property var nodes: []
    property var connections: []
    property int graphWidth: 800
    property int graphHeight: 600
    property string pathName: ""

    Column {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 20

        Rectangle {
            width: parent.width
            height: 60
            color: "#16213e"
            border.color: "#0f3460"
            border.width: 3

            Text {
                anchors.centerIn: parent
                text: "TRACE GRAPH"
                font.pixelSize: 28
                font.bold: true
                color: "#e94560"
                font.family: "Courier New"
            }
        }
        // положение подписи с загрузкой файла
        Rectangle {
            width: parent.width
            height: 90
            color: "#16213e"
            border.color: "#0f3460"
            border.width: 3

            Column {
                 anchors.centerIn: parent
                spacing: 10
                width: parent.width - 40

                Row {
                    spacing: 10
                    anchors.horizontalCenter: parent.horizontalCenter

                    Rectangle {
                        width: 400
                        height: 35
                        color: "#0f0f1a"
                        border.color: "#e94560"
                        border.width: 2

                        TextInput {
                            id: filePathInput
                            anchors.fill: parent
                            anchors.margins: 5
                            color: "#00ff00"
                            font.family: "Courier New"
                            font.pixelSize: 14
                            text: window.pathName
                            clip: true
                            onTextChanged: window.pathName = text
                        }
                    }
                    // работа с файлами
                    FileDialog {
                        id: fileDialog
                        title: "Выберите файл"
                        currentFolder: StandardPaths.writableLocation(StandardPaths.DocumentsLocation)
                        onAccepted: {
                            // Путь к файлу (в формате URL: file:///...)
                            let fullPath = selectedFile.toString();

                            // Как получить только имя файла
                            let fileName = fullPath.substring(fullPath.lastIndexOf('/') + 1);

                            console.log("Полный путь:", fullPath);
                            console.log("Имя файла:", fileName);

                            // Если нужно передать путь в C++ (убираем префикс file://)
                            let localPath = urlToPath(selectedFile)
                            console.log("Локальный путь для C++:", localPath)
                            window.pathName = localPath;

                            // Вызываем C++ функцию
                            if (traceParser.loadFile(localPath)) {
                                statusText.text = "[ LOADED ]";
                                statusText.color = "#00ff00";
                                // Здесь же можно вызвать drawGraph() или другую логику
                            } else {
                                statusText.text = "[ ERROR ]";
                                statusText.color = "#ff0000";
                            }

                        }
                        // Вспомогательная функция для очистки пути от префикса file://
                        function urlToPath(url) {
                            let path = url.toString();
                            if (Qt.platform.os === "windows") {
                                return path.replace(/^(file:\/{3})/, "");
                            } else {
                                return path.replace(/^(file:\/{2})/, "");
                            }
                        }
                    }

                    Button {
                        id: loadButton
                        text: "[ LOAD ]"
                        font.family: "Courier New"
                        font.pixelSize: 14

                        background: Rectangle {
                            color: loadButton.hovered ? "#e94560" : "#0f3460"
                            border.color: "#e94560"
                            border.width: 2
                        }

                        contentItem: Text {
                            text: loadButton.text
                            font: loadButton.font
                            color: "#ffffff"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked: {
                            fileDialog.open()
                            // var path = filePathInput.text.trim()
                            // if (path === "") return

                            //     if (traceParser.loadFile(path)) {
                            //         statusText.text = "[ LOADED ]"
                            //         statusText.color = "#00ff00"
                            //         drawGraph()
                            //     } else {
                            //         statusText.text = "[ ERROR ]"
                            //         statusText.color = "#ff0000"
                            //     }
                        }
                    }
                }

                Text {
                    id: statusText
                    text: "[ READY ]"
                    font.pixelSize: 12
                    color: "#00ff00"
                    font.family: "Courier New"
                    anchors.horizontalCenter: parent.horizontalCenter
                }
            }
        }
        // Отладочная информация
        Text {
            text: "Количество записей: " + traceParser.traceData.length
            color: "#ffffff"
            font.family: "Courier New"
            anchors.horizontalCenter: parent.horizontalCenter
        }


            // Список функций
                    ListView {
                        id: listView
                        width: parent.width
                        height: parent.height
                        model: traceParser.traceData
                        clip: true

                        onCountChanged: {
                            console.log("QML: ListView обновлен, количество элементов:", count)
                        }


                        delegate: Rectangle {
                            width: ListView.view.width-10
                            height: 30
                            color: index % 2 === 0 ? "#1a1a2e" : "#16213e"

                            Component.onCompleted: {
                                console.log("QML: Создан элемент", index,
                                           "имя:", modelData.name,
                                           "длительность:", modelData.duration,
                                            "глубина", modelData.depth)
                            }

                            // Имя функции
                            Text {
                                x: modelData.indent + 20
                                anchors.verticalCenter: parent.verticalCenter
                                text: {
                                    var prefix = ""
                                    for (var i = 0; i < modelData.depth; i++) {
                                        prefix += "  "
                                    }
                                    if (modelData.depth > 0) {
                                        return prefix + "+- " + modelData.name + " [" + modelData.duration+ "]";
                                    } else {
                                        return "--- " + modelData.name + " [" + modelData.duration+ "]";
                                    }
                                }
                                color: modelData.color
                                font.family: "Courier New"
                                font.pixelSize: 14
                            }

                        }
                    }



    }
}
