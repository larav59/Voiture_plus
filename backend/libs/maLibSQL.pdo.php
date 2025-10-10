<?php

include_once "config.php";

/************************************************************************
Ce fichier contient les fonctions nécésaire a la manipulation de la
base de données
On pourra l'utiliser dans le fichier modele.php qui contient les fonctions
de traitement des données de la base de données

************************************************************************/


/**
 * Exécuter une requête UPDATE. Renvoie le nb de modifs ou faux si pb
 * 
 * @param string $sql
 * @pre Les variables  $BDD_login, $BDD_password $BDD_chaine doivent exister
 * @return boolean|int|null|array nombre d'enregistrements affectés, ou false si problème
 */
function SQLUpdate($sql)
{
	global $BDD_host;
	global $BDD_base;
	global $BDD_user;
	global $BDD_password;

	try {
		$dbh = new PDO("mysql:host=$BDD_host;dbname=$BDD_base", $BDD_user, $BDD_password);
	} catch (PDOException $e) {
		die("<font color=\"red\">SQLUpdate/Delete: Erreur de connexion : " . $e->getMessage() . "</font>");
	}

	$dbh->exec("SET CHARACTER SET utf8");
	$res = $dbh->query($sql);
	if ($res === false) {
		$e = $dbh->errorInfo(); 
		die("<font color=\"red\">SQLUpdate/Delete: Erreur de requete : " . $e[2] . "</font>");
	}

	$dbh = null;
	$nb = $res->rowCount();
	if ($nb != 0) return $nb;
	else return false;
	
}


function SQLDelete($sql) {return SQLUpdate($sql);}


/**
 * Exécuter une requête INSERT 
 * @param string $sql
 * @pre Les variables  $BDD_login, $BDD_password $BDD_chaine doivent exister
 * @return int|null|array l'insert ID
 */
function SQLInsert($sql)
{
	global $BDD_host;
	global $BDD_base;
	global $BDD_user;
	global $BDD_password;
	
	try {
		$dbh = new PDO("mysql:host=$BDD_host;dbname=$BDD_base", $BDD_user, $BDD_password);
	} catch (PDOException $e) {
		die("<font color=\"red\">SQLInsert: Erreur de connexion : " . $e->getMessage() . "</font>");
	}

	$dbh->exec("SET CHARACTER SET utf8");
	$res = $dbh->query($sql);
	if ($res === false) {
		$e = $dbh->errorInfo(); 
		die("<font color=\"red\">SQLInsert: Erreur de requete : " . $e[2] . "</font>");
	}

	$lastInsertId = $dbh->lastInsertId();
	$dbh = null; 
	return $lastInsertId;
}



/**
* Effectue une requete SELECT dans une base de données SQL SERVER, pour récupérer uniquement un champ
* Renvoie FALSE si pas de resultats, ou la valeur du champ sinon
* @pre Les variables  $BDD_login, $BDD_password $BDD_chaine doivent exister
* @param string $SQL
* @return false|string
*/
function SQLGetChamp($sql)
{
	global $BDD_host;
	global $BDD_base;
	global $BDD_user;
	global $BDD_password;

	try {
		$dbh = new PDO("mysql:host=$BDD_host;dbname=$BDD_base", $BDD_user, $BDD_password);
	} catch (PDOException $e) {
		die("<font color=\"red\">SQLGetChamp: Erreur de connexion : " . $e->getMessage() . "</font>");
	}

	$dbh->exec("SET CHARACTER SET utf8");
	$res = $dbh->query($sql);
	if ($res === false) {
		$e = $dbh->errorInfo(); 
		die("<font color=\"red\">SQLGetChamp: Erreur de requete : " . $e[2] . "</font>");
	}

	$num = $res->rowCount();
	$dbh = null;

	if ($num==0) return false;
	
	$res->setFetchMode(PDO::FETCH_NUM);

	$ligne = $res->fetch();
	if ($ligne == false) return false;
	else return $ligne[0];

}

/**
 * Effectue une requete SELECT dans une base de données SQL SERVER
 * Renvoie FALSE si pas de resultats, ou la ressource sinon
 * @pre Les variables  $BDD_login, $BDD_password $BDD_chaine doivent exister
 * @param string $SQL
 * @return boolean|resource
 */
function SQLSelect($sql)
{	
 	global $BDD_host;
	global $BDD_base;
 	global $BDD_user;
 	global $BDD_password;

	try {
		$dbh = new PDO("mysql:host=$BDD_host;dbname=$BDD_base", $BDD_user, $BDD_password);
	} catch (PDOException $e) {
		die("<font color=\"red\">SQLSelect: Erreur de connexion : " . $e->getMessage() . "</font>");
	}

	$dbh->exec("SET CHARACTER SET utf8");
	$res = $dbh->query($sql);
	if ($res === false) {
		$e = $dbh->errorInfo(); 
		die("<font color=\"red\">SQLSelect: Erreur de requete : " . $e[2] . "</font>");
	}
	
	$num = $res->rowCount();
	$dbh = null;

	if ($num==0) return false;
	else return $res;
}

/**
* Parcours les enregistrements d'un résultat mysql et les renvoie sous forme de tableau associatif
* @param $result
*/
function parcoursRs($result)
{
	if  ($result == false) return array();

	$result->setFetchMode(PDO::FETCH_ASSOC);
	while ($ligne = $result->fetch()) 
		$tab[]= $ligne;

	return $tab;
}

?>
