<?php

include_once "service.php";

/************************************************************************
Ce fichier contient les appels aux fonctions de traitement des données de la base de données
Celui-ci est uniquement fonctionel avec la base de données de l'application

Toute les definitions de fonctions utiliser dans ce fichier sont dans le fichier modele.php




Fonctionnement :
A la magnère d'une api on apelle ce fichier grace a 
une query string contenant le nom de la fonction que l'on desire utiliser
ainsi que les parametres de la fonction necessaire.
************************************************************************/






// récupération du nom de la fonction a utiliser
if(isset($_GET["nomFonction"]))
    $nomFonction = $_GET["nomFonction"];
else
    $nomFonction = "";

foreach ($_GET as $key => $value) {
    $_GET[$key] = addslashes($value);
}



switch ($nomFonction) {
    case 'getTypeMachine':
        echo json_encode(getTypeMachine());
        break;

    default:
        echo "querry string error";
        break;
}
?>
