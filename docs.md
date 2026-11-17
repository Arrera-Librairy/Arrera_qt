# Documentation — Arrera Qt (`Arrera::Qt`)

Guide complet d'utilisation de la bibliothèque **Arrera Qt** (Qt 6 / C++17), incluant la gestion des thèmes dynamiques et le détail de chaque widget.

---

## Table des matières

1. [Installation et intégration CMake](#1-installation-et-intégration-cmake)
2. [Démarrage rapide](#2-démarrage-rapide)
3. [Système de thèmes (`Arrera_Theme`)](#3-système-de-thèmes-arrera_theme)
4. [Conteneurs & Mise en page](#4-conteneurs--mise-en-page)
   - [`AFrame`](#aframe)
   - [`AApplicationBar`](#aapplicationbar)
   - [`AScrollArea` & `AScrollBar`](#ascrollarea--ascrollbar)
5. [Boutons & Navigation](#5-boutons--navigation)
   - [`APushButton`](#apushbutton)
   - [`ANavButton`](#anavbutton)
   - [`ASegmentedButton`](#asegmentedbutton)
6. [Saisie & Contrôles](#6-saisie--contrôles)
   - [`ASearchBar`](#asearchbar)
   - [`ALineEdit`](#alineedit)
   - [`AComboBox`](#acombobox)
   - [`ASwitch`](#aswitch)
   - [`ACheckBox`](#acheckbox)
   - [`ARadioButton`](#aradiobutton)
7. [Feedback & Confort](#7-feedback--confort)
   - [`ASlider`](#aslider)
   - [`AProgressBar`](#aprogressbar)
   - [`ALabel`](#alabel)
   - [`ASettingRow`](#asettingrow)
   - [`AToast`](#atoast)
8. [Utilisation avec Qt Designer](#8-utilisation-avec-qt-designer)

---

## 1. Installation et intégration CMake

La bibliothèque expose une cible unique **`Arrera::Qt`** qui embarque automatiquement les headers (`include/`), les bibliothèques Qt requises (`Qt::Core`, `Qt::Gui`, `Qt::Widgets`) et les 11 thèmes JSON dans les ressources Qt (`:/arrera_qt/theme/...`).

### Via `add_subdirectory` (sous-module Git)
```cmake
add_subdirectory(external/arrera_qt)
target_link_libraries(mon_app PRIVATE Arrera::Qt)
```

### Via `FetchContent`
```cmake
include(FetchContent)

FetchContent_Declare(
    arrera_qt
    GIT_REPOSITORY https://github.com/Arrera-Librairy/Arrera_qt.git
    GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(arrera_qt)

target_link_libraries(mon_app PRIVATE Arrera::Qt)
```

### Via `find_package` (après installation)
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DARRERA_QT_BUILD_EXAMPLES=OFF
cmake --build build
cmake --install build --prefix /chemin/installation
```

```cmake
find_package(arrera_qt REQUIRED)
target_link_libraries(mon_app PRIVATE Arrera::Qt)
```

---

## 2. Démarrage rapide

```cpp
#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>

#include "arrera_qt/arrera_theme.h"
#include "arrera_qt/aframe.h"
#include "arrera_qt/alabel.h"
#include "arrera_qt/apushbutton.h"
#include "arrera_qt/atoast.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Arrera Softwares");
    QCoreApplication::setApplicationName("MonApplication");

    // 1. Initialiser le thème
    Arrera_Theme theme;
    theme.setTheme("default");
    theme.setPersistenceEnabled(true); // Restaure et sauvegarde le choix utilisateur

    // 2. Construire l'interface
    QMainWindow window;
    auto *central = new QWidget(&window);
    auto *rootLayout = new QVBoxLayout(central);

    auto *card = new AFrame(central);
    card->setCardStyle(AFrame::CardStyle::Elevated);
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(20, 20, 20, 20);
    cardLayout->setSpacing(12);

    auto *title = new ALabel("Bienvenue dans Arrera", ALabel::Typography::Title, card);
    auto *btn = new APushButton("Continuer", card);

    QObject::connect(btn, &APushButton::clicked, &window, [&window]() {
        AToast::showToast(&window, "Action validée !", AToast::Type::Success);
    });

    cardLayout->addWidget(title);
    cardLayout->addWidget(btn);
    rootLayout->addWidget(card);

    window.setCentralWidget(central);
    window.resize(520, 320);
    window.show();

    return app.exec();
}
```

---

## 3. Système de thèmes (`Arrera_Theme`)

**Header :** `arrera_qt/arrera_theme.h`  
**Hérite de :** `QObject`

`Arrera_Theme` applique le style `Fusion` et gère deux palettes (`light` et `dark`) ainsi que les tokens de design [`ArreraThemeTokens`](include/arrera_qt/arrera_theme.h). Tous les widgets de la bibliothèque écoutent `QEvent::PaletteChange` et se mettent à jour instantanément à chaud.

### Thèmes intégrés
Les 11 thèmes suivants sont compilés dans la bibliothèque :
`default`, `blanc`, `blanc_gris`, `bleu`, `bleu_blanc`, `bleu_violet`, `gris`, `jaune`, `orange`, `rose`, `rouge`.

### Méthodes & Slots principaux

| Symbole | Description |
| :--- | :--- |
| `Arrera_Theme::instance()` | Retourne l'instance globale (la crée si nécessaire). |
| `Arrera_Theme::availableThemes()` | Retourne la liste des noms de thèmes intégrés (`"default"` en premier). |
| `setTheme(const QString &nameOrPath)` *(slot)* | Charge un thème intégré par nom (`"bleu"`) ou un chemin `.json` externe, sauvegarde si activé, émet `themeChanged`. |
| `loadThemeFromJson(const QString &filePath)` | Charge directement un fichier JSON (ressource `:/...` ou fichier disque). |
| `setThemeMode(ThemeMode mode)` *(slot)* | Définit le mode : `ThemeMode::Auto` (système), `ThemeMode::Light`, ou `ThemeMode::Dark`. |
| `themeMode()` / `activeColorScheme()` | Retourne le mode configuré / le schéma actif (`Qt::ColorScheme`). |
| `currentTheme()` | Retourne le nom ou chemin du thème actif. |
| `setPersistenceEnabled(bool enable, const QString &group)` | Active/désactive la sauvegarde automatique dans `QSettings` (défaut groupe : `"ArreraTheme"`). |
| `restoreSavedTheme()` | Restaure manuellement le thème et le mode depuis `QSettings`. |
| `Arrera_Theme::currentTokens()` | Retourne les tokens actifs (`ArreraThemeTokens`). |

### Signaux
- `void themeChanged(const QString &themeName)`
- `void themeModeChanged(ThemeMode mode)`
- `void colorSchemeChanged(Qt::ColorScheme scheme)`

### Tokens de design (`ArreraThemeTokens`)

| Champ | Type | Défaut / Rôle |
| :--- | :--- | :--- |
| `surfaceContainer` | `QColor` | Surface de fond des barres, champs et cartes remplies (`AlternateBase`) |
| `surfaceElevated` | `QColor` | Surface surélevée des cartes `Elevated`, focus et popups (`Base`) |
| `outline` | `QColor` | Bordure standard (boutons `Outlined`, `AComboBox`, `ALineEdit`, `AFrame::Outlined`) |
| `outlineVariant` | `QColor` | Bordure ultra-subtile (séparateurs, `ASearchBar`, `AFrame::Elevated`) |
| `success`, `warning`, `danger` | `QColor` | Couleurs sémantiques (boutons destructifs, erreurs, `AToast`) |
| `onSemantic` | `QColor` | Couleur de texte sur fond sémantique (défaut : blanc) |
| `radiusSmall` | `int` | `8` px |
| `radiusMedium` | `int` | `12` px |
| `radiusLarge` | `int` | `16` px |
| `animationDuration` | `int` | `140` ms |

### Structure d'un fichier thème JSON personnalisé

```json
{
  "tokens": {
    "radiusSmall": 8,
    "radiusMedium": 12,
    "radiusLarge": 16,
    "animationDuration": 140
  },
  "light": {
    "Window": "#FFFFFF",
    "WindowText": "#0E1726",
    "Base": "#FFFFFF",
    "AlternateBase": "#F6F7FB",
    "Text": "#0E1726",
    "Button": "#FFFFFF",
    "ButtonText": "#0E1726",
    "Highlight": "#4F46E5",
    "HighlightedText": "#FFFFFF",
    "tokens": {
      "outline": "#CBD5E1",
      "outlineVariant": "#E2E8F0",
      "danger": "#D92D20"
    }
  },
  "dark": {
    "Window": "#0E1116",
    "WindowText": "#E8EDF2",
    "Base": "#1E1E1E",
    "AlternateBase": "#12161F",
    "Text": "#E8EDF2",
    "Button": "#11151C",
    "ButtonText": "#E8EDF2",
    "Highlight": "#6D5EFC",
    "HighlightedText": "#0B0F14"
  }
}
```
*(La section `"tokens"` est optionnelle : si elle est absente, `Arrera_Theme` dérive automatiquement tous les tokens à partir de la palette.)*

---

## 4. Conteneurs & Mise en page

### `AFrame`
**Header :** `arrera_qt/aframe.h` · **Hérite de :** `QFrame`

Conteneur carte aux coins arrondis inspiré des `.card` GTK4 et des cartes Material 3.

- **Variantes (`AFrame::CardStyle`) :**
  - `Filled` *(défaut)* : surface douce sans contour (`surfaceElevated` ou `surfaceContainer`).
  - `Elevated` : surface surélevée avec liseré `outlineVariant` et ombre portée douce.
  - `Outlined` : fond transparent avec bordure `1px` `outline`.
- **Propriétés & méthodes :**
  - `cardStyle` / `setCardStyle(CardStyle)`
  - `cornerRadius` / `setCornerRadius(int)` *(suit `tokens.radiusLarge` par défaut)*
  - `backgroundColor` / `setBackgroundColor(const QColor &)` / `resetBackgroundColor()`
- **Signaux :** `cardStyleChanged(CardStyle)`, `cornerRadiusChanged(int)`

```cpp
auto *card = new AFrame(parent);
card->setCardStyle(AFrame::CardStyle::Elevated);
card->setCornerRadius(18);
```

---

### `AApplicationBar`
**Header :** `arrera_qt/aapplicationbar.h` · **Hérite de :** `QFrame`

Barre de navigation latérale arrondie (`minimumWidth` 80 px) avec un `QVBoxLayout` déjà configuré (`margins: 12, 24, 12, 24`, `spacing: 16`), prête à accueillir des [`ANavButton`](#anavbutton).

```cpp
auto *sidebar = new AApplicationBar(parent);
sidebar->layout()->addWidget(new ANavButton(iconHome, "Accueil", sidebar));
sidebar->layout()->addWidget(new ANavButton(iconSettings, "Réglages", sidebar));
static_cast<QVBoxLayout *>(sidebar->layout())->addStretch();
```

---

### `AScrollArea` & `AScrollBar`
**Headers :** `arrera_qt/ascrollarea.h`, `arrera_qt/ascrollbar.h` · **Héritent de :** `QScrollArea`, `QScrollBar`

- `AScrollBar` : barre de défilement minimaliste (4 px au repos, s'élargit à 6 px au survol/glisser, sans flèches, saut direct au clic). Utilisable sur n'importe quel widget scrollable (`QListView`, `QTextEdit`, etc.).
- `AScrollArea` : `QScrollArea` sans bordure avec viewport transparent et deux `AScrollBar` préinstallées (`widgetResizable = true`).

```cpp
auto *scroll = new AScrollArea(parent);
scroll->setWidget(monContenu);
```

---

## 5. Boutons & Navigation

### `APushButton`
**Header :** `arrera_qt/apushbutton.h` · **Hérite de :** `QPushButton`

Bouton pilule animé gérant le survol, l'appui souris/clavier, l'état checkable, l'état désactivé et l'anneau de focus clavier.

- **Constructeurs :**
  - `APushButton(QWidget *parent = nullptr)`
  - `APushButton(const QString &text, QWidget *parent = nullptr)`
  - `APushButton(const QIcon &icon, const QString &text, QWidget *parent = nullptr)`
- **Variantes (`APushButton::Variant`) :**
  - `Primary` *(défaut)* : rempli avec `Highlight`.
  - `Secondary` : style tonal doux (teinte légère de `Highlight` + texte `WindowText`).
  - `Outlined` : fond transparent + bordure `1px` `outline`.
  - `Flat` : transparent au repos, halo au survol.
  - `Destructive` : rouge sémantique (`tokens.danger`) pour les actions critiques.
- **Propriétés & méthodes :**
  - `variant` / `setVariant(Variant)`
  - `cornerRadius` / `setCornerRadius(int)` *(`-1` par défaut = pilule automatique `height / 2`)*
  - `baseColor` / `setBaseColor(const QColor &)` / `resetBaseColor()`

```cpp
auto *saveBtn = new APushButton("Enregistrer", parent);
auto *cancelBtn = new APushButton("Annuler", parent);
cancelBtn->setVariant(APushButton::Variant::Outlined);

auto *deleteBtn = new APushButton("Supprimer", parent);
deleteBtn->setVariant(APushButton::Variant::Destructive);
```

---

### `ANavButton`
**Header :** `arrera_qt/anavbutton.h` · **Hérite de :** `QAbstractButton`

Bouton de navigation checkable et auto-exclusif conçu pour [`AApplicationBar`](#aapplicationbar), avec pilule active animée.

- **Dispositions (`ANavButton::LayoutStyle`) :**
  - `Horizontal` *(défaut)* : icône + texte alignés à gauche dans une pilule pleine largeur (style sidebar GTK4).
  - `Vertical` : pilule centrée `56x32` autour de l'icône avec texte en dessous (style Material 3 Navigation Rail).
- **Propriétés & méthodes :** `layoutStyle` / `setLayoutStyle(LayoutStyle)`

```cpp
auto *navBtn = new ANavButton(QIcon(":/icons/home.svg"), "Accueil", sidebar);
navBtn->setLayoutStyle(ANavButton::LayoutStyle::Vertical);
navBtn->setChecked(true);
```

---

### `ASegmentedButton`
**Header :** `arrera_qt/asegmentedbutton.h` · **Hérite de :** `QWidget`

Sélecteur multi-options en pilule avec indicateur actif coulissant et navigation clavier (`Flèche gauche` / `Flèche droite`).

- **Méthodes :**
  - `int addSegment(const QString &text, const QVariant &data = {})`
  - `int addSegment(const QIcon &icon, const QString &text, const QVariant &data = {})`
  - `void clear()`, `int count() const`
  - `int currentIndex() const`, `setCurrentIndex(int index)` *(slot)*
  - `QVariant currentData() const`, `setCurrentData(const QVariant &data)`
  - `QString segmentText(int index) const`, `QVariant segmentData(int index) const`
- **Signaux :** `currentIndexChanged(int index)`, `segmentClicked(int index)`

```cpp
auto *modeSwitch = new ASegmentedButton(parent);
modeSwitch->addSegment("Auto", QVariant::fromValue(Arrera_Theme::ThemeMode::Auto));
modeSwitch->addSegment("Clair", QVariant::fromValue(Arrera_Theme::ThemeMode::Light));
modeSwitch->addSegment("Sombre", QVariant::fromValue(Arrera_Theme::ThemeMode::Dark));

connect(modeSwitch, &ASegmentedButton::currentIndexChanged, this, [modeSwitch]() {
    Arrera_Theme::instance()->setThemeMode(
        modeSwitch->currentData().value<Arrera_Theme::ThemeMode>()
    );
});
```

---

## 6. Saisie & Contrôles

### `ASearchBar`
**Header :** `arrera_qt/asearchbar.h` · **Hérite de :** `QWidget`

Barre de recherche en pilule (hauteur min. 48 px) avec icône loupe vectorielle à gauche, bouton effacer automatique à droite et transitions hover/focus.

- **Propriétés & méthodes :**
  - `text` / `setText(const QString &)`
  - `placeholderText` / `setPlaceholderText(const QString &)`
  - `clearButtonEnabled` / `setClearButtonEnabled(bool)` *(défaut : `true`)*
  - `searchIconVisible` / `setSearchIconVisible(bool)` *(défaut : `true`)*
  - `searchIcon` / `setSearchIcon(const QIcon &)` *(remplace la loupe vectorielle)*
  - `backgroundColor` / `setBackgroundColor(const QColor &)` / `resetBackgroundColor()`
  - `QLineEdit *lineEdit() const` *(accès direct au champ interne)*
- **Slots :** `clear()`, `selectAll()`
- **Signaux :** `textChanged(QString)`, `textEdited(QString)`, `returnPressed()`, `searchRequested(QString)`, `cleared()`

```cpp
auto *search = new ASearchBar(parent);
search->setPlaceholderText("Rechercher un fichier...");
connect(search, &ASearchBar::searchRequested, this, &MonWidget::lancerRecherche);
```

---

### `ALineEdit`
**Header :** `arrera_qt/alineedit.h` · **Hérite de :** `QWidget`

Champ texte standard pour formulaires (coins `radiusMedium`, hauteur min. 42 px) avec mode mot de passe et état d'erreur.

- **Propriétés & méthodes :**
  - `text` / `setText(const QString &)`, `placeholderText` / `setPlaceholderText(const QString &)`
  - `echoMode` / `setEchoMode(QLineEdit::EchoMode)`
  - `passwordToggleEnabled` / `setPasswordToggleEnabled(bool)` *(défaut : `true`, affiche un bouton œil en mode `Password`)*
  - `clearButtonEnabled` / `setClearButtonEnabled(bool)` *(défaut : `false`)*
  - `readOnly` / `setReadOnly(bool)`
  - `error` / `setError(bool)` *(affiche une bordure `tokens.danger`)*
  - `cornerRadius` / `setCornerRadius(int)`
  - `QLineEdit *lineEdit() const`
- **Slots :** `clear()`, `selectAll()`
- **Signaux :** `textChanged(QString)`, `textEdited(QString)`, `returnPressed()`, `editingFinished()`, `errorChanged(bool)`

```cpp
auto *pwd = new ALineEdit("Mot de passe", parent);
pwd->setEchoMode(QLineEdit::Password);
pwd->setError(motDePasseInvalide);
```

---

### `AComboBox`
**Header :** `arrera_qt/acombobox.h` · **Hérite de :** `QComboBox`

Remplacement direct de `QComboBox` avec coins arrondis, chevron vectoriel qui pivote à l'ouverture et popup thématisé dynamiquement.

- **Propriétés & méthodes :** `cornerRadius` / `setCornerRadius(int)`, plus toute l'API standard `QComboBox` (`addItem`, `addItems`, `currentData`, `currentIndexChanged`, etc.).

```cpp
auto *combo = new AComboBox(parent);
combo->addItems(Arrera_Theme::availableThemes());
connect(combo, &AComboBox::currentTextChanged,
        Arrera_Theme::instance(), &Arrera_Theme::setTheme);
```

---

### `ASwitch`
**Header :** `arrera_qt/aswitch.h` · **Hérite de :** `QAbstractButton`

Interrupteur ON/OFF animé style GTK4 / Material 3 Expressive (le curseur grandit à l'activation et au clic), avec label optionnel à droite.

- **Propriétés & méthodes :**
  - `isChecked()` / `setChecked(bool)` / `toggle()` *(hérités de `QAbstractButton`)*
  - `activeColor` / `setActiveColor(const QColor &)` / `resetActiveColor()`
- **Signaux :** `toggled(bool)`, `clicked(bool)`

```cpp
auto *sw = new ASwitch("Activer les notifications", parent);
sw->setChecked(true);
connect(sw, &ASwitch::toggled, this, &MonWidget::onNotifToggled);
```

---

### `ACheckBox`
**Header :** `arrera_qt/acheckbox.h` · **Hérite de :** `QCheckBox`

Case à cocher animée compatible tri-état (`Qt::Unchecked`, `Qt::PartiallyChecked`, `Qt::Checked`) avec halo au survol.

```cpp
auto *cb = new ACheckBox("Accepter les conditions", parent);
cb->setTristate(true);
```

---

### `ARadioButton`
**Header :** `arrera_qt/aradiobutton.h` · **Hérite de :** `QRadioButton`

Bouton radio animé avec point central progressif et exclusivité automatique entre frères ou dans un `QButtonGroup`.

```cpp
auto *r1 = new ARadioButton("Option 1", groupeWidget);
auto *r2 = new ARadioButton("Option 2", groupeWidget);
r1->setChecked(true);
```

---

## 7. Feedback & Confort

### `ASlider`
**Header :** `arrera_qt/aslider.h` · **Hérite de :** `QSlider`

Curseur horizontal ou vertical à piste épaisse arrondie (8 px), halo interactif et saut direct à la position cliquée.

```cpp
auto *slider = new ASlider(Qt::Horizontal, parent);
slider->setRange(0, 100);
slider->setValue(50);
```

---

### `AProgressBar`
**Header :** `arrera_qt/aprogressbar.h` · **Hérite de :** `QProgressBar`

Barre de progression en pilule avec transition animée sur changement de valeur et mode indéterminé automatique lorsque `minimum() == 0 && maximum() == 0`.

- Par défaut `textVisible` est à `false` (barre fine 8 px). Appeler `setTextVisible(true)` passe la barre à 22 px avec le pourcentage centré.

```cpp
auto *bar = new AProgressBar(parent);
bar->setRange(0, 100);
bar->setValue(65);
bar->setTextVisible(true);

// Mode indéterminé (chargement continu) :
auto *loader = new AProgressBar(parent);
loader->setRange(0, 0);
```

---

### `ALabel`
**Header :** `arrera_qt/alabel.h` · **Hérite de :** `QLabel`

Label typographique suivant automatiquement les changements de thème.

- **Variantes (`ALabel::Typography`) :**
  - `Title` : titre principal en gras (+5 pt)
  - `Subtitle` : sous-titre semi-gras (+2 pt)
  - `Body` *(défaut)* : texte courant
  - `Caption` : légende réduite (-1.5 pt), `muted` activé automatiquement par défaut
- **Propriétés :** `typography` / `setTypography(Typography)`, `muted` / `setMuted(bool)`

```cpp
auto *titre = new ALabel("Paramètres généraux", ALabel::Typography::Title, parent);
auto *info = new ALabel("Synchronisé avec votre compte", ALabel::Typography::Caption, parent);
```

---

### `ASettingRow`
**Header :** `arrera_qt/asettingrow.h` · **Hérite de :** `QWidget`

Ligne de configuration inspirée d'`AdwActionRow` (libadwaita) : icône optionnelle à gauche, titre + sous-titre, séparateur bas optionnel et widget de contrôle aligné à droite.

- **Propriétés & méthodes :**
  - `title` / `setTitle(const QString &)`
  - `subtitle` / `setSubtitle(const QString &)`
  - `icon` / `setIcon(const QIcon &)`
  - `activatable` / `setActivatable(bool)` *(ajoute un retour visuel au survol/clic ; si le widget droit est un bouton checkable comme `ASwitch`, cliquer sur la ligne le bascule automatiquement)*
  - `showDivider` / `setShowDivider(bool)`
  - `setWidget(QWidget *)` / `widget()`
- **Signaux :** `activated()`, `titleChanged(QString)`, `subtitleChanged(QString)`

```cpp
auto *card = new AFrame(parent);
card->setCardStyle(AFrame::CardStyle::Elevated);
auto *layout = new QVBoxLayout(card);
layout->setContentsMargins(0, 4, 0, 4);
layout->setSpacing(0);

auto *row = new ASettingRow("Mode sombre automatique", "Suit l'apparence du système", card);
row->setActivatable(true);
row->setShowDivider(true);
row->setWidget(new ASwitch(row));
layout->addWidget(row);
```

---

### `AToast`
**Header :** `arrera_qt/atoast.h` · **Hérite de :** `QWidget`

Notification flottante en pilule ancrée en bas de la fenêtre parente (se recentre automatiquement si la fenêtre est redimensionnée).

- **Types (`AToast::Type`) :** `Info`, `Success`, `Warning`, `Danger`
- **Utilisation rapide (recommandée) :**
  ```cpp
  // S'auto-détruit proprement après fermeture (3500 ms par défaut, 0 = persistant)
  AToast *toast = AToast::showToast(this, "Modifications enregistrées", AToast::Type::Success, 4000);
  toast->setAction("Annuler");
  connect(toast, &AToast::actionTriggered, this, &MonWidget::annuler);
  ```
- **Utilisation en instance manuelle :** `setMessage()`, `setToastType()`, `setAction()`, `clearAction()`, `setDuration(int ms)`, `showAnimated()`, `dismiss()`.
- **Signaux :** `actionTriggered()`, `dismissed()`

---

## 8. Utilisation avec Qt Designer

### Méthode 1 — Promotion de widgets (sans compilation de plugin)
1. Placez un widget standard Qt dans votre `.ui` (ex. `QFrame`, `QPushButton`, `QComboBox`, `QLineEdit`, `QSlider`, `QProgressBar`, `QCheckBox`, `QRadioButton`, `QLabel`, `QScrollArea`).
2. Clic droit → **Promote to...**
3. Renseignez le nom de la classe Arrera (ex. `APushButton`) et le header correspondant (`arrera_qt/apushbutton.h`).

### Méthode 2 — Plugin natif Qt Designer
Compilez et installez le plugin (avec la **même version de Qt et le même compilateur** que votre Qt Designer) :

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
      -DARRERA_QT_BUILD_DESIGNER_PLUGIN=ON \
      -DARRERA_QT_BUILD_EXAMPLES=OFF
cmake --build build
cmake --install build
```

Les 16 widgets formulaires/conteneurs apparaîtront alors directement dans le panneau latéral de Qt Designer sous le groupe **"Arrera Qt"**.
