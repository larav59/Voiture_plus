# Central Command Unit - Documentation

## Description

La Central Command Unit (CCU) est le composant principal de la centrale. Elle est responsable de la coordination et de la gestion de la flotte de véhicules autonomes dans le projet Voiture+.

La CCU est composée de plusieurs services qui communiquent entre eux via MQTT afin de permettre une gestion des véhicules en temps réel et à terme de pouvoir dispatcher les services sur plusieurs machines.

## Micro-services

La CCU est structurée autour des principaux micro-services suivants :

### Gestionnaire de trajet (Route Manager)
- Fonction : Indique aux véhicules les noeuds à atteindre pour suivre l'itinéraire et réajuste le trajet en cas d'obstacles ou de changements.

### Gestionnaire de conflit (Conflict Manager)
- Objectif : Calculer les itinéraires optimaux pour les véhicules à l'aide de l'algorithme de Dijkstra. Il transforme une demande de trajet en instruction de points à atteindre pour le véhicule.

Au démarrage, ce service récupère la topologie du réseau auprès de l'API.

Il écoute les topics suivants :
- `services/route_manager/request` : Pour recevoir les demandes de trajets et les changement de mode.

Selon l'action demandée, il effectue les traitements suivants :

- **Planification d'itinéraire** :
  - Valide l'existence des noeuds demandés sur la carte.
  - Calcule l'itinéraire optimal segment par segment en utilisant l'algorithme de Dijkstra.
  - Couvertit le chemin en une liste de points navigables.
  - Publie l'itinéraire calculé sur le topic `vehicles/<vehicle_id>/request`.
  - Renvoie le chemin planifié à l'API 
 
### Moniteur de présence (Heartbeat)

- Objectif : Surveiller l'état des véhicules et des autres services agissant lorsque le LWT (Last Will and Testament) MQTT est déclenché. Puis alerte les services concernés pour gérer les situations de panne ou de perte de communication.

Ce service pour l'instant agit en Fire-And-Forget, c'est à dire qu'il ne s'occupe pas de savoir si les commandes envoyées ont bien été reçues et exécutées par les véhicules ou autres services. A terme, un système de confirmation de réception et d'exécution des commandes sera mis en place.

Il écoute les topics suivants :
- `vehicles/+/status` : Pour surveiller l'état des véhicules.
- `services/+/status` : Pour surveiller l'état des autres services de la CCU.

Selon le topic du message reçu, il effectue les actions suivantes :
- **Véhicule hors ligne** :
  - Journalise un avertissement indiquant que le véhicule est hors ligne.
  - Envoie une requête pour annuler le trajet en cours du véhicule.
  - Envoie une requête pour révoquer l'accès aux zones critiques demandées par le véhicule.
- **Service de gestion des conflits hors ligne** :
  - Journalise un avertissement indiquant que le service de gestion des conflits est hors ligne.
  - Envoie une requête pour activer le mode de trajet sécurisé, demandant au planificateur de trajets d'éviter les zones de conflit.
- **Service de planification des trajets hors ligne** :
  - Journalise un avertissement indiquant que le service de planification des trajets est hors ligne.
  - A terme, envisager un mécanisme pour relancer le service.
- **Service de synchronisation ferroviaire hors ligne** :
  - Journalise un avertissement indiquant que le service de synchronisation ferroviaire est hors ligne.
  - Envoie une requête pour désactiver le mode ferroviaire, demandant au planificateur de trajets de ne plus prendre en compte les passages à niveau ni les synchronisations avec les trains.

### Service de synchronisation ferroviaire (Railway Synchronizer)
- Fonction : Donne l’autorisation pour le passage à niveau et reste à l’écoute des informations provenant de la centrale train.  


## Architecture du Projet

### Stack technique

La CCU est développée en langage C pour garantir des performances optimales et une faible latence dans les communications entre les services. La communication entre les micro-services est assurée via le protocole MQTT, utilisant la bibliothèque Libmosquitto pour la gestion des connexions et des messages.

### Structure des répertoires

Le code source de la CCU est organisé en plusieurs répertoires principaux :
- `includes/` : Contient les fichiers d'en-tête (.h) pour lesdifférentes fonctionnalités et modules.
- `src/` : Contient les fichiers source (.c) pour les différents micro-services et modules.
- `external/` : Contient les bibliothèques tierces utilisées par la CCU.
- `bin/` : Répertoire de sortie pour les exécutables compilés.
- `lib/` : Répertoire de sortie pour les bibliothèques dynamiques compilées.
- `mk-lib/` : Contient les Makefiles pour la compilation des bibliothèques externes.
- `services/` : Contient les fichiers de configuration systemd pour chaque micro-service.
- `config_model/` : Contient des exemples de fichiers de configuration INI pour les services.
- `tests/` : Contient les tests unitaires et d'intégration pour les différents modules.
- `docs/` : Contient la documentation du projet.


### Organisation du code

Chaque micro-service est implémenté dans son propre sous-répertoire sous `src/`, avec des fichiers source et d'en-tête dédiés. Les fonctionnalités communes, telles que la gestion MQTT, la journalisation et le parsing de configuration, sont regroupées dans des modules réutilisables sous `src/core/` et `includes/core/`.

L'ensemble du code commun aux services est centralisé dans le module `core`, qui fournit des abstractions et des utilitaires pour faciliter le développement des micro-services de la CCU. Ces fichiers forment une librairie partagée utilisée par tous les services `libcore.so`.

### Threading

Chaque micro-service lance au moins 3 threads :
- Un thread principal qui le fait fonctionner.
- Un thread de gestion MQTT qui s'occupe de la boucle réseau et des callbacks.
- Un thread de journalisation qui s'occupe d'envoyer les messages de log.

## Dépendances

Il est nécessaire d'installer les dépendances suivantes pour compiler et exécuter la CCU :
- **Un broker MQTT :** Eclipse Mosquitto est recommandé
- **Make : Outil** de gestion de compilation
- **GCC :** Compilateur C 

Les dépendances suivantes sont incluses en tant que submodule dans le répertoire `external/` et n'ont pas besoin d'être installées séparément pour pouvoir compiler la CCU :
- [inih](https://github.com/benhoyt/inih) : Parser INI simple en C
- [Libmosquitto](https://mosquitto.org/) : Bibliothèque cliente MQTT et broker MQTT
- [cJSON](https://github.com/DaveGamble/cJSON) : Bibliothèque de manipulation JSON en C

Les dépendances du répertoire `external/` sont compilées automatiquement et génèrent des libraries dynamiques (.so) dans le répertoire `lib/`.

## Compilation et exécution

Tout d'abord veillez à récupérer les submodules git si ce n'est pas déjà fait. Sans cela, la compilation échouera car les dépendances ne seront pas présentes.

```bash
git submodule update --init --recursive
```

Les variables d'environnement suivantes peuvent être configurées :

- `CC` : Le compilateur à utiliser (par défaut `gcc`)
- `CFLAGS` : Les options de compilation à passer au compilateur
- `LDFLAGS` : Les options de liaison à passer à l'éditeur de liens


Pour compiler la CCU, utilisez le Makefile fourni dans le répertoire principal.

```bash
make all
```

La compilation génère un exécutable pour chaque micro-service dans le répertoire `bin/`.
Pour exécuter un micro-service, utilisez la commande suivante en remplaçant `<service>` par le nom du service souhaité (par exemple `heartbeat`).

```bash
./bin/<service> -c path/to/config.ini
```

Vérifier bien que la variable d'environnement `LD_LIBRARY_PATH` inclut le répertoire `lib/` pour que les bibliothèques dynamiques soient trouvées lors de l'exécution.

```bash
export LD_LIBRARY_PATH=$(pwd)/lib:$LD_LIBRARY_PATH
```

## Système de déploiement et d'installation

Le projet dispose désormais d'un système de déploiement automatisé via des scripts bash (`deploy.sh` et `install.sh`). 

Ces scripts gèrent la compilation, l'installation des bibliothèques, la configuration et l'enregistrement des services systemd.

### Déploiement à distance

Le script `deploy.sh` permet de déployer la CCU sur une machine distante via SSH.

Usage :

```bash
./deploy.sh -h <ip_cible> -p <mot_de_passe> -u <utilisateur> -s <service1> [service2 ...]
```
Arguments :
- `-h <ip_cible>` : Adresse IP de la machine distante
- `-p <mot_de_passe>` : Mot de passe SSH pour l'utilisateur
- `-u <utilisateur>` : Nom d'utilisateur SSH
- `-s <service1> [service2 ...]` : Liste des services à déployer (exemple : `heartbeat route_manager`)
- `-f` : Forcer l'écrasement des fichiers de configuration existants sur la machine distante

Par exemple, pour déployer les services Heartbeat et Route Manager sur une machine distante :

```bash
./deploy.sh -h 192.168.1.10 -p raspberry -s heartbeat route_manager
```

### Installation locale

Le script `install.sh` permet d'installer les services de la CCU en tant que services systemd sur la machine locale. Vous devez exécuter ce script avec des privilèges root car il installe les fichiers dans des répertoires système et enregistre les services systemd.

Usage :

```bash
sudo ./install.sh <service1> [service2 ...]
```

Exemple pour installer les services Heartbeat et Route Manager :

```bash
sudo ./install.sh heartbeat route_manager
```

### Emplacement des fichiers

Une fois installés, les fichiers sur le système se trouvent aux emplacements suivants :

- Binaires des services : `/usr/local/bin/`
- Blibliothèques partagées : `/usr/local/lib/`
- Fichiers de configuration : `/etc/ccu/`
- Fichiers de logs : `/var/log/ccu/`
- Fichiers de services systemd : `/etc/systemd/system/`

### Gestion des services systemd

Après l'installation, vous pouvez gérer les services de la CCU comme n'importe quel autre service systemd.

```bash
# Démarrer un service
sudo systemctl start <service>.service
# Arrêter un service
sudo systemctl stop <service>.service
# Redémarrer un service
sudo systemctl restart <service>.service
# Vérifier le statut d'un service
sudo systemctl status <service>.service
# Activer un service au démarrage
sudo systemctl enable <service>.service
# Désactiver un service au démarrage
sudo systemctl disable <service>.service
# Voir les logs d'un service
sudo journalctl -u <service>.service -f
```


## Configuration des services

Chaque service de la CCU peut être configuré via un fichier INI. Le chemin vers ce fichier est passé en argument lors du lancement du service avec l'option `-c` ou `--config` suivie du chemin du fichier de configuration.

### Section Network

La section `[Network]` du fichier de configuration contient les paramètres de connexion au broker MQTT.

- `mqtt_broker_address` : Adresse du broker MQTT (exemple : `192.168.1.100`)
- `mqtt_broker_port` : Port du broker MQTT (exemple : `1883`)
- `mqtt_client_id` : ID unique du client MQTT (exemple : 'Heartbeat")
- `mqtt_timeout_sec` : Temps d'attente en secondes pour la connexion au broker MQTT (exemple : `60`)

### Journalisation (Logging)

Chaque service utilise un système de journalisation (logging) pour enregistrer les événements importants, les erreurs et les informations de débogage. Les messages de log sont publiés sur un topic MQTT dédié ainsi que sur la console standard.

- `log_topic` : Topic MQTT pour les messages de log (exemple : `system/logs/`)
- `log_level` : Niveau de log (exemple : `DEBUG`, `INFO`, `WARNING`, `ERROR`)

