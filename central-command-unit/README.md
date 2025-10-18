# Central Command Unit - Documentation

## Description

La Central Command Unit (CCU) est le composant principal de la centrale. Elle est responsable de la coordination et de la gestion de la flotte de véhicules autonomes dans le projet Voiture+.

La CCU est composée de plusieurs services qui communiquent entre eux via MQTT afin de permettre une gestion des véhicules en temps réel et à terme de pouvoir dispatcher les services sur plusieurs machines.

## Micro-services

La CCU est structurée autour des principaux micro-services suivants :

### Gestionnaire de trajet (Route Manager)
- Fonction : Indique aux véhicules les noeuds à atteindre pour suivre l'itinéraire et réajuste le trajet en cas d'obstacles ou de changements.

### Gestionnaire de conflit (Conflict Manager)
- Fonction : Permet aux véhicules de demander l’autorisation pour dépasser ou s’engager sur des zones critiques afin d’éviter les collisions. 

 
### Moniteur de présence (Heartbeat)
- Fonction : Surveille l'état des véhicules et des autres services agissant lorsque le LWT (Last Will and Testament) MQTT est déclenché. Puis alerte les services concernés pour gérer les situations de panne ou de perte de communication.

### Service de synchronisation ferroviaire (Railway Synchronizer)
- Fonction : Donne l’autorisation pour le passage à niveau et reste à l’écoute des informations provenant de la centrale train.  


## Architecture technique
- Langage : C
- Communication inter-services : MQTT


## Compilation et exécution

Pour compiler la CCU, utilisez le Makefile fourni dans le répertoire principal.

```bash
make all
```

Pour exécuter les tests unitaires, utilisez la commande suivante :

```bash
make test-run
```

Les variables d'environnement suivantes peuvent être configurées :

- `CC` : Le compilateur à utiliser (par défaut `gcc`)
- `CFLAGS` : Les options de compilation à passer au compilateur
- `LDFLAGS` : Les options de liaison à passer à l'éditeur de liens


## Configuration des services

TODO

#