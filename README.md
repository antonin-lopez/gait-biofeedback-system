```python
readme_content = """# Gait Biofeedback System (Système de Biofeedback de la Démarche)

Un système embarqué de biofeedback en temps réel conçu pour analyser la biomécanique de la course à pied, détecter les asymétries de foulée et prévenir les blessures. Ce projet repose sur une architecture multi-capteurs sans fil utilisant des modules **M5Stick-C Plus2** (basés sur l'ESP32), communiquant via le protocole ultra-rapide **ESP-NOW**.

---

## 🚀 Fonctionnalités Principales

- **Échantillonnage Haute Fréquence Strict (100 Hz)** : Capture précise des forces d'impact via l'accéléromètre (IMU) interne des capteurs de cheville, régulée sans dérive temporelle.
- **Filtrage Évolué des Impacts** : Algorithme avec seuils physiques configurables et période réfractaire de 250 ms pour éliminer les faux positifs (rebonds, oscillations).
- **Calibration Personnalisée** : Établissement d'une ligne de base (baseline) biomécanique sur les 16 premiers pas de chaque jambe pour s'adapter à la morphologie de l'utilisateur.
- **Analyse d'Asymétrie en Temps Réel** : Calcul instantané basé sur une moyenne glissante (tampon circulaire de 8 pas) permettant une réactivité optimale du biofeedback.
- **Architecture Réseau Robuste & Sûre** : Découplage de la pile réseau asynchrone et de la logique métier principale via une file d'attente (**FreeRTOS Queue**), éliminant tout risque de *race condition*.
- **Interface Audio-Visuelle Dynamique** : Changements de couleurs d'arrière-plan de l'écran (Vert/Rouge/Bleu) et signaux sonores (beeps de fréquences adaptatives) selon l'état actuel du système et les alertes d'asymétrie.
- **Surveillance de l'état du matériel** : Suivi des niveaux de batterie du bracelet et de chaque cheville avec détection de déconnexion automatique (Timeout 1500 ms).

---

## 📐 Algorithmes Biomécaniques

### 1. Détection des Pics d'Impact
Chaque capteur de cheville calcule en continu la magnitude du vecteur accélération :
$$\|A\| = \sqrt{x^2 + y^2 + z^2}$$

Lorsqu'elle dépasse le seuil `DEFAULT_DETECTION_THRESHOLD` (3.5G), le système entre en phase d'impact et mémorise la force maximale (crête). L'impact est validé et transmis dès que la magnitude repasse sous le seuil.

### 2. Calcul de l'Asymétrie
L'asymétrie est calculée au niveau du bracelet à l'aide de la formule normalisée suivante :
$$\text{Asymétrie (\%)} = \frac{|Moyenne_{Gauche} - Moyenne_{Droite}|}{\max(Moyenne_{Gauche}, Moyenne_{Droite})} \times 100$$

Si cette valeur dépasse le seuil personnalisé calculé lors de la calibration ($Baseline + 5.0\%$), le système bascule immédiatement en état d'alerte (`RUNNING_ALERT`).

---

## 🏗️ Architecture Logicielle

Le projet est découpé de manière modulaire afin de faciliter sa maintenance et de bien séparer le matériel de la logique algorithmique :


```

```text
README.md successfully created!


```

├── .gitignore
├── platformio.ini              # Configuration PlatformIO multi-environnements
├── boards/
│   └── m5stick-c-plus2.json    # Définition matérielle de la carte custom
├── include/
│   └── Protocol.h              # Constantes, énumérations (SystemState) et structures réseau
├── lib/
│   ├── Algorithms/             # Logique d'analyse de la marche (ImpactDetector, GaitAnalyzer)
│   │   ├── GaitAlgorithms.h
│   │   └── GaitAlgorithms.cpp
│   └── HAL/                    # Couche d'Abstraction Matérielle (Hardware Abstraction Layer)
│       ├── Hardware.h
│       └── Hardware.cpp
└── src/
├── ankle/                  # Code source pour les modules chevilles (Gauche / Droit)
│   └── main.cpp
└── wrist/                  # Code source pour le module bracelet (Maître / FSM / UI)
└── main.cpp

```

### Machine d'État Finie (FSM) du Bracelet
Le système évolue à travers différents états (`SystemState`) gérés par les pressions sur le bouton principal (Bouton A) :
- **`IDLE` (Noir)** : Repos, le système attend les connexions.
- **`DIAGNOSTIC` (Jaune)** : Mode de test permettant de valider l'appairage et le fonctionnement des capteurs (émet un bip à chaque impact).
- **`CALIBRATION` (Bleu)** : Phase initiale de course à pied récoltant 16 pas par côté pour calibrer le profil de l'utilisateur.
- **`RUNNING_NORMAL` (Vert)** : Mode de course standard affichant la répartition de force Gauche/Droite.
- **`RUNNING_ALERT` (Rouge)** : Alerte active d'asymétrie excessive nécessitant une correction de foulée.
- **`PAUSE` (Magenta)** : Course suspendue (manuellement ou suite à la déconnexion d'une cheville).

---

## 📡 Protocole Réseau (ESP-NOW)

Les données sont transmises sans fil en paquets bruts, optimisés au maximum via l'attribut de compaction de structure C++ `#pragma pack(push, 1)`.

### Message d'Impact (`ImpactMessage`)
Envoyé par une cheville au bracelet dès qu'une foulée est détectée.
```cpp
struct ImpactMessage {
    float peakForce;     // Force max mesurée en G
    uint8_t isLeft;      // 1 = Jambe Gauche, 0 = Jambe Droite
    uint32_t seqNum;     // Compteur séquentiel pour le suivi des pertes de paquets
};

```

### Message de Présence (`HeartbeatMessage`)

Émis toutes les 500 ms par chaque cheville pour notifier de sa présence et de son état d'autonomie.

```cpp
struct HeartbeatMessage {
    DeviceRole role;     // Rôle de l'appareil (ANKLE_LEFT ou ANKLE_RIGHT)
    uint8_t batteryLevel;// Niveau de batterie restant (0 à 100%)
};

```

Le bracelet écoute en continu et injecte instantanément les paquets reçus dans une file d'attente FreeRTOS, garantissant qu'aucun traitement lourd ne s'exécute dans le thread d'interruption réseau (ISR).

---

## 🛠️ Installation et Compilation

Le projet utilise **PlatformIO** au sein d'un environnement d'IDE de type VS Code.

### Prérequis

1. Installez [Visual Studio Code](https://code.visualstudio.com/).
2. Installez l'extension **PlatformIO IDE**.
3. Connectez vos modules M5Stick-C Plus2 en USB.

### Déploiement des Environnements

Le fichier `platformio.ini` gère intelligemment trois cibles de compilation distinctes grâce aux drapeaux de préprocesseur :

1. **Flasher le Bracelet (Wrist)** :
```bash
pio run -e wrist --target upload

```


2. **Flasher la Cheville Gauche (Ankle Left)** :
```bash
pio run -e ankle_left --target upload

```


3. **Flasher la Cheville Droite (Ankle Right)** :
```bash
pio run -e ankle_right --target upload

```



---

## 🎮 Mode d'Emploi

1. **Démarrage** : Allumez les trois modules. Les modules chevilles affichent leur côté assigné (`GAUCHE` ou `DROITE`). Le bracelet démarre en mode `REPOS` (écran noir).
2. **Test de connexion (`DIAGNOSTIC`)** : Appuyez brièvement sur le bouton du bracelet pour passer en mode Diagnostic (écran jaune). Vérifiez que les pourcentages de batterie des modules de cheville s'affichent correctement en bas de l'écran (`G: XX% D: XX%`). Faites une simulation d'impact au sol pour vérifier le déclenchement du signal sonore. Ré-appuyez brièvement pour revenir au repos.
3. **Calibration** : Effectuez un appui long sur le bouton pour lancer la `CALIBRATION` (écran bleu). Commencez à courir. L'écran scindé affiche le nombre de pas restants à accomplir pour chaque jambe (ex: `G: 16` | `D: 16`).
4. **Course & Biofeedback** : Une fois les 32 pas accumulés, un long bip retentit et l'écran devient vert (`COURSE OK`). L'écran affiche en permanence la distribution dynamique des forces (ex: `G: 49%` | `D: 51%`).
5. **Alerte** : Si votre foulée devient asymétrique (fatigue, boiterie), l'écran vire instantanément au rouge et émet un signal sonore strident pour vous inviter à corriger votre posture de course.
6. **Mise en Pause / Arrêt** : Un appui bref pendant la course met le système en `PAUSE` (écran magenta). Un appui long à tout moment réinitialise le système et le renvoie à l'état `REPOS`.

---

## 📜 Licence

Ce projet est développé sous licence privée dans le cadre de recherche et développement sur le biofeedback de la marche humaine.
"""

with open("README.md", "w", encoding="utf-8") as f:
f.write(readme_content)

print("README.md successfully created!")

```
Votre fichier Markdown **README.md** complet et professionnel est prêt.

[file-tag: code-generated-file-0-1780996029477266379]

Ce fichier a été rédigé avec soin en suivant les meilleures pratiques de documentation pour l'informatique embarquée et la biomécanique. Voici un aperçu des sections majeures qu'il contient :

1. **🚀 Fonctionnalités Principales** : Résumé des points clés (Échantillonnage 100 Hz régulé, filtrage anti-rebond, calibration personnalisée, et utilisation asynchrone sécurisée des Queues FreeRTOS pour découpler le réseau).
2. **📐 Algorithmes Biomécaniques** : Présentation mathématique formalisée (en syntaxe LaTeX) du calcul de la magnitude du vecteur accélération et du calcul de l'index d'asymétrie basé sur la moyenne glissante.
3. **结构 Architecture Logicielle** : Arborescence claire du projet montrant le rôle de chaque fichier, incluant une explication détaillée de la **Machine d'État Finie (FSM)** du bracelet.
4. **📡 Protocole Réseau (ESP-NOW)** : Description des structures de données compactées (`#pragma pack(push, 1)`) pour les messages d'impact et de présence (`Heartbeat`).
5. **🛠️ Installation, Configuration et Compilations** : Instructions précises pour PlatformIO expliquant comment compiler et flasher indépendamment le bracelet (`wrist`), la cheville gauche (`ankle_left`) et la cheville droite (`ankle_right`).
6. **🎮 Mode d'Emploi de l'utilisateur** : Guide d'utilisation pas à pas à destination d'un coureur ou d'un examinateur (Diagnostic, Calibration scindée avec décompte des pas restants par jambe, alertes visuelles et sonores en cas d'asymétrie).

Vous pouvez directement placer ce fichier à la racine de votre dépôt Git pour disposer d'une documentation de niveau professionnel !

```
