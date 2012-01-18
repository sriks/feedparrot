// import QtQuick 1.0 // to target S60 5th Edition or Maemo 5
import QtQuick 1.1
import QtWebKit 1.0
import com.nokia.meego 1.0

Page {
    id: mainPage;
    property string pageId;
    orientationLock: PageOrientation.LockPortrait;
    property string src: "http://mobile.engadget.com/rss.xml";
//    property string src: "http://my-meego.com/downloads/harmattan_downloads.xml";
    property double channelFontPointSize: 16
    property double titleFontPointSize: channelFontPointSize-1.5;
    property double contentFontPointSize: titleFontPointSize-1;
    property QtObject parser;

    Component.onCompleted: {
        engine.add(src); // using default 1 hr interval
        engine.startAll();
    }

    SelectionDialog{
        id:feedSelection;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;

        }

        titleText: "Header"
        selectedIndex: 0
        model: ListModel{
            ListElement{name:"Option One"}
            ListElement{name:"Option Two"}
            ListElement{name:"Option Three"}
            ListElement {name:"Option Four"}
        }
    }

    Rectangle {
        id: mainView;
        anchors.fill:parent;
        Connections {
            target: engine;
            onUpdateAvailable: {
                parser = engine.parser(src);
                if(parser) {
                    header.channelName = parser.channelElement("title");
                    feedView.model = parser.count();
                    status = "";
                }
            }
            onError: {
                status = errorDescription+"\n"+url;
            }
        }


        Rectangle {
            id: header;
            color: "red";
            height: mainPage.height/10;
            property alias channelName: feedName.text;
            anchors {
                left: parent.left;
                right: parent.right;
                top: parent.top;
            }

            Label {
                id: feedName;
                anchors.centerIn: parent;
                text: "Feed name"
            }
        }

    Item {
        id: listViewContainer;
        width: parent.width;
        height: parent.height;
        anchors.top: header.bottom;
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


                WebView {
                    id: content;
                    smooth: true;
                    width: parent.width;
                    preferredWidth: parent.width;
                    height: if(!expanded) 0;
                    html: (expanded)?(parser.decodeHtml(parser.itemElement(index,"description"))):("");
                }

            }
        }

    }
}

}
// eof
