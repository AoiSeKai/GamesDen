#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // Check if we are in debug or release mode.
    bool isDebugBuild = false;
#ifdef QT_DEBUG
    isDebugBuild = true;
#endif

    engine.rootContext()->setContextProperty("IS_DEBUG_MODE", isDebugBuild);
    
    const QUrl url(QStringLiteral("qrc:/GamesDenApp/src/main.qml"));
    
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
        
    engine.load(url);

    return app.exec();
}