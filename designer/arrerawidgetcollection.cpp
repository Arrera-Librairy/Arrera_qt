#include "arrerawidgetcollection.h"

#include "arrera_qt/aapplicationbar.h"
#include "arrera_qt/acheckbox.h"
#include "arrera_qt/acombobox.h"
#include "arrera_qt/aframe.h"
#include "arrera_qt/alabel.h"
#include "arrera_qt/alineedit.h"
#include "arrera_qt/anavbutton.h"
#include "arrera_qt/aprogressbar.h"
#include "arrera_qt/apushbutton.h"
#include "arrera_qt/aradiobutton.h"
#include "arrera_qt/ascrollarea.h"
#include "arrera_qt/asearchbar.h"
#include "arrera_qt/asegmentedbutton.h"
#include "arrera_qt/asettingrow.h"
#include "arrera_qt/aslider.h"
#include "arrera_qt/aswitch.h"

#include <functional>

namespace {
class ArreraWidgetPlugin : public QObject, public QDesignerCustomWidgetInterface
{
public:
    using Factory = std::function<QWidget *(QWidget *)>;

    ArreraWidgetPlugin(QString className,
                       QString includeFile,
                       QString toolTip,
                       bool isContainer,
                       Factory factory,
                       QString domXml = QString(),
                       QObject *parent = nullptr)
        : QObject(parent)
        , _className(std::move(className))
        , _includeFile(std::move(includeFile))
        , _toolTip(std::move(toolTip))
        , _isContainer(isContainer)
        , _factory(std::move(factory))
        , _domXml(std::move(domXml))
    {
    }

    QString name() const override { return _className; }
    QString group() const override { return QStringLiteral("Arrera Qt"); }
    QString toolTip() const override { return _toolTip; }
    QString whatsThis() const override { return _toolTip; }
    QString includeFile() const override { return _includeFile; }
    QIcon icon() const override { return QIcon(); }
    bool isContainer() const override { return _isContainer; }
    bool isInitialized() const override { return _initialized; }

    void initialize(QDesignerFormEditorInterface *core) override
    {
        Q_UNUSED(core);
        _initialized = true;
    }

    QWidget *createWidget(QWidget *parent) override
    {
        return _factory(parent);
    }

    QString domXml() const override
    {
        if (!_domXml.isEmpty()) {
            return _domXml;
        }
        return QDesignerCustomWidgetInterface::domXml();
    }

private:
    QString _className;
    QString _includeFile;
    QString _toolTip;
    bool _isContainer;
    bool _initialized = false;
    Factory _factory;
    QString _domXml;
};

template <typename T>
QDesignerCustomWidgetInterface *makePlugin(const char *className,
                                           const char *includeFile,
                                           const char *toolTip,
                                           bool isContainer,
                                           const QString &domXml = QString(),
                                           QObject *parent = nullptr)
{
    return new ArreraWidgetPlugin(
        QLatin1StringView(className),
        QLatin1StringView(includeFile),
        QLatin1StringView(toolTip),
        isContainer,
        [](QWidget *w) { return new T(w); },
        domXml,
        parent);
}
} // namespace

ArreraWidgetCollection::ArreraWidgetCollection(QObject *parent)
    : QObject(parent)
{
    _widgets = {
        makePlugin<AFrame>(
            "AFrame", "arrera_qt/aframe.h",
            "Conteneur carte arrondi (Filled, Elevated, Outlined)", true,
            QStringLiteral("<widget class=\"AFrame\" name=\"aFrame\"/>"), this),
        makePlugin<AApplicationBar>(
            "AApplicationBar", "arrera_qt/aapplicationbar.h",
            "Barre de navigation latérale", true,
            QStringLiteral("<widget class=\"AApplicationBar\" name=\"aApplicationBar\"/>"), this),
        makePlugin<AScrollArea>(
            "AScrollArea", "arrera_qt/ascrollarea.h",
            "Zone de défilement avec barres fines Arrera", true,
            QStringLiteral("<widget class=\"AScrollArea\" name=\"aScrollArea\"/>"), this),
        makePlugin<APushButton>(
            "APushButton", "arrera_qt/apushbutton.h",
            "Bouton pilule Arrera (Primary, Secondary, Outlined, Flat, Destructive)", false,
            QStringLiteral("<widget class=\"APushButton\" name=\"aPushButton\"><property name=\"text\"><string>Bouton</string></property></widget>"), this),
        makePlugin<ASearchBar>(
            "ASearchBar", "arrera_qt/asearchbar.h",
            "Barre de recherche pilule avec loupe et bouton effacer", false,
            QStringLiteral("<widget class=\"ASearchBar\" name=\"aSearchBar\"><property name=\"placeholderText\"><string>Rechercher...</string></property></widget>"), this),
        makePlugin<ALineEdit>(
            "ALineEdit", "arrera_qt/alineedit.h",
            "Champ texte formulaire avec mode mot de passe et erreur", false,
            QStringLiteral("<widget class=\"ALineEdit\" name=\"aLineEdit\"/>"), this),
        makePlugin<ASwitch>(
            "ASwitch", "arrera_qt/aswitch.h",
            "Interrupteur ON/OFF animé", false,
            QStringLiteral("<widget class=\"ASwitch\" name=\"aSwitch\"><property name=\"text\"><string>Option</string></property></widget>"), this),
        makePlugin<AComboBox>(
            "AComboBox", "arrera_qt/acombobox.h",
            "Liste déroulante stylée Arrera", false,
            QStringLiteral("<widget class=\"AComboBox\" name=\"aComboBox\"/>"), this),
        makePlugin<ACheckBox>(
            "ACheckBox", "arrera_qt/acheckbox.h",
            "Case à cocher animée", false,
            QStringLiteral("<widget class=\"ACheckBox\" name=\"aCheckBox\"><property name=\"text\"><string>Case à cocher</string></property></widget>"), this),
        makePlugin<ARadioButton>(
            "ARadioButton", "arrera_qt/aradiobutton.h",
            "Bouton radio animé", false,
            QStringLiteral("<widget class=\"ARadioButton\" name=\"aRadioButton\"><property name=\"text\"><string>Choix</string></property></widget>"), this),
        makePlugin<ANavButton>(
            "ANavButton", "arrera_qt/anavbutton.h",
            "Bouton de navigation pour AApplicationBar", false,
            QStringLiteral("<widget class=\"ANavButton\" name=\"aNavButton\"><property name=\"text\"><string>Onglet</string></property></widget>"), this),
        makePlugin<ASegmentedButton>(
            "ASegmentedButton", "arrera_qt/asegmentedbutton.h",
            "Sélecteur segmenté en pilule", false,
            QStringLiteral("<widget class=\"ASegmentedButton\" name=\"aSegmentedButton\"/>"), this),
        makePlugin<ASlider>(
            "ASlider", "arrera_qt/aslider.h",
            "Curseur arrondi Arrera", false,
            QStringLiteral("<widget class=\"ASlider\" name=\"aSlider\"/>"), this),
        makePlugin<AProgressBar>(
            "AProgressBar", "arrera_qt/aprogressbar.h",
            "Barre de progression pilule déterminée/indéterminée", false,
            QStringLiteral("<widget class=\"AProgressBar\" name=\"aProgressBar\"/>"), this),
        makePlugin<ALabel>(
            "ALabel", "arrera_qt/alabel.h",
            "Label avec variantes typographiques", false,
            QStringLiteral("<widget class=\"ALabel\" name=\"aLabel\"><property name=\"text\"><string>Texte</string></property></widget>"), this),
        makePlugin<ASettingRow>(
            "ASettingRow", "arrera_qt/asettingrow.h",
            "Ligne de paramètre style AdwActionRow", false,
            QStringLiteral("<widget class=\"ASettingRow\" name=\"aSettingRow\"><property name=\"title\"><string>Paramètre</string></property></widget>"), this)
    };
}

ArreraWidgetCollection::~ArreraWidgetCollection()
{
    qDeleteAll(_widgets);
}

QList<QDesignerCustomWidgetInterface *> ArreraWidgetCollection::customWidgets() const
{
    return _widgets;
}
