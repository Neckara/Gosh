#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "cli/configurer_partie.h"
#include "cli/saisie.h"
#include "go/plateau.h"
#include "go/joueur.h"
#include "go/ordinateur.h"


bool saisir_type_joueur(Partie partie, enum CouleurJoueur couleur) {
#ifdef CONFIGURER_PARTIE_AUTOMATIQUEMENT
	partie->joueurs[couleur].type = ORDINATEUR;
	return true;
#endif
	const char* str = couleur == JOUEUR_NOIR ? "Type du joueur noir" : "Type du joueur blanc";
	char res = cli_demander_char(str, 0, 'h', "humain", 'o', "ordinateur", 'r', "retour", 0);

	enum TypeJoueur type;
	if (res == 'o')
		type = ORDINATEUR;
	else if (res == 'h')
		type = HUMAIN;

	partie->joueurs[couleur].type = type;

	return res != 'r';
}

bool saisir_nom_joueur(Partie partie, enum CouleurJoueur couleur) {
#ifdef CONFIGURER_PARTIE_AUTOMATIQUEMENT
	if (couleur == JOUEUR_BLANC)
		strcpy(partie->joueurs[couleur].nom, "Henry");
	else
		strcpy(partie->joueurs[couleur].nom, "Jack");
	return true;
#endif
	const char* str = couleur == JOUEUR_NOIR ? "Nom du joueur noir" : "Nom du joueur blanc";
	char buf[TAILLE_NOM_JOUEUR] = {0};
	cli_demander_string(str, buf, sizeof(buf));

	if (buf[0]) {
		strncpy(partie->joueurs[couleur].nom, buf, TAILLE_NOM_JOUEUR);
	}

	return buf[0] != 0;
}

bool saisir_programme(Partie partie, enum CouleurJoueur couleur) {
	// TODO: saisi
	Ordinateur ordi = charger_ordinateur("src/ordinateurs/librandom.so");
	partie->joueurs[couleur].ordinateur = ordi;
	return true;
}

bool saisir_taille_plateau(Partie partie) {
#ifdef CONFIGURER_PARTIE_AUTOMATIQUEMENT
	partie->plateau = creer_plateau(13);
	return true;
#endif
	const char* str = "Taille du plateau";
	char rep = cli_demander_char(str, 0, 'p', "petit (9x9)",
	                             'm', "moyen (13x13)", 'g', "grand (19x19)", 'r', "retour", 0);
	if (rep == 'p')
		partie->plateau = creer_plateau(9);
	else if (rep == 'm')
		partie->plateau = creer_plateau(13);
	else if (rep == 'g')
		partie->plateau = creer_plateau(19);
	return rep != 'r';
}

bool questions_callback(enum Question question, Partie partie) {
	switch (question) {
		case TYPE_JOUEUR_BLANC:
				return saisir_type_joueur(partie, JOUEUR_BLANC);
		case NOM_JOUEUR_BLANC:
			return saisir_nom_joueur(partie, JOUEUR_BLANC);
		case PROGRAMME_JOUEUR_BLANC:
			return saisir_programme(partie, JOUEUR_BLANC);
		case TYPE_JOUEUR_NOIR:
			return saisir_type_joueur(partie, JOUEUR_NOIR);
		case NOM_JOUEUR_NOIR:
			return saisir_nom_joueur(partie, JOUEUR_NOIR);
		case PROGRAMME_JOUEUR_NOIR:
			return saisir_programme(partie, JOUEUR_NOIR);
		case TAILLE_PLATEAU:
			return saisir_taille_plateau(partie);
		default:
			assert(("question non gérée", false));
			return true;
	}
}

Partie cli_creer_nouvelle_partie(void) {
	Partie partie = creer_partie();
	initialisation_partie(partie, questions_callback);
	if (!partie->initialisee) {
		detruire_partie(partie);
		return NULL;
	}
	return partie;
}
