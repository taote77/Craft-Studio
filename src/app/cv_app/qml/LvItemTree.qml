import QtQuick 2.0
import QtQuick.Controls 1.4
import QtQml.Models 2.12



TreeView {
    anchors.fill: parent
    model: treeModel  // 绑定Python模型
    delegate: Item {
        Text {
            text: model.display  // 显示节点文本
            x: 20 * styleData.depth  // 根据深度缩进
        }
        // 展开/折叠图标
        Text {
            visible: styleData.hasChildren
            text: styleData.isExpanded ? "▼" : "▶"
        }
    }
}
