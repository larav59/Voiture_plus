# AI Camera

IA Training : https://www.kaggle.com/code/tretomas/sm-voiture-cam-ra-ia

## Lancement de la caméra IA

Il faut lancer le script suivant depuis le répertoire `ai-camera` :

```bash
./start.sh
```

Cela va lancer le script Python qui démarre la camera IA avec les bons paramètres.
Il execute la commande suivante :

```bash
/usr/bin/python3 imx500_object_detection.py --model supercool/network.rpk/network.rpk --labels supercool/labels.txt --fps 16 --bbox-normalization --ignore-dash-labels --bbox-order xy
```

## Todo :

- Ajouter un service systemd pour lancer la caméra IA au démarrage de la voiture après le service de la voiture.
- Ajouter dans le dataset plus d'images pour les obstacles.
- Eviter que le script crash si la socket du serveur du service de la voiture redémarre ou crash.
