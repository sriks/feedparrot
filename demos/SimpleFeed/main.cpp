#include <QtGui/QApplication>
#include <QtDeclarative>
#include <QDeclarativeContext>
#include "qmlapplicationviewer.h"
#include "rssmanager.h"
#include "rssparser.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> app(createApplication(argc, argv));
    RSSManager engine;
    qmlRegisterType<RSSParser>();
    QmlApplicationViewer viewer;
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationLockPortrait);
    viewer.rootContext()->setContextProperty("engine",(QObject*)&engine);
    viewer.setMainQmlFile(QLatin1String("qml/SimpleFeed/main.qml"));
    viewer.showExpanded();
    return app->exec();
}
