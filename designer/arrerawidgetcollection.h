#ifndef ARRERAWIDGETCOLLECTION_H
#define ARRERAWIDGETCOLLECTION_H

#include <QList>
#include <QObject>
#include <QtUiPlugin/QDesignerCustomWidgetCollectionInterface>
#include <QtUiPlugin/QDesignerCustomWidgetInterface>

class ArreraWidgetCollection : public QObject, public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QDesignerCustomWidgetCollectionInterface_iid)
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

public:
    explicit ArreraWidgetCollection(QObject *parent = nullptr);
    ~ArreraWidgetCollection() override;

    QList<QDesignerCustomWidgetInterface *> customWidgets() const override;

private:
    QList<QDesignerCustomWidgetInterface *> _widgets;
};

#endif // ARRERAWIDGETCOLLECTION_H
