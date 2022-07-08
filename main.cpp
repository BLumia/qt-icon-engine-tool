#include <QIconEnginePlugin>
#include <private/qiconloader_p.h>

#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickImageProvider>
#include <QQmlContext>
#include <QLabel>

class IconLoaderImageProvider : public QQuickImageProvider {
public:
    IconLoaderImageProvider() :
        m_loader(QIconEngineFactoryInterface_iid, QLatin1String("/iconengines"), Qt::CaseSensitive),
        QQuickImageProvider(QQuickImageProvider::Pixmap)
    {}
    ~IconLoaderImageProvider() {}

    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) override
    {
        QPixmap invalid(requestedSize);
        invalid.fill(Qt::transparent);

        QStringList idAndName = id.split('/');
        if (idAndName.length() != 2) return invalid;
        int index = idAndName.at(0).toInt();
        QString name = idAndName.at(1).trimmed();
        if (!m_loader.keyMap().contains(index) || name.isEmpty()) return invalid;
        qDebug() << id << index << name << requestedSize;

        QIconEnginePlugin * iePlugin = qobject_cast<QIconEnginePlugin *>(m_loader.instance(index));
        QIconEngine * engine = iePlugin->create(name);
        QIcon icon = QIcon(engine);
        return icon.pixmap(QSize(requestedSize));
    }

    QStringList iconEngineNames() const
    {
        QMultiMap<int, QString> mm(m_loader.keyMap());
        QList<int> keys = mm.uniqueKeys(); // should be 0 to iconenginecount-1...
        QStringList result;
        for (const int key : qAsConst(keys)) {
            result << mm.value(key);
        }
        return result;
    }

private:
    QFactoryLoader m_loader;
};

int main(int argc, char ** argv) {
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    IconLoaderImageProvider * provider = new IconLoaderImageProvider;
    engine.addImageProvider("icon_engine_provider", provider);
    engine.rootContext()->setContextProperty("engines", provider->iconEngineNames());
    engine.loadData(R"(
    import QtQuick 2
    import QtQuick.Window 2
    import QtQuick.Controls 2
    import QtQuick.Layouts 1
    Window{
        visible: true
        width: 450
        height: 450

        function updateImageUrl() {
            iconImage.source = "image://icon_engine_provider/" + iconEngineList.currentIndex + "/" + iconName.text;
        }

        ColumnLayout {
            anchors.fill: parent
            spacing: 0
            TextField {
                id: iconName
                Layout.fillWidth: true
                text: "folder"
                onEditingFinished: () => { updateImageUrl() }
            }
            ComboBox {
                id: iconEngineList
                Layout.fillWidth: true
                model: engines
                onActivated: () => { updateImageUrl() }
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true

                Image {
                    id: iconImage
                    anchors.centerIn: parent
                    fillMode: Image.PreserveAspectFit
                    source: undefined
                    sourceSize.width: 128
                    sourceSize.height: 128
                }
            }
            Slider {
                id: sizeSlider
                Layout.alignment: Qt.AlignHCenter
                from: 1
                value: 1
                to: 3
                snapMode: Slider.SnapAlways
                stepSize: 1
                onMoved: () => {
                    var wh = 128
                    switch (sizeSlider.value) {
                    case 1:
                        wh = 128; break
                    case 2:
                        wh = 256; break
                    case 3:
                        wh = 384; break
                    }
                    iconImage.sourceSize.width = wh
                    iconImage.sourceSize.height = wh
                }
            }
        }

        Component.onCompleted: {
            updateImageUrl()
        }
    }
    )");

    return app.exec();
}
