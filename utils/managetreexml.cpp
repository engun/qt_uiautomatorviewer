#include "managetreexml.h"
#include <QDebug>
#include <QFile>
#include <QStack>

ManageTreeXml::ManageTreeXml() = default;

bool ManageTreeXml::SaveTreeXml(QTreeWidget *pTree, const QString &xmlPath) {
    QDomDocument doc;
    QDomProcessingInstruction head = doc.createProcessingInstruction(
            "xml", R"(version="1.0" encoding="UTF-8")");
    doc.appendChild(head);
    // invisibleRootItem()函数并不是得到我们想要的这个顶层节点，它得到的是所有节点的最终根节点，因此，得到顶层节点需要自己写操作，下面是根据任意一个节点获取其可见顶层节点的代码
    QTreeWidgetItem *rootItem = pTree->invisibleRootItem(); //树的根节点
    QTreeWidgetItem *childItem = rootItem->child(0);

    QString strRoot;
    strRoot = childItem->text(0);
    QDomElement root = doc.createElement(strRoot);
    doc.appendChild(root);

    saveItem(root, childItem, doc);

    QFile file(xmlPath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    QString xml = doc.toString();
    QTextStream txtOutput(&file);
    txtOutput.setEncoding(QStringConverter::Utf8);
    txtOutput << xml;
    file.close();

    return true;
}

bool ManageTreeXml::LoadTreeXml(QTreeWidget *pTree, const QString &xmlPath) {
    pTree->clear();

    if (xmlPath.isEmpty())
        return false;

    auto *doc = new QDomDocument;
    QFile fl(xmlPath);
    if (!doc->setContent(&fl)) {
        fl.close();
        return false;
    }
    fl.close();

    showXml(pTree, doc);

    return true;
}

void ManageTreeXml::showXml(QTreeWidget *pTree, QDomDocument *pDoc) {

    QDomElement docElem = pDoc->documentElement();

    auto *it = new QTreeWidgetItem;
    auto domNode = docElem.firstChild();
    setTreeNodeText(domNode, it);
    pTree->addTopLevelItem(it);
    showXmlTreeNode(pTree, domNode.firstChild(), it);

    pTree->expandAll();
}

bool ManageTreeXml::showXmlTreeNode(QTreeWidget *pTree, QDomNode domParent,
                                    QTreeWidgetItem *treeParent) {
    while (!domParent.isNull()) {
        auto *it = new QTreeWidgetItem;
        setTreeNodeText(domParent, it);
        treeParent->addChild(it);
        if (domParent.hasChildNodes()) {
            showXmlTreeNode(pTree, domParent.firstChild(), it);
        }
        domParent = domParent.nextSibling();
    }
    return true;
}

void ManageTreeXml::setTreeNodeText(const QDomNode &domNode,
                                    QTreeWidgetItem *treeNode) {

    auto element = domNode.toElement();

    auto map = attributes2Map(element.attributes());

    QString nodeText = "(" + map["index"] + ") ";

    QString packageName = map["class"];
    auto parts = packageName.split('.');
    nodeText += parts.at(parts.length() - 1);

    QString text = map["text"];
    if (text.isEmpty()) {
        text = " " + text;
    } else {
        text = ":" + text;
    }
    nodeText += text;

    nodeText += element.attribute("bounds");

    treeNode->setText(0, nodeText);
    treeNode->setData(0, Qt::UserRole, QVariant::fromValue(map));
}

// 取包含pos且范围最小的node
QTreeWidgetItem *ManageTreeXml::containsPosSmallest(QTreeWidget *tree, const QPoint &pos) {
    QRect rect(0, 0, 0, 0);
    QTreeWidgetItemIterator it(tree);
    QTreeWidgetItem *currentItem = nullptr;
    while (*it) {
        auto role = (*it)->data(0, Qt::UserRole);
        if (role.isValid()) {
            auto map = role.value<QMap<QString, QString>>();
            QString bound = map["bounds"].replace("][", ",");
            bound.replace("[", "");
            bound.replace("]", "");
            auto parts = bound.split(",");
            QPoint topLeft(parts[0].toInt(), parts[1].toInt());
            QPoint bottomRight(parts[2].toInt(), parts[3].toInt());
            QRect r(topLeft, bottomRight);
            if (r.contains(pos) && (rect.width() == 0 || rect.contains(r))) {
                rect = r;
                currentItem = *it;
            }
        }
        ++it;
    }
    return currentItem;
}

QList<QTreeWidgetItem *> ManageTreeXml::search(QTreeWidget *tree, const QString &keyword) {
    QList<QTreeWidgetItem *> items;
    if (keyword.isEmpty()) return items;
    QTreeWidgetItemIterator it(tree);

    while (*it) {
        auto role = (*it)->data(0, Qt::UserRole);
        if (role.isValid()) {
            auto map = role.value<QMap<QString, QString>>();
            if (map["text"].contains(keyword) || map["resource-id"].contains(keyword)) {
                items.push_back(*it);
            }
        }
        ++it;
    }
    return items;
}

QMap<QString, QString> ManageTreeXml::attributes2Map(const QDomNamedNodeMap &attributes) {
    QMap<QString, QString> map;
    int size = attributes.size();
    for (int i = 0; i < size; ++i) {
        auto item = attributes.item(i);
        map[item.nodeName()] = item.nodeValue();
    }
    return map;
}

void ManageTreeXml::saveItem(QDomElement &root, QTreeWidgetItem *item,
                             QDomDocument &doc) {
    int countChild = item->childCount();
    for (int i = 0; i < countChild; i++) {
        QTreeWidgetItem *childItem = item->child(i);
        QString itemTxt = childItem->text(0);
        QDomElement ele = doc.createElement(itemTxt);
        root.appendChild(ele);

        saveItem(ele, childItem, doc);
    }
}
