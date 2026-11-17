# TODO — Arrera Qt

Feuille de route pour faire d'`Arrera::Qt` une bibliothèque complète au style **GTK4 (libadwaita) + Material 3 Expressive**.

---

## Déjà en place
- [x] Structure bibliothèque CMake (`arrera_qt` / alias `Arrera::Qt`) et séparation de la démo dans `examples/`
- [x] Embarquement des thèmes JSON en ressources Qt (`:/arrera_qt/theme/...`)
- [x] Gestionnaire de thèmes dynamique [`Arrera_Theme`](include/arrera_qt/arrera_theme.h) (`availableThemes`, `setTheme`, `ThemeMode::Auto/Light/Dark`, persistance `QSettings`)
- [x] `AFrame` (conteneur arrondi adapté au thème, sans bordure dure)
- [x] `AApplicationBar` (barre de navigation latérale)
- [x] `ASearchBar` (barre de recherche en pilule avec animations hover/focus)
- [x] `APushButton` (bouton pilule avec variantes `Primary`, `Secondary`, `Outlined`, `Flat`, `Destructive`, états complet et `sizeHint()`)

---

## Améliorations sur l'existant
- [ ] **`ASearchBar`** : déplacer l'ajustement de taille de police hors de `updateThemeColors()` (actuellement incrémenté à chaque `PaletteChange`) et ajouter une icône loupe + bouton effacer optionnels
- [ ] **`AFrame`** : ajouter une `Q_PROPERTY` de style (`Filled`, `Elevated`, `Outlined`) configurable dans Qt Designer
- [ ] **`Arrera_Theme` & JSON** : enrichir les thèmes avec des tokens dédiés (rayons d'arrondi, bordures subtiles `OutlineVariant`, couleurs sémantiques succès/avertissement/danger)

---

## Nouveaux widgets à créer

### Priorité 1 — Contrôles de base
- [ ] **`ASwitch`** : interrupteur ON/OFF animé (style GTK4 / M3)
- [ ] **`ALineEdit`** : champ texte standard pour formulaires (coins `10–12px`, bordure discrète au focus, mode mot de passe)
- [ ] **`AComboBox`** : liste déroulante stylée pour remplacer la `QComboBox` Fusion par défaut
- [ ] **`ACheckBox`** : case à cocher animée cohérente avec le thème
- [ ] **`ARadioButton`** : bouton radio cohérent avec le thème

### Priorité 2 — Navigation & feedback
- [ ] **`ANavButton`** : bouton checkable (icône + texte + pilule active) dédié à `AApplicationBar`
- [ ] **`ASegmentedButton`** : sélecteur segmenté en pilule (2 à 4 options côte à côte, ex. `Auto | Clair | Sombre`)
- [ ] **`ASlider`** : curseur avec piste épaisse arrondie
- [ ] **`AProgressBar`** : barre de progression arrondie (déterminée et indéterminée)
- [ ] **`AScrollBar` / `AScrollArea`** *(ou `QProxyStyle`)* : barres de défilement fines et modernes

### Priorité 3 — Composants confort (style libadwaita)
- [ ] **`ASettingRow`** *(équivalent `AdwActionRow`)* : ligne titre + sous-titre + contrôle aligné à droite (`ASwitch`, `AComboBox`, `APushButton`)
- [ ] **`ALabel`** : variantes typographiques prédéfinies (`Title`, `Subtitle`, `Body`, `Caption`)
- [ ] **`AToast`** : notification flottante discrète en bas de fenêtre

---

## Intégration & distribution
- [ ] Ajouter les règles CMake `install()` / `export` pour la cible `arrera_qt` (`find_package(arrera_qt)`)
- [ ] *(Optionnel)* Ajouter un plugin Qt Designer (`QDesignerCustomWidgetCollectionInterface`) derrière une option CMake `ARRERA_QT_BUILD_DESIGNER_PLUGIN`
- [ ] Compléter `readme.md` avec des exemples d'intégration (`add_subdirectory` / `FetchContent`) et l'API des widgets
