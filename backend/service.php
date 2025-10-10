<?php

/************************************************************************
Ce fichier contient les fonctions de traitement des données de la base de données
Celui-ci est uniquement fonctionel avec la base de données de l'application
************************************************************************/

include_once "libs/maLibSQL.pdo.php";


//Load Composer's autoloader
//require 'vendor/autoload.php';

//$mail = new PHPMailer(true);



/**
 * retourne la liste des machines actives
 * @param mixed $typeMachine id type machine
 * @return array
 */
function getTypeMachine(){
    $SQL = "Select * From type_machine";

    return parcoursRs(SQLSelect($SQL));
}

?>