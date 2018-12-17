
//////////////////////////////////////////////////////////////////////////////////////////////
/* FileName:		Belegaufgabe Schachspiel												*/
/* FileType:		C Source file															*/
/* Author:			Sven Pieper							      ________						*/
/* S-Nummer:		s76042								     / ____/ /_  ___  __________	*/
/* Matrikel:		41887								    / /   / __ \/ _ \/ ___/ ___/	*/
/* Semester:		1									   / /___/ / / /  __(__  |__  )		*/
/* Studiengruppe:	16/043/61							   \____/_/ /_/\___/____/____/		*/
/* Prüfer:			Prof. Dr.-Ing. Kai Bruns												*/
/* Fach:			I-120 Programmierung I													*/
/*																							*/
/* Last edited:		14.12.2016 19:11														*/
//////////////////////////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>
void gotoXY(int x, int y) {
	HANDLE  hConsoleOutput; COORD koords;
	koords.X = x; koords.Y = y;
	hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hConsoleOutput, koords);
}



/* Globale Variable für Figurposition */
char figuren[8][8] = {
	{ 'T', 'S', 'L', 'K', 'D', 'L', 'S', 'T' },
	{ 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B' },
	{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
	{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
	{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
	{ ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
	{ 'b', 'b', 'b', 'b', 'b', 'b', 'b', 'b' },
	{ 't', 's', 'l', 'k', 'd', 'l', 's', 't' }
};
/*** Globale Laufvariable, wird für Spielerwechsel benötigt ***/
int sw = 2;			



/*** Leerzeile via printf ausgeben ***/
void printf_leerzeile(int zeilenanzahl) {
	int i;

	for (i = 0; i < zeilenanzahl; i++) {
		printf("                                                                                                      \n");
	}
}



/*** Stackspeicher für Undo-Funktion ***/
void stack_undo(int auswahl) {
	static int i = 0;								//Laufvariable i enspricht aktuellen Stackspeicherpunkt
	int x = 0;										//Laufvariable x dient der Einlesung der Erstaufstellung im Stack auf Pos. 0, für neue sowie geladene Spiele
	int a, b;										//Zähler für 8x8 Aufstellungen in doppelter for-Schleife
	static char stack[100][64];						//Stackspeicher mit Platz für 100 Züge. Hier werden die 64 Felder des Schachbretts gespeichert
	
	switch (auswahl)
	{
	/* Erstaufstellung */
	case 0:											//Intialisierung der Erstaufstellung im Stack auf Pos. 0, kann Standartaufstellung oder Geladene Startaufstellung sein
		for (a = 0; a < 8; a++) {				
			for (b = 0; b < 8; b++) {
				stack[0][x] = figuren[a][b];
				x++;
			}
		} break;
	/* PUSH-Funktion des Stacks */
	case 1:											//PUSH: Stack mit neuster Aufstellung füttern
		i++;										//Vorher noch i erhöhen damit neuer Arraybereich angesprochen wird
		x = 0;
		for (a = 0; a < 8; a++) {
			for (b = 0; b < 8; b++) {
				stack[i][x] = figuren[a][b];
				x++;
			}
		} break;
	/* POP-Funktion des Stacks */
	case 2:											//POP: Stack erniedrigen, Definition neuste Aufstellung um 1 minimieren
		if (i == 0) {								//UNDO Funktion wird nicht ermöglicht falls Stackspeicher schon alle Schritte komplett rückgängig gemacht wurden
			sw++; break;
		}
		i--;										//Vorher noch i erniedrigen damit Figuren-Array mit alter Aufstellung bestückt werden kann.
		for (a = 0; a < 8; a++) {
			for (b = 0; b < 8; b++) {
				figuren[a][b] = stack[i][x];
				x++;								
			}
		} break;
	default:
		break;
	}

}



/*** Speicherung und Beenden des Spiels ***/
void spiel_speichern_und_beenden() {
	FILE *fp;
	int i, y;
	char name[80];

	system("cls");
	printf("\nBitte geben Sie noch den Namen des Spiels an um es zu speichern:\n\n > ");
	fseek(stdin, 0, SEEK_END);											//Puffer löschen
	gets(name);
	fseek(stdin, 0, SEEK_END);											//Puffer löschen
	strcat(&name, ".txt");												//.txt an Dateiname anhängen um es für fopen lauffähig zu machen
	fp = fopen(name, "w");
	if (fp == NULL) {
		printf("\nFehler beim Speichern der Datei.\n");
		exit(0);
	}
	else {
		for (i = 0; i < 8; i++) {										//Doppelte For-Schleife um jedes Zeichen des
			for (y = 0; y < 8; y++) {									//2-dim. Arrays nacheinander speichern zu können
				fputc(figuren[i][y], fp);								//Speichern des Zeichens auf der txt-Datei
			}
		}
		fprintf(fp, "%i", sw);											//Laufvariable sw wird in txt-Datei gespeichert, damit später der richtige Spieler am Zug ist
		fclose(fp);														//Datei schließen
	}
	system("cls");
	printf("\nSpiel wurde erfolgreich gespeichert und beendet!\n\n");
	Sleep(3000);
	exit(0);															//Beenden des Programms
}



/*** Datei auf Existenz überprüfen. Mit Rückgabe wert -1 oder 0 ***/
int file_exist(char *dateiname) {
	FILE *datei_ptr;
	int result;
	datei_ptr = fopen(dateiname, "r");
	if (datei_ptr != NULL) {
		result = -1;					//Datei existiert
		fclose(datei_ptr);				//Datei schließen
	}
	else {
		result = 0;						//Datei existiert nicht
	}
	return result;						//Rückgabewert wird für Funktion spielstand_laden benötigt um nicht vorhandene Spielstände abzugreifen
}



/*** Spielstand wird geladen ***/
void spielstand_laden() {
	int i, y;
	char *name[80];

	printf("\nBitte geben Sie den Namen Ihres Spielstandes ein.\n\n > ");
	fseek(stdin, 0, SEEK_END);											//Puffer löschen
	gets(name);															//Dateinamen in char Array einlesen
	fseek(stdin, 0, SEEK_END);											//Puffer löschen
	strcat(&name, ".txt");												//.txt an Dateinamen anhängen

	if (file_exist(name) == 0) {										//Prüfung ob eine Datei mit dem eingegebenen Dateinamen existiert, vorher wird keine Datei geöffnet
		system("cls");
		printf("\nSpielstand nicht gefunden!\n");
		spielstand_laden();
	}
	else if (file_exist(name) == -1) {
		FILE *fp;
		fp = fopen(name, "r");
		for (i = 0; i < 8; i++) {										//Doppelte For-Schleife um jedes Zeichen der
			for (y = 0; y < 8; y++) {									//txt-Datei auf die richtige Stelle im 2-dim. Array speichern zu können
				figuren[i][y] = fgetc(fp);
			}
		}
		fscanf(fp, "%i", &sw); sw--;									//Laufvariable sw wird geladen, damit der richtige Spieler wieder am Zug ist.
		fclose(fp);
	}
	stack_undo(0);														//Pos. 0 im Undo-Stack, nicht wie bei neuem Spiel, Standardaufstelllung speichern
																		//sondern die geladene Aufstellung, damit bei mehrmaligem Undo nicht ausversehen
																		//das geladene Spiel mit Standartaufstellung überschrieben wird
	system("cls");
}



/*** Anzeige des Startmenüs mit Auswahlmöglichkeiten ***/
void startmenu() {
	int auswahl;

	printf("*------------------------------------------------------------------------------*\n");
	printf("*------------------------------------------------------------------------------*\n\n\n");
	printf(" Was wollen Sie tun?\n");
	printf(" Bitte 1 2 oder 3 eingeben um Menuepunkt auszuwaehlen.\n\n");
	printf(" (1) Neues Spiel\n (2) Alten Spielstand laden\n (3) Beenden\n\n");
	printf("  > ");
	fseek(stdin, 0, SEEK_END);						//Eingabepuffer löschen
	scanf("%i", &auswahl);							//auswahl über Tastatureingabe bestimmen
	fseek(stdin, 0, SEEK_END);

	if (auswahl == 1) {								//Auswahl um neues Spiel zu spielen
		system("cls");								//Bild wird gelöscht, da im nächsten Zug das Feld angezeigt wird
	}
	else if (auswahl == 2) {						//Auswahl um Altes Spiel zu laden
		system("cls");
		spielstand_laden();							//Funktionsaufruf für Spielstand laden
	}
	else if (auswahl == 3) {						//Auswahl zum Beenden des Spiels
		system("cls");
		printf("\nSpiel wurde beendet!\n\n");
		exit(0);									//Programm wird beendet
	}
	else {
		system("cls");
		startmenu();								//Für alles flasche wird das Menü erneut augerufen damit erneut eingegeben werden kann
	}

}



/*** Darstellung des Schachbretts im 8 x 8 Muster ***/
void schachlinien(int x) {
	switch (x) {
	case 0:
		printf("\n     a        b        c        d        e        f        g        h\n");
		printf("+-----------------------------------------------------------------------+\n");
		printf("|%c%c%c%c%c%c%c%c|        |%c%c%c%c%c%c%c%c|        |%c%c%c%c%c%c%c%c|        |%c%c%c%c%c%c%c%c|        |\n",219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219 );
		break;
	case 1:
		printf("|%c%c%c%c%c%c%c%c|        |%c%c%c%c%c%c%c%c|        |%c%c%c%c%c%c%c%c|        |%c%c%c%c%c%c%c%c|        |\n",219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219);
		printf("+-----------------------------------------------------------------------+\n");
		printf("|        |%c%c%c%c%c%c%c%c|        |%c%c%c%c%c%c%c%c|        |%c%c%c%c%c%c%c%c|        |%c%c%c%c%c%c%c%c|\n", 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219);
		break;
	case 2:
		printf("|        |%c%c%c%c%c%c%c%c|        |%c%c%c%c%c%c%c%c|        |%c%c%c%c%c%c%c%c|        |%c%c%c%c%c%c%c%c|\n", 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219);
		printf("+-----------------------------------------------------------------------+\n");
		printf("|%c%c%c%c%c%c%c%c|        |%c%c%c%c%c%c%c%c|        |%c%c%c%c%c%c%c%c|        |%c%c%c%c%c%c%c%c|        |\n", 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219);
		break;
	case 3:
		printf("|        |%c%c%c%c%c%c%c%c|        |%c%c%c%c%c%c%c%c|        |%c%c%c%c%c%c%c%c|        |%c%c%c%c%c%c%c%c|\n", 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219, 219);
		printf("+-----------------------------------------------------------------------+\n");
		printf("     a        b        c        d        e        f        g        h\n\n");
		break;
	default:	break;
	}
}
void schachbrett_darstellung() {
	schachlinien(0);
	printf("|%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|    %c   |   8\n", 219, 219, 219, figuren[0][0], 219, 219, figuren[0][1], 219, 219, 219, figuren[0][2], 219, 219, figuren[0][3], 219, 219, 219, figuren[0][4], 219, 219, figuren[0][5], 219, 219, 219, figuren[0][6], 219, 219, figuren[0][7] );
	schachlinien(1);
	printf("|    %c   |%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|   7\n", figuren[1][0], 219, 219, 219, figuren[1][1], 219, 219, figuren[1][2], 219, 219, 219, figuren[1][3], 219, 219, figuren[1][4], 219, 219, 219, figuren[1][5], 219, 219, figuren[1][6], 219, 219, 219, figuren[1][7], 219, 219 );
	schachlinien(2);
	printf("|%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|    %c   |   6\n", 219, 219, 219, figuren[2][0], 219, 219, figuren[2][1], 219, 219, 219, figuren[2][2], 219, 219, figuren[2][3], 219, 219, 219, figuren[2][4], 219, 219, figuren[2][5], 219, 219, 219, figuren[2][6], 219, 219, figuren[2][7] );
	schachlinien(1);
	printf("|    %c   |%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|   5\n", figuren[3][0], 219, 219, 219, figuren[3][1], 219, 219, figuren[3][2], 219, 219, 219, figuren[3][3], 219, 219, figuren[3][4], 219, 219, 219, figuren[3][5], 219, 219, figuren[3][6], 219, 219, 219, figuren[3][7], 219, 219 );
	schachlinien(2);
	printf("|%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|    %c   |   4\n", 219, 219, 219, figuren[4][0], 219, 219, figuren[4][1], 219, 219, 219, figuren[4][2], 219, 219, figuren[4][3], 219, 219, 219, figuren[4][4], 219, 219, figuren[4][5], 219, 219, 219, figuren[4][6], 219, 219, figuren[4][7] );
	schachlinien(1);
	printf("|    %c   |%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|   3\n", figuren[5][0], 219, 219, 219, figuren[5][1], 219, 219, figuren[5][2], 219, 219, 219, figuren[5][3], 219, 219, figuren[5][4], 219, 219, 219, figuren[5][5], 219, 219, figuren[5][6], 219, 219, 219, figuren[5][7], 219, 219 );
	schachlinien(2);
	printf("|%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|    %c   |   2\n", 219, 219, 219, figuren[6][0], 219, 219, figuren[6][1], 219, 219, 219, figuren[6][2], 219, 219, figuren[6][3], 219, 219, 219, figuren[6][4], 219, 219, figuren[6][5], 219, 219, 219, figuren[6][6], 219, 219, figuren[6][7] );
	schachlinien(1);
	printf("|    %c   |%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|    %c   |%c%c%c %c %c%c|   1\n", figuren[7][0], 219, 219, 219, figuren[7][1], 219, 219, figuren[7][2], 219, 219, 219, figuren[7][3], 219, 219, figuren[7][4], 219, 219, 219, figuren[7][5], 219, 219, figuren[7][6], 219, 219, 219, figuren[7][7], 219, 219 );
	schachlinien(3);
}



/*** Prüfung des Zugs ob allgemeine Schachregeln eingehalten werden ***/
int pruefung_schachregeln(int x_alt, int y_alt, int x_neu, int y_neu) {
	char spielfigur = ' ';																		//Intialisierung der Variablen mit Leerzeichen
	char zielfeld = ' ';																		//Intialisierung der Variablen mit Leerzeichen
	int i = 1;																					//Laufvariable für Prüfung ob Figur das Springen versucht

	spielfigur = figuren[x_alt][y_alt];															//Variable spielfigur mit zubewegender Figur füllen für weitere Prüfungen
	zielfeld = figuren[x_neu][y_neu];															//Variable zielfeld mit Inhalt des Zielfeldes füllen für weitere Prüfungen

	/* Test ob Zielfeld von eigener Farbe besetzt wird */
	if ((sw % 2) == 0) {																		//If-Bedingung nur erfüllt wenn Weiß am Zug ist
		if (zielfeld == 'b' || zielfeld == 'd' || zielfeld == 's' || zielfeld == 'k' || zielfeld == 'l' || zielfeld == 't') {	//Wenn Zielfeld eigene Farbe ist wird If-Bedingung erfüllt
			return 0;																			//Zug wird abgebrochen da keine eigenen Figuren geschmissen werden dürfen
		}
	}
	if ((sw % 2) != 0) {																		//If-Bedingung nur erfüllt wenn Schwarz am Zug ist
		if (zielfeld == 'B' || zielfeld == 'D' || zielfeld == 'S' || zielfeld == 'K' || zielfeld == 'L' || zielfeld == 'T') {	//Wenn Zielfeld eigene Farbe ist wird If-Bedingung erfüllt
			return 0;																			//Zug wird abgebrochen da keine eigenen Figuren geschmissen werden dürfen
		}
	}

	/* Bauer */
	if (spielfigur == 'b' || spielfigur == 'B') {
		if ((sw % 2) == 0) {																	//Weiß am Zug
			if (y_neu == y_alt && ((x_neu)-(x_alt)) == -1 && zielfeld == ' ') {					//Bauer 1 Feld gerade aus, rückwärts nicht erlaubt
				return 1;
			}
		}
		if ((sw % 2) != 0) {																	//Schwarz am Zug
			if (y_neu == y_alt && ((x_neu)-(x_alt)) == 1 && zielfeld == ' ') {					//Bauer 1 Feld gerade aus, rückwärts nicht erlaubt
				return 1;
			}
		}
		if ((sw % 2) == 0) {																	//Weiß ist am Zug
			if (abs((y_neu)-(y_alt)) == 1 && ((x_neu)-(x_alt)) == -1 && zielfeld != ' ') {		//Bauer wirft schräg eine andere Figur, rückwärts nicht erlaubt
				return 1;
			}
		}
		if ((sw % 2) != 0) {																	//Schwarz ist am Zug
			if (abs((y_neu)-(y_alt)) == 1 && ((x_neu)-(x_alt)) == 1 && zielfeld != ' ') {		//Bauer wirft schräg eine andere Figur, rückwärts nicht erlaubt
				return 1;
			}
		}
		if ((sw % 2) == 0 && x_alt == 6 && abs((x_alt)-(x_neu)) == 2 && zielfeld == ' ') {		//Bauer (Weiß) geht von Startfeld 2 Felder gerade aus
			for (((x_alt)-i); ((x_alt)-i) > x_neu; i++) {
				if (figuren[(x_alt)-i][y_alt] != ' ') {											//Bauer darf keine anderen Figuren überspringen
					return 0;
				}
			}
			return 1;
		}
		if ((sw % 2) != 0 && x_alt == 1 && abs((x_alt)-(x_neu)) == 2 && zielfeld == ' ') {		//Bauer (Schwarz) geht von Startfeld 2 Felder gerade aus
			for (((x_alt)+i); ((x_alt)+i) < x_neu; i++) {
				if (figuren[(x_alt)+i][y_alt] != ' ') {											//Bauer darf keine anderen Figuren überspringen
					return 0;
				}
			}
			return 1;
		}
		return 0;																				//Rückgabewert 0 falls Zug nicht erlaubt
	}

	/*  Springer */
	if (spielfigur == 's' || spielfigur == 'S') {
		if (abs((x_neu)-(x_alt)) == 2 && abs((y_neu)-(y_alt)) == 1 || abs((x_neu)-(x_alt)) == 1 && abs((y_neu)-(y_alt)) == 2) {
			return 1;																			// 1. Bed 2 nach vorne 1 nach rechts oder links
		}																						// 2. Bed 1 nach vorne 2 nach rechts oder links
		return 0;
	}

	/* Turm */
	if (spielfigur == 't' || spielfigur == 'T') {
		/* Horizontale Bewegung */
		if (x_neu == x_alt) {
			/* Nach Rechts */
			if (y_neu > y_alt) {
				for (((y_alt)+i); ((y_alt)+i) < y_neu; i++) {									//Der Weg zwischen Start- und Zielfeld wird entlang gegangen
					if (figuren[(x_alt)][(y_alt)+i] != ' ') {									//Prüfung ob auf den Feldern zwischen Ziel- und Startfeld eine Figur übersprungen wird
						return 0;																//Zug ist ungültig falls Übersprungen wird
					}
				}
				return 1;																		//Zug ist gültig
			}
			/* Nach Links */
			if (y_neu < y_alt) {
				for (((y_alt)-i); ((y_alt)-i) > y_neu; i++) {									//Der Weg zwischen Start- und Zielfeld wird entlang gegangen
					if (figuren[(x_alt)][(y_alt)-i] != ' ') {									//Prüfung ob auf den Feldern zwischen Ziel- und Startfeld eine Figur übersprungen wird
						return 0;																//Zug ist ungültig falls übersprungen wird
					}
				}
				return 1;																		//Zug ist gültig
			}
		}
		/* Vertikale Bewegungen */
		if (y_neu == y_alt) {
			/* Nach Unten */
			if (x_neu > x_alt) {
				for (((x_alt)+i); ((x_alt)+i) < x_neu; i++) {									//Der Weg zwischen Start- und Zielfeld wird entlang gegangen
					if (figuren[(x_alt)+i][(y_alt)] != ' ') {									//Prüfung ob auf den Feldern zwischen Ziel- und Startfeld eine Figur übersprungen wird
						return 0;																//Zug ist ungültig falls übersprungen wird
					}
				}
				return 1;																		//Zug ist gültig
			}
			/* Nach Oben */
			if (x_neu < x_alt) {
				for (((x_alt)-i); ((x_alt)-i) > x_neu; i++) {									//Der Weg zwischen Start- und Zielfeld wird entlang gegangen
					if (figuren[(x_alt)-i][(y_alt)] != ' ') {									//Prüfung ob auf den Feldern zwischen Ziel- und Startfeld eine Figur übersprungen wird
						return 0;																//Zug ist ungültig falls übersprungen wird
					}
				}
				return 1;																		//Zug ist gültig
			}
		}
		return 0;
 	}

	/* Läufer */
	if (spielfigur == 'l' || spielfigur == 'L') {
		/* Diagonale Bewegung */
		if (abs((y_neu)-(y_alt)) == abs((x_neu)-(x_alt))) {
			/* Nach Unten */
			if ((x_neu) > (x_alt)) {
				/* Rechts Unten */
				if ((y_neu) > (y_alt)) {
					for (((x_alt)+i); ((x_alt)+i) < x_neu; i++) {								//Weg nach Rechts Unten entlang gehen
						if (figuren[(x_alt+i)][(y_alt)+i] != ' ') {								//Wurde auf dem Weg Zum Zielfeld eine Figur übersprungen?
							return 0;															//Zug war ungültig
						}
					}
					return 1;																	//Zug war gültig
				}
				/* Links Unten */
				if ((y_neu) < (y_alt)) {
					for (((x_alt)+i); ((x_alt)+i) < x_neu; i++) {								//Weg nach Links Unten entlang gehen
						if (figuren[(x_alt)+i][(y_alt)-i] != ' ') {								//Wurde auf dem Weg Zum Zielfeld eine Figur übersprungen?
							return 0;															//Zug war ungültig
						}
					}
					return 1;																	//Zug war gültig
				}
			}


			/* Nach Oben */
			if ((x_neu) < (x_alt)) {
				/* Rechts Oben */
				if (y_neu > y_alt) {
					for (((x_alt)-i); ((x_alt)-i) > x_neu; i++) {								//Weg nach Rechts oben entlang gehen
						if (figuren[(x_alt)-i][(y_alt)+i] != ' ') {								//Wurde auf dem Weg Zum Zielfeld eine Figur übersprungen?
							return 0;															//Zug ist ungültig
						}
					}
					return 1;																	//Zug ist gültig
				}
				/* Links Oben */
				if (y_neu < y_alt) {
					for (((x_alt)-i); ((x_alt)-i) > x_neu; i++) {								//Weg nach Links oben entlang gehen
						if (figuren[(x_alt)-i][(y_alt-i)] != ' ') {								//Wurde auf dem Weg Zum Zielfeld eine Figur übersprungen?
							return 0;															//Zug ist ungültig
						}
					}
					return 1;																	//Zug ist gültig
				}

			}
		}
	}

	/* König */
	if (spielfigur == 'k' || spielfigur == 'K') {
		if (abs((y_neu)-(y_alt)) <= 1 && abs((x_neu)-(x_alt)) <= 1) {							//König darf sich nur ein Feld um sich herum, dafür in alle Richtungen bewegen
			return 1;;																			
		}
		return 0;
	}

	/* Dame */
	if (spielfigur == 'd' || spielfigur == 'D') {
		/* Horizontale und Veritkale Bewegungen */
		/* Horizontale Bewegung */
		if (x_neu == x_alt) {
			/* Nach Rechts */
			if (y_neu > y_alt) {
				for (((y_alt)+i); ((y_alt)+i) < y_neu; i++) {									//Der Weg zwischen Start- und Zielfeld wird entlang gegangen
					if (figuren[(x_alt)][(y_alt)+i] != ' ') {									//Prüfung ob auf den Feldern zwischen Ziel- und Startfeld eine Figur übersprungen wird
						return 0;																//Zug ist ungültig falls Übersprungen wird
					}
				}
				return 1;																		//Zug ist gültig
			}
			/* Nach Links */
			if (y_neu < y_alt) {
				for (((y_alt)-i); ((y_alt)-i) > y_neu; i++) {									//Der Weg zwischen Start- und Zielfeld wird entlang gegangen
					if (figuren[(x_alt)][(y_alt)-i] != ' ') {									//Prüfung ob auf den Feldern zwischen Ziel- und Startfeld eine Figur übersprungen wird
						return 0;																//Zug ist ungültig falls übersprungen wird
					}
				}
				return 1;																		//Zug ist gültig
			}
		}
		/* Vertikale Bewegungen */
		if (y_neu == y_alt) {
			/* Nach Unten */
			if (x_neu > x_alt) {
				for (((x_alt)+i); ((x_alt)+i) < x_neu; i++) {									//Der Weg zwischen Start- und Zielfeld wird entlang gegangen
					if (figuren[(x_alt)+i][(y_alt)] != ' ') {									//Prüfung ob auf den Feldern zwischen Ziel- und Startfeld eine Figur übersprungen wird
						return 0;																//Zug ist ungültig falls übersprungen wird
					}
				}
				return 1;																		//Zug ist gültig
			}
			/* Nach Oben */
			if (x_neu < x_alt) {
				for (((x_alt)-i); ((x_alt)-i) > x_neu; i++) {									//Der Weg zwischen Start- und Zielfeld wird entlang gegangen
					if (figuren[(x_alt)-i][(y_alt)] != ' ') {									//Prüfung ob auf den Feldern zwischen Ziel- und Startfeld eine Figur übersprungen wird
						return 0;																//Zug ist ungültig falls übersprungen wird
					}
				}
				return 1;																		//Zug ist gültig
			}
		}

		/* Diagonale Bewegungen */
		if (abs((y_neu)-(y_alt)) == abs((x_neu)-(x_alt))) {
			/* Nach Unten */
			if ((x_neu) > (x_alt)) {
				/* Rechts Unten */
				if ((y_neu) > (y_alt)) {
					for (((x_alt)+i); ((x_alt)+i) < x_neu; i++) {								//Weg nach Rechts Unten entlang gehen
						if (figuren[(x_alt + i)][(y_alt)+i] != ' ') {							//Wurde auf dem Weg Zum Zielfeld eine Figur übersprungen?
							return 0;															//Zug war ungültig
						}
					}
					return 1;																	//Zug war gültig
				}
				/* Links Unten */
				if ((y_neu) < (y_alt)) {
					for (((x_alt)+i); ((x_alt)+i) < x_neu; i++) {								//Weg nach Links Unten entlang gehen
						if (figuren[(x_alt)+i][(y_alt)-i] != ' ') {								//Wurde auf dem Weg Zum Zielfeld eine Figur übersprungen?
							return 0;															//Zug war ungültig
						}
					}
					return 1;																	//Zug war gültig
				}
			}


			/* Nach Oben */
			if ((x_neu) < (x_alt)) {
				/* Rechts Oben */
				if (y_neu > y_alt) {
					for (((x_alt)-i); ((x_alt)-i) > x_neu; i++) {								//Weg nach Rechts oben entlang gehen
						if (figuren[(x_alt)-i][(y_alt)+i] != ' ') {								//Wurde auf dem Weg Zum Zielfeld eine Figur übersprungen?
							return 0;															//Zug ist ungültig
						}
					}
					return 1;																	//Zug ist gültig
				}
				/* Links Oben */
				if (y_neu < y_alt) {
					for (((x_alt)-i); ((x_alt)-i) > x_neu; i++) {								//Weg nach Links oben entlang gehen
						if (figuren[(x_alt)-i][(y_alt - i)] != ' ') {								//Wurde auf dem Weg Zum Zielfeld eine Figur übersprungen?
							return 0;															//Zug ist ungültig
						}
					}
					return 1;																	//Zug ist gültig
				}

			}
		}
		return 0;
	}
}



/* Darstellung Siegerehrung */
void outro() {
	system("cls");
	printf("*------------------------------------------------------------------------------*\n");
	printf("*------------------------------------------------------------------------------*\n");
	printf_leerzeile(2);
	if ((sw % 2) == 0) {
		printf("\t\t   W E I S S   H A T   G E W O N N E N !\n");
	}
	else {
		printf("\t\t   S C H W A R Z   H A T   G E W O N N E N !\n");
	}
	printf_leerzeile(7);
	printf("\t    .::.\n");
	printf("\t    _::_\n");
	printf("\t  _/____\\_        ()\n");
	printf("\t  \\      /      <~~~~>\n");
	printf("\t  \\     /       <~~~~>\n");
	printf("\t   \\____/        \\__/         <>_\n");
	printf("\t   (____)       (____)      (\\)  )                        __/'''\\\n");
	printf("\t    |  |         |  |        \\__/      WWWWWW            ]___ 0  }\n");
	printf("\t    |__|         |  |       (____)      |  |       __        /   }\n");
	printf("\t   /    \\        |__|        |  |       |  |      (  )     /~    }\n");
	printf("\t  (______)      /____\\       |__|       |__|       ||      \\____/\n");
	printf("\t (________)    (______)     /____\\     /____\\     /__\\     /____\\\n");
	printf("\t /________\\   (________)   (______)   (______)   (____)   (______)\n\n\n");
	Sleep(3000);
	exit(0);
}



/*** Schachfigur bewegen, via zug-Array ***/
void figur_bewegen(char zug_move[5], char figuren_move[8][8]) {
	int alt_x = 7, alt_y = 0, neu_x = 6, neu_y = 0;
	int p;

	// switch-Anweisung für die Umwandlung der DAU-Eingabe in Arrayform (0-7 und nicht a/1-h/8)
	switch (zug_move[0])
	{
		case 'a': alt_y = 0; break;
		case 'b': alt_y = 1; break;
		case 'c': alt_y = 2; break;
		case 'd': alt_y = 3; break;
		case 'e': alt_y = 4; break;
		case 'f': alt_y = 5; break;
		case 'g': alt_y = 6; break;
		case 'h': alt_y = 7; break;
	}
	switch (zug_move[1])
	{
		case '1': alt_x = 7; break;
		case '2': alt_x = 6; break;
		case '3': alt_x = 5; break;
		case '4': alt_x = 4; break;
		case '5': alt_x = 3; break;
		case '6': alt_x = 2; break;
		case '7': alt_x = 1; break;
		case '8': alt_x = 0; break;
	}
	switch (zug_move[2])
	{
		case 'a': neu_y = 0; break;
		case 'b': neu_y = 1; break;
		case 'c': neu_y = 2; break;
		case 'd': neu_y = 3; break;
		case 'e': neu_y = 4; break;
		case 'f': neu_y = 5; break;
		case 'g': neu_y = 6; break;
		case 'h': neu_y = 7; break;
	}
	switch (zug_move[3])
	{
		case '1': neu_x = 7; break;
		case '2': neu_x = 6; break;
		case '3': neu_x = 5; break;
		case '4': neu_x = 4; break;
		case '5': neu_x = 3; break;
		case '6': neu_x = 2; break;
		case '7': neu_x = 1; break;
		case '8': neu_x = 0; break;
	}

	sw--;										//Zähler muss vor Prüfung um eins reduziert werden und IST-Stand zu bekommen, da vorher schon um 1 erhöht
	if ((sw % 2) == 0) {						//Falls Weiß am Zug ist wird die IF-Anweisung betreten
		switch (figuren_move[alt_x][alt_y]) {	//Falls die bewegende Figur nicht weiß sondern schwarz ist,
		case 'B': sw = 2; return;				//wird die komplette Funktion beendet damit der weiße Spieler eine richtige Eingabe tätigen kann
		case 'L': sw = 2; return;				//Der Zähler wird erneut angepasst, damit erneut der weiße Spieler eine Eingabe tätigen kann
		case 'T': sw = 2; return;
		case 'K': sw = 2; return;
		case 'D': sw = 2; return;
		case 'S': sw = 2; return;
		case ' ': sw = 2; return;
		default: break;
		}
	}
	else if ((sw % 2) != 0) {					//Falls Schwarz am Zug ist wird die IF-Anweisung betreten
		switch (figuren_move[alt_x][alt_y]) {	//Falls die bewegende Figur nicht schwarz sondern weiß ist,
		case 'b': sw = 3; return;				//wird die komplette Funktion beendet damit der schwarze Spieler eine richtige Eingabe tätigen kann
		case 'l': sw = 3; return;				//Der Zähler wird erneut angepasst, damit erneut der schwarze Spieler eine Eingabe tätigen kann
		case 't': sw = 3; return;
		case 'k': sw = 3; return;
		case 'd': sw = 3; return;
		case 's': sw = 3; return;
		case ' ': sw = 3; return;
		default: break;
		}
	}
			

	p = pruefung_schachregeln(alt_x, alt_y, neu_x, neu_y);			//Prüfung des Zugs auf allgemeine Schachregeln. Übergabe von Quellen- und Zielkoordinate auf dem Schachbrett
	if (p == 0) {													//Rückgabewert ist 0
		return;														//Zug NICHT ERLAUBT, somit wird Eingabe wiederholt in dem die Funktion figur_bewegen abbricht
	}

	if (figuren[neu_x][neu_y] == 'k' || figuren[neu_x][neu_y] == 'K') {		//Prüfung Auf Niederlage des Königs
		outro();
	}
																	//Falls der Zug richtig war wird ganz normal gezogen
	figuren_move[neu_x][neu_y] = figuren_move[alt_x][alt_y];		//Zielfeld wird mit Ausgangsfeld gefüllt
	figuren_move[alt_x][alt_y] = ' ';								//Figur wird von Ausgangsfeld gelöscht
	stack_undo(1);													//PUSH: Stack in der Undo-Funktion wird um neuen Zug ergänzt
	sw++;
}



/*** Nächsten Zug definieren ***/
char naechster_zug(char zug_neu[]) {
	fseek(stdin, 0, SEEK_END);
	gets(zug_neu);
	fseek(stdin, 0, SEEK_END);
	if (strcmp(zug_neu,"EXIT") == 0) {								//Bei Eingabe von EXIT wird in die Funktion
		spiel_speichern_und_beenden();								//spiel_speichern_und_beenden gewechselt
		return;
	}
	if (strcmp(zug_neu, "undo") == 0) {
		stack_undo(2);												//stack_undo mit 2 aufgerufen führt dazu dass der letzte Zug rückgängig gemacht wird
		return 1;													//Falls UNDO gewählt wurde soll in MAIN keine figuren_bewegen aufgerufen werden
	}
	while (zug_neu[0] < 'a' || zug_neu[0] > 'h') {					//Zug liegt außerhalb des Schachfelds
		{
			gotoXY(10, 35);
			printf_leerzeile(7);
			gotoXY(10, 35);
		}
		printf("\nEingabe war Fehlerhaft!\nBitte in folgenden Format eingeben: z. B. a1b2 (Altes Feld | Neues Feld).\n");
		printf("Um den letzten rueckgaenig zu machen geben Sie bitte 'undo' ein.\n");
		printf("Um das Spiel zu beenden und zu speichern, geben Sie bitte 'EXIT' ein.\n");
		printf(">>> naechster Zug: ");
		gets(zug_neu);
		if (strcmp(zug_neu, "EXIT") == 0) {
			spiel_speichern_und_beenden();
		}
	}
	while (zug_neu[1] < '1' || zug_neu[1] > '8') {					//Zug liegt außerhalb des Schachfelds
		{
			gotoXY(10, 36);
			printf_leerzeile(7);
			gotoXY(10, 36);
		}
		printf("\nEingabe war Fehlerhaft!\nBitte in folgenden Format eingeben: z. B. a1b2 (Altes Feld | Neues Feld).\n");
		printf("Um den letzten Zug rueckgaenig zu machen geben Sie bitte 'undo' ein.\n");
		printf("Um das Spiel zu beenden und zu speichern, geben Sie bitte 'EXIT' ein.\n");
		printf(">>> naechster Zug: ");
		gets(zug_neu);
		if (strcmp(zug_neu, "EXIT") == 0) {
			spiel_speichern_und_beenden();
		}
	}
	while (zug_neu[2] < 'a' || zug_neu[2] > 'h') {					//Zug liegt außerhalb des Schachfelds
		{
			gotoXY(10, 36);
			printf_leerzeile(7);
			gotoXY(10, 36);
		}
		printf("\nEingabe war Fehlerhaft!\nBitte in folgenden Format eingeben: z. B. a1b2 (Altes Feld | Neues Feld).\n");
		printf("Um den letzten Zug rueckgaenig zu machen geben Sie bitte 'undo' ein.\n");
		printf("Um das Spiel zu beenden und zu speichern, geben Sie bitte 'EXIT' ein.\n");
		printf(">>> naechster Zug: ");
		gets(zug_neu);
		if (strcmp(zug_neu, "EXIT") == 0) {
			spiel_speichern_und_beenden();
		}
	}
	while (zug_neu[3] < '1' || zug_neu[3] > '8') {					//Zug liegt außerhalb des Schachfelds
		{
			gotoXY(10, 36);
			printf_leerzeile(7);
			gotoXY(10, 36);
		}
		printf("\nEingabe war Fehlerhaft!\nBitte in folgenden Format eingeben: z. B. a1b2 (Altes Feld | Neues Feld).\n");
		printf("Um den letzten Zug rueckgaenig zu machen geben Sie bitte 'undo' ein.\n");
		printf("Um das Spiel zu beenden und zu speichern, geben Sie bitte 'EXIT' ein.\n");
		printf(">>> naechster Zug: ");
		gets(zug_neu);
		if (strcmp(zug_neu, "EXIT") == 0) {
			spiel_speichern_und_beenden();
		}
	}
	while (zug_neu[4] != '\0') {									//Die Eingabe übersteigt die Länge der 4 einzugebenen Zeichen
		{
			gotoXY(10, 36);
			printf_leerzeile(7);
			gotoXY(10, 36);
		}
		printf("\nEingabe war Fehlerhaft!\nBitte in folgenden Format eingeben: z. B. a1b2 (Altes Feld | Neues Feld).\n");
		printf("Um den letzten Zug rueckgaenig zu machen geben Sie bitte 'undo' ein.\n");
		printf("Um das Spiel zu beenden und zu speichern, geben Sie bitte 'EXIT' ein.\n");
		printf(">>> naechster Zug: ");
		gets(zug_neu);
		if (strcmp(zug_neu, "EXIT") == 0) {
			spiel_speichern_und_beenden();
		}
	}
	zug_neu[4] = '\0';												//5. Arrayfeld wird als Stringende definiert
	return 0;

}



/*** Darstellung Startseite ***/
void intro() {
	int a;
	printf("*------------------------------------------------------------------------------*\n");
	printf("*------------------------------------------------------------------------------*");
	printf_leerzeile(3);
	printf("\t\t\t      ________\n");
	printf("\t\t\t     / ____/ /_  ___  __________\n");
	printf("\t\t\t    / /   / __ \\/ _ \\/ ___/ ___/\n");
	printf("\t\t\t   / /___/ / / /  __(__  |__  )\n");
	printf("\t\t\t   \\____/_/ /_/\\___/____/____/");
	printf_leerzeile(2);
	printf("\t                 - - - P R E S S   E N T E R - - -\n\n\n");
	printf("\t    .::.\n");
	printf("\t    _::_\n");
	printf("\t  _/____\\_        ()\n");
	printf("\t  \\      /      <~~~~>\n");
	printf("\t  \\     /       <~~~~>\n");
	printf("\t   \\____/        \\__/         <>_\n");
	printf("\t   (____)       (____)      (\\)  )                        __/'''\\\n");
	printf("\t    |  |         |  |        \\__/      WWWWWW            ]___ 0  }\n");
	printf("\t    |__|         |  |       (____)      |  |       __        /   }\n");
	printf("\t   /    \\        |__|        |  |       |  |      (  )     /~    }\n");
	printf("\t  (______)      /____\\       |__|       |__|       ||      \\____/\n");
	printf("\t (________)    (______)     /____\\     /____\\     /__\\     /____\\\n");
	printf("\t /________\\   (________)   (______)   (______)   (____)   (______)\n");

	printf("\n  (V1.0)    Fuer beste Spielerfahrung in WIN10 oeffnen und Fenster maximieren\n");
	a = getchar();
	system("cls");
}



main() {
	char zug[80];
	int a;

	intro();
	stack_undo(0);									//Pos. 0 im UNDO-Stack mit Standardaufstellung (ggf. auch geladene Aufstellung) intialisieren
	startmenu();									//Darstellung Auswahlmenü
	schachbrett_darstellung();						//Schachbrettdarstellung mit gewählter Füllung (NEU / GELADENES SPIEL)
	while (1) {
		printf("Um den letzten Zugrueckgaengig zu machen geben Sie bitte 'undo' ein.\n");
		printf("Um das Spiel zu beenden und zu speichern, geben Sie bitte 'EXIT' ein.\n");
		if ((sw % 2) == 0) {
			printf("Weiss ist am Zug! Nur Zuege der richtigen Farbe werden angenommen.\n");
			sw++;
		}
		else {
			printf("Schwarz ist am Zug! Nur Zuege der richtigen Farbe werden angenommen.\n");
			sw++;
		}
		printf(">>> naechster Zug: ");
		int a = naechster_zug(zug);					//Rückgabewert bestimmt ob normaler Zug und somit auch figuren_bewegen aufgerufen werden soll
		if (a == 0) {								//Rückgabewert ist 0 und somit besteht ein normaler Zug und die Funktion figuren_bewegen WIRD aufgerufen
			figur_bewegen(zug, figuren);
		}
		if (a == 1) {								//Rückgabewert ist 1 und somit besteht kein normaler Zug und die Funktion figuren_bewegen WIRD NICHT aufgerufen
			;
		}
		system("cls");
		schachbrett_darstellung();
	}
}