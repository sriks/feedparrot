// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import com.nokia.meego 1.0

Page {
    id: mainPage;
    property string pageId;
    orientationLock: PageOrientation.LockPortrait;
    property string src: "http://mobile.engadget.com/rss.xml";
    property double channelFontPointSize: 16
    property double titleFontPointSize: channelFontPointSize-1.5;
    property double contentFontPointSize: titleFontPointSize-1;
    property QtObject parser;
    property alias status: statusText.text;

    Rectangle {
        id: mainView;
        anchors.fill:parent;
        Connections {
            target: engine;
            onUpdateAvailable: {
                parser = engine.parser(src);
                if(parser) {
                    channel.feedName = parser.channelElement("title");
                    feedView.model = parser.count();
                    status = "";
                }
            }
            onError: {
                status = errorDescription+"\n"+url;
            }
        }

        Rectangle {
            id: channel;
            property alias feedName: channelName.text;
            width: parent.width;
            height: channelName.height;
            anchors {
                top: parent.top;
            }

            Text {
                id: channelName;
                smooth: true;
                width: parent.width;
                wrapMode: Text.WordWrap;
                horizontalAlignment: Text.AlignHCenter;
                verticalAlignment: Text.AlignVCenter;
                font.pointSize: channelFontPointSize;
                font.bold: true;
                anchors {
                    centerIn: parent;
                }
            }

            Text {
                id: statusText;
                smooth: true;
                width: parent.width;
                wrapMode: Text.WordWrap;
                font.pointSize: channelFontPointSize;
            }
        }

        Item {
            id: listViewContainer;
            width: parent.width;
            height: parent.height;
            anchors.top: channel.bottom;
            anchors.bottom: parent.bottom;
            ListView {
                id: feedView;
                anchors.fill: parent;
                clip: true;
                anchors.horizontalCenter: parent.horizontalCenter;
                cacheBuffer: 1000;
                delegate: feedItemDelegate;
            }
        }

        Component {
            id: feedItemDelegate;
            Rectangle {
                property bool expanded: false;
                width: parent.width;
                height: feedItemColumn.height;
                color: (index%2===0)?("grey"):("lightgrey");

                MouseArea {
                    id: feedItemMouseArea;
                    anchors.fill: parent;
                    onClicked: expanded = true;
                    onDoubleClicked: expanded = false;
                }

                Column {
                    id: feedItemColumn;
                    width: parent.width;
                    spacing: 4;
                    Text {
                        id: title;
                        smooth: true;
                        width: parent.width;
                        wrapMode: Text.WordWrap;
                        text: if(parser) parser.itemElement(index,"title");
                        font.pointSize: titleFontPointSize;
                    }
                    Text {
                        id: feedDate;
                        smooth: true;
                        width: parent.width;
                        wrapMode: Text.WordWrap;
                        text: if(parser) parser.decodeHtml(parser.itemElement(index,"pubDate"));
                        font.pointSize: title.font.pointSize - 1.5;
                    }

                    Text {
                        id: content;
                        smooth: true;
                        width: parent.width;
                        wrapMode: Text.WordWrap;
                        font.pointSize: contentFontPointSize;
                        text: (expanded)?(parser.decodeHtml(parser.itemElement(index,"description"))):("");
                    }
                }
            }
        }

        Component.onCompleted: {
            engine.add(src); // using default 1 hr interval
            engine.startAll();
            status = "Loading...";
        }
    }
}
// eof
