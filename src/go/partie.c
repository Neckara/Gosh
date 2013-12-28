#include "gosh_alloc.h"
#include "go/partie.h"
#include "go/plateau.h"
#include "go/position.h"
#include "go/ordinateur.h"

#define JOUEUR_SUIVANT(couleur) ((couleur) == JOUEUR_BLANC ? JOUEUR_NOIR : JOUEUR_BLANC)

Partie creer_partie(void) {
	Partie partie = gosh_alloc(*partie);
	partie->initialisee = false;
	return partie;
}

void detruire_partie(Partie partie) {
	gosh_free(partie);
}

void initialisation_partie(Partie partie, FonctionQuestions fonctionQuestions) {
	bool continuer = true;
	for (int i = 0; i < NOMBRE_QUESTIONS && continuer; i++) {
		if (i == PROGRAMME_JOUEUR_BLANC && partie->joueurs[JOUEUR_BLANC].type != ORDINATEUR)
			continue;
		if (i == PROGRAMME_JOUEUR_NOIR && partie->joueurs[JOUEUR_NOIR].type != ORDINATEUR)
			continue;
		continuer = continuer && fonctionQuestions(i, partie);
	}
	if (continuer) {
		partie->initialisee = true;
	}
	// envoi des informations aux ordinateurs
	for (int j = 0; j < 2; j++) {
		struct s_Joueur joueur = partie->joueurs[j];
		if (joueur.type == ORDINATEUR) {
			ordinateur_debut_partie(joueur.ordinateur, partie);
		}
	}

	partie->joueur_courant = JOUEUR_NOIR;
}

enum CouleurJoueur partie_get_joueur(Partie partie) {
	return partie->joueur_courant;
}

bool partie_jouer_coup(Partie partie, s_Coup coup) {
	bool valide = false;
	if (!POSITION_EST_VALIDE(coup.position)) {
		// le joueur passe son tour
		valide = true;
	} else {
		// placement d'un pion
		s_Pion pion;
		pion.position = coup.position;
		pion.couleur = partie->joueur_courant == JOUEUR_BLANC ? BLANC : NOIR;
		Chaines capturees = plateau_capture_chaines(partie->plateau, pion, &valide);
		if (capturees)
			detruire_ensemble_chaine(capturees);
	}
	if (valide) {
		// notification aux ordinateurs
		enum CouleurJoueur couleur = partie->joueur_courant;
		for (int j = 0; j < 2; j++) {
			struct s_Joueur joueur = partie->joueurs[j];
			if (joueur.type == ORDINATEUR) {
				ordinateur_notifier_coup(joueur.ordinateur, partie, couleur, coup);
			}
		}
		// et on passe au joueur suivant
		partie->joueur_courant = JOUEUR_SUIVANT(partie->joueur_courant);
	}
	return valide;
}

void partie_jouer_ordinateur(Partie partie) {
	Ordinateur ordi = partie->joueurs[partie->joueur_courant].ordinateur;
	ordinateur_jouer_coup(ordi, partie, partie->joueur_courant);
}

