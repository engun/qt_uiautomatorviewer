#ifndef MANAGETREEXML_H
#define MANAGETREEXML_H
#include <QTreeWidget>
#include <QDomDocument>


class ManageTreeXml
{
public:
    ManageTreeXml();

    bool SaveTreeXml(QTreeWidget* pTree, const QString& xmlPath);
    bool LoadTreeXml(QTreeWidget* pTree, const QString& xmlPath);
    static QTreeWidgetItem *containsPosSmallest(QTreeWidget *tree, const QPoint &pos);
    QList<QTreeWidgetItem*> search(QTreeWidget *tree, const QString& keyword);
private:
    bool showXmlTreeNode(QTreeWidget* pTree, QDomNode domParent, QTreeWidgetItem *treeParent);
    void setTreeNodeText(const QDomNode& Node, QTreeWidgetItem *treeNode);
    void saveItem(QDomElement& root, QTreeWidgetItem* item, QDomDocument& doc);
    static QMap<QString, QString> attributes2Map(const QDomNamedNodeMap &attributes);

    void showXml(QTreeWidget *pTree, QDomDocument *pDoc);
};

#endif // MANAGETREEXML_H
