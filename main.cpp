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

        if (!id.contains('/')) return invalid;
        int index = id.section('/', 0, 0).toInt();
        QString name = id.section('/', 1).trimmed();
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
    engine.rootContext()->setContextProperty("qtversion", qVersion());
    engine.loadData(R"(
    import QtQuick 2
    import QtQuick.Window 2
    import QtQuick.Controls 2
    import QtQuick.Layouts 1
    Window{
        visible: true
        width: 450
        height: 450
        color: systemPalette.window
        title: qtversion

        function updateImageUrl() {
            iconImage.source = "image://icon_engine_provider/" + iconEngineList.currentIndex + "/" + iconName.text;
        }

        SystemPalette { id: systemPalette; }

        ColumnLayout {
            anchors.fill: parent
            spacing: 0
            TextField {
                id: iconName
                Layout.fillWidth: true
                text: "folder"
                placeholderText: "xdg icon name or svg icon file path"
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
                color: "transparent"

                Image {
                    id: iconImage
                    anchors.centerIn: parent
                    fillMode: Image.PreserveAspectFit
                    source: undefined
                    sourceSize: Qt.size(128, 128)
                }
            }
            Slider {
                id: sizeSlider
                Layout.alignment: Qt.AlignHCenter
                from: 1
                value: 3
                to: 5
                snapMode: Slider.SnapAlways
                stepSize: 1
                onMoved: () => {
                    var wh = 128
                    switch (sizeSlider.value) {
                    case 1: wh = 22; break
                    case 2: wh = 64; break
                    case 3: wh = 128; break
                    case 4: wh = 256; break
                    case 5: wh = 384; break
                    }
                    iconImage.sourceSize = Qt.size(wh, wh)
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
