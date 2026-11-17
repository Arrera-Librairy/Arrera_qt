# Arrera Qt

Bibliothèque de widgets Qt 6 (C++17) pour les applications **Arrera Softwares**, avec un style hybride inspiré de **GTK4 (libadwaita)** et **Material 3 Expressive**, et prise en charge dynamique des thèmes clair/sombre.

---

## Intégration dans un projet CMake

La bibliothèque expose la cible **`Arrera::Qt`** et embarque automatiquement tous les thèmes JSON intégrés (`:/arrera_qt/theme/...`).

### Option 1 — Via `add_subdirectory` (sous-module Git)
```cmake
add_subdirectory(external/arrera_qt)
target_link_libraries(mon_app PRIVATE Arrera::Qt)
```

### Option 2 — Via `FetchContent`
```cmake
include(FetchContent)

FetchContent_Declare(
    arrera_qt
    GIT_REPOSITORY https://github.com/<utilisateur>/arrera_qt.git
    GIT_TAG main
)
FetchContent_MakeAvailable(arrera_qt)

target_link_libraries(mon_app PRIVATE Arrera::Qt)
```

### Option 3 — Installation système et `find_package(arrera_qt)`
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DARRERA_QT_BUILD_EXAMPLES=OFF
cmake --build build
cmake --install build --prefix /chemin/installation
```

Puis dans le `CMakeLists.txt` de votre application :
```cmake
find_package(arrera_qt REQUIRED)
target_link_libraries(mon_app PRIVATE Arrera::Qt)
```

---

## Gestion des thèmes (`Arrera_Theme`)

Initialisez [`Arrera_Theme`](include/arrera_qt/arrera_theme.h) au démarrage de l'application :

```cpp
#include <QApplication>
#include "arrera_qt/arrera_theme.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("Arrera Softwares");
    QCoreApplication::setApplicationName("MonApp");

    Arrera_Theme theme;
    theme.setTheme("default");          // Thème embarqué (:/arrera_qt/theme/theme_default.json)
    theme.setPersistenceEnabled(true);  // Optionnel : sauvegarde/restaure thème & mode dans QSettings

    // ...
    return app.exec();
}
```

### Changer de thème dynamiquement
```cpp
// Lister les thèmes intégrés ("default", "blanc", "bleu", "orange", etc.)
QStringList themes = Arrera_Theme::availableThemes();

// Changer de thème depuis n'importe où dans l'application
Arrera_Theme::instance()->setTheme("bleu");

// Charger un fichier JSON externe
Arrera_Theme::instance()->loadThemeFromJson("/chemin/vers/mon_theme.json");

// Changer de mode (Auto / Clair / Sombre)
Arrera_Theme::instance()->setThemeMode(Arrera_Theme::ThemeMode::Dark);
```

### Tokens de design (`ArreraThemeTokens`)
Chaque thème expose automatiquement des tokens accessibles via `Arrera_Theme::currentTokens()` (`surfaceContainer`, `surfaceElevated`, `outline`, `outlineVariant`, `success`, `warning`, `danger`, `onSemantic`, `radiusSmall`, `radiusMedium`, `radiusLarge`, `animationDuration`), personnalisables dans une section `"tokens"` des fichiers JSON (voir [theme/theme_default.json](theme/theme_default.json)).

---

## Catalogue des widgets

| Catégorie | Widget | Header | Rôle principal |
| :--- | :--- | :--- | :--- |
| **Conteneurs** | `AFrame` | `arrera_qt/aframe.h` | Carte arrondie (`Filled`, `Elevated`, `Outlined`) |
| | `AApplicationBar` | `arrera_qt/aapplicationbar.h` | Barre latérale de navigation |
| | `AScrollArea` / `AScrollBar` | `arrera_qt/ascrollarea.h` | Zone et barres de défilement fines |
| **Boutons & Nav** | `APushButton` | `arrera_qt/apushbutton.h` | Bouton pilule (`Primary`, `Secondary`, `Outlined`, `Flat`, `Destructive`) |
| | `ANavButton` | `arrera_qt/anavbutton.h` | Bouton d'onglet checkable (`Horizontal` / `Vertical`) pour `AApplicationBar` |
| | `ASegmentedButton` | `arrera_qt/asegmentedbutton.h` | Sélecteur segmenté animé |
| **Saisie & Choix** | `ASearchBar` | `arrera_qt/asearchbar.h` | Barre de recherche pilule avec loupe et bouton effacer |
| | `ALineEdit` | `arrera_qt/alineedit.h` | Champ texte formulaire (mot de passe avec œil, état erreur) |
| | `AComboBox` | `arrera_qt/acombobox.h` | Liste déroulante stylée avec popup thématisé |
| | `ASwitch` | `arrera_qt/aswitch.h` | Interrupteur ON/OFF animé |
| | `ACheckBox` | `arrera_qt/acheckbox.h` | Case à cocher animée (états décoché/coché/partiel) |
| | `ARadioButton` | `arrera_qt/aradiobutton.h` | Bouton radio animé |
| **Feedback & Confort** | `ASlider` | `arrera_qt/aslider.h` | Curseur horizontal / vertical à piste épaisse |
| | `AProgressBar` | `arrera_qt/aprogressbar.h` | Barre de progression déterminée et indéterminée |
| | `ALabel` | `arrera_qt/alabel.h` | Typographie (`Title`, `Subtitle`, `Body`, `Caption`, `muted`) |
| | `ASettingRow` | `arrera_qt/asettingrow.h` | Ligne de paramètre style `AdwActionRow` |
| | `AToast` | `arrera_qt/atoast.h` | Notification flottante (`Info`, `Success`, `Warning`, `Danger`) |

---

## Qt Designer

- **Sans plugin (promotion de widgets)** : ajoutez un widget standard (`QPushButton`, `QFrame`, `QComboBox`, etc.) dans Qt Designer puis faites *Promote to...* avec le header `arrera_qt/<widget>.h`.
- **Avec plugin natif Qt Designer** :
  ```bash
  cmake -S . -B build -DARRERA_QT_BUILD_DESIGNER_PLUGIN=ON -DARRERA_QT_BUILD_EXAMPLES=OFF
  cmake --build build
  cmake --install build
  ```
  Le plugin doit être compilé avec la **même version de Qt et le même compilateur** que Qt Designer.
