# Voiture+ - Superviseur Industriel

## Description

Ce dépôt contient le superviseur industriel du projet **Voiture+**, développé dans le cadre du parcours Smart Mobility de l'IG2I (5ème année).

Voiture+ est un projet global visant à concevoir deux prototypes de véhicules autonomes capables de parcourir un circuit sur la plateforme **UniRAIL**, en respectant le code de la route, les panneaux de signalisation, et en évitant les obstacles à l'aide de caméras embarquées.

Le superviseur industriel est une **application web full-stack** permettant aux utilisateurs de configurer et suivre en temps réel les itinéraires des véhicules.

## Fonctionnalités principales

### Monitoring et gestion des trajets
- Définir un trajet pour chaque véhicule
- Suivi en temps réel des déplacements sur le circuit
- Suivi des évènements du système (alertes)

### Authentification
- Système de connexion sécurisé avec JWT
- Gestion des utilisateurs
- Hachage sécurisé des mots de passe (bcrypt)

## Architecture technique

### Frontend
- **Framework** : Vite (bundler moderne)
- **Langages** : HTML5, CSS3, JavaScript
- **Dépendances** :
    - **Bootstrap** (5.3.8) - Framework CSS
    - **jQuery** (3.7.1) - Manipulation du DOM
    - **Leaflet** (1.9.4) - Cartes interactives
- **Pages** : Login, Dashboard principal, Gestion des alarmes

### Backend
- **Runtime** : Node.js
- **Framework** : Express.js (API RESTful)
- **ORM** : TypeORM pour la gestion de la base de données
- **Authentification** : JWT (jsonwebtoken)
- **Sécurité** : 
    - CORS activé
    - Compression des réponses
    - Validation des données
    - Gestion des erreurs centralisée
- **Utilitaires** : 
    - Multer (upload de fichiers)
    - Sharp (traitement d'images)
    - Archiver (création d'archives)

### Base de données
- **Système** : MariaDB/MySQL
- **Gestion** : TypeORM avec migrations automatiques
- **Pool de connexions** : mysql2

### Infrastructure
- **Logging** : Système de logs personnalisé (console, fichiers)
- **Validation** : Middleware de validation des requêtes
- **Pagination** : Support de la pagination pour les listes
- **Images** : Traitement et stockage optimisé avec Sharp

## 📁 Structure du projet

```
supervision/
├── backend/                    # API REST Node.js/Express
│   ├── src/
│   │   ├── controllers/       # Contrôleurs métier
│   │   ├── domain/            # Entités métier et DTOs
│   │   ├── infrastructure/    # DB, logging, webserver, validation
│   │   ├── middlewares/       # Middlewares Express
│   │   ├── routes/            # Définition des routes API
│   │   ├── app.ts             # Point d'entrée
│   │   └── config.ts          # Configuration
│   ├── package.json
│   └── tsconfig.json
│
└── frontend/                 # Interface web
    ├── js/                   # Scripts JavaScript/jQuery
    ├── css/                  # Feuilles de style
    ├── images/               # Assets (cartes, icônes)
    ├── *.html                # Pages HTML
    ├── package.json
    └── vite.config.js
```

## 🚀 Installation et démarrage

### Prérequis
- Node.js 18+ et npm
- MariaDB/MySQL Server
- Vite CLI (optionnel)

### Backend
```bash
cd supervision/backend
npm install
npm run dev        # Développement avec hot-reload
npm run build      # Compilation TypeScript
npm start          # Démarrage en production
```

### Frontend
```bash
cd supervision/frontend
npx vite        # Serveur de développement Vite
```

## 📦 Dépendances principales

### Backend
| Package | Version | Usage |
|---------|---------|-------|
| express | ^4.19.2 | Framework web |
| typeorm | ^0.3.20 | ORM |
| jsonwebtoken | ^9.0.2 | Authentification JWT |
| bcrypt | ^5.1.1 | Hachage mots de passe |
| mysql2 | ^3.11.0 | Driver MySQL |
| multer | ^2.0.2 | Upload fichiers |
| sharp | ^0.34.3 | Traitement images |

### Frontend
| Package | Version | Usage |
|---------|---------|-------|
| bootstrap | ^5.3.8 | Framework CSS |
| jquery | ^3.7.1 | Manipulation DOM |
| leaflet | ^1.9.4 | Cartes interactives |
| vite | ^7.1.10 | Bundler |
