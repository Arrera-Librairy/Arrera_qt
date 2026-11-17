# TODO — Arrera Qt

Feuille de route pour faire d'`Arrera::Qt` une bibliothèque complète au style **GTK4 (libadwaita) + Material 3 Expressive**.

---

## Déjà en place
- [x] Structure bibliothèque CMake (`arrera_qt` / alias `Arrera::Qt`) et séparation de la démo dans `examples/`
- [x] Embarquement des thèmes JSON en ressources Qt (`:/arrera_qt/theme/...`)
- [x] Gestionnaire de thèmes dynamique [`Arrera_Theme`](include/arrera_qt/arrera_theme.h) (`availableThemes`, `setTheme`, `ThemeMode::Auto/Light/Dark`, persistance `QSettings`, `ArreraThemeTokens`)
- [x] `AFrame` (conteneur arrondi avec variantes `Filled`, `Elevated`, `Outlined`, rayon et fond personnalisables)
- [x] `AApplicationBar` (barre de navigation latérale)
- [x] `ASearchBar` (barre de recherche en pilule : loupe vectorielle, bouton effacer, animations hover/focus, `sizeHint()` et état désactivé)
- [x] `APushButton` (bouton pilule avec variantes `Primary`, `Secondary`, `Outlined`, `Flat`, `Destructive`, états complet et `sizeHint()`)

---

## Nouveaux widgets à créer

### Priorité 1 — Contrôles de base
- [x] **`ASwitch`** : interrupteur ON/OFF animé (style GTK4 / M3)
- [x] **`ALineEdit`** : champ texte standard pour formulaires (coins `10–12px`, bordure discrète au focus, mode mot de passe)
- [x] **`AComboBox`** : liste déroulante stylée pour remplacer la `QComboBox` Fusion par défaut
- [x] **`ACheckBox`** : case à cocher animée cohérente avec le thème
- [x] **`ARadioButton`** : bouton radio cohérent avec le thème

### Priorité 2 — Navigation & feedback
- [x] **`ANavButton`** : bouton checkable (icône + texte + pilule active) dédié à `AApplicationBar`
- [x] **`ASegmentedButton`** : sélecteur segmenté en pilule (2 à 4 options côte à côte, ex. `Auto | Clair | Sombre`)
- [x] **`ASlider`** : curseur avec piste épaisse arrondie
- [x] **`AProgressBar`** : barre de progression arrondie (déterminée et indéterminée)
- [x] **`AScrollBar` / `AScrollArea`** *(ou `QProxyStyle`)* : barres de défilement fines et modernes

### Priorité 3 — Composants confort (style libadwaita)
- [ ] **`ASettingRow`** *(équivalent `AdwActionRow`)* : ligne titre + sous-titre + contrôle aligné à droite (`ASwitch`, `AComboBox`, `APushButton`)
- [ ] **`ALabel`** : variantes typographiques prédéfinies (`Title`, `Subtitle`, `Body`, `Caption`)
- [ ] **`AToast`** : notification flottante discrète en bas de fenêtre

---

## Intégration & distribution
- [ ] Ajouter les règles CMake `install()` / `export` pour la cible `arrera_qt` (`find_package(arrera_qt)`)
- [ ] *(Optionnel)* Ajouter un plugin Qt Designer (`QDesignerCustomWidgetCollectionInterface`) derrière une option CMake `ARRERA_QT_BUILD_DESIGNER_PLUGIN`
- [ ] Compléter `readme.md` avec des exemples d'intégration (`add_subdirectory` / `FetchContent`) et l'API des widgets
