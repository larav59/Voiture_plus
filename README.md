# Voiture + (superviseur industriel)

## Description

Ce dépôt contient le superviseur industriel du projet Voiture+, développé dans le cadre du parcours Smart Mobility de notre 5ème année à l’IG2I.

Voiture+ est un projet global visant à concevoir deux prototypes de véhicules autonomes capables de parcourir un circuit sur la plateforme UniRAIL, en respectant le code de la route, les panneaux de signalisation, et en évitant les obstacles à l’aide de caméras embarquées.

Le superviseur industriel est une application web développée en JavaScript, HTML et PHP, qui permet aux utilisateurs de configurer et suivre les itinéraires des véhicules.

## Fonctionnalités

- Définir un itinéraire pour chaque véhicule :
    - Choix du point de départ (parking de la plateforme UniRAIL)
    - Choix du point d’arrivée
    - Sélection du sens de circulation (horaire ou trigonométrique)
- Superviser plusieurs véhicules (jusqu'à deux en parallèle)
- Suivi en temps réel des déplacements sur le circuit
- Affichage des contraintes du circuit (panneaux, obstacles)

## Architecture technique

- Frontend : HTML, CSS, JavaScript
- Backend : PHP (traitement des requêtes, échanges avec la centrale)