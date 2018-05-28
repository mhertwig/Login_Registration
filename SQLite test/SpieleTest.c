/********************************************************************************
| Programname: User Login
| Autor: Marwin Hertwig
| Datum: 05.04.2018
| Compiler: VS 15
| Beschreibung: User Login Menü mit Login bzw Registrierungs Funktion
|
*********************************************************************************/

/*
================================================
Praeprozessoranweisungen
================================================
*/

#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_SECURE_NO_WARNINGS
#define MAX 100

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#define DATABASE_FILE "spiele.sqlite3"
/*
================================================
Funktionsprototypen
================================================
*/

int login_user();
int register_user(char *spielername, char *passwort, char *nachname, char *vorname);
void scan(char *spielername, char *passwort, char *nachname, char *vorname);


void main(void) {
	int schleife = 1;
	while (schleife == 1) {
		char spielername[MAX];
		char passwort[MAX];
		char nachname[MAX];
		char vorname[MAX];

		int menu = 0;
		printf("1:		Einloggen\n2:		Benutzer anlegen\n3:		Beenden\n");
		fflush(stdin);
		scanf("%i", &menu);
		switch (menu) {
		case 1:
			login_user();
			schleife = 0;
			break;
		case 2:
			register_user(spielername, passwort, nachname, vorname);
			break;
		case 3:
			schleife = 0;
			break;
		}
	}
	system("pause");
}

/*
================================================
Funktionen
================================================
*/

int register_user(char *spielername, char *passwort, char *nachname, char *vorname) {

	char *sql;
	char *zErrMsg;
	sqlite3 *db_handle;
	int rc;

	scan(spielername, passwort, nachname, vorname);

	rc = sqlite3_open(DATABASE_FILE, &db_handle);

	sql = sqlite3_mprintf("INSERT INTO benutzer VALUES (NULL, %Q, %Q, %Q, %Q, \
				date('now'));", spielername, passwort, nachname, vorname);

	if (rc == SQLITE_OK)
	{
		rc = sqlite3_exec(db_handle, sql, NULL, NULL, &zErrMsg);
		if (rc == SQLITE_OK)
		{
			sqlite3_close(db_handle);
			printf("User erfolgreich erstellt!\n\n\n\n\n");
			return 0;
		}
		else {
			printf("SQL Fehler: %s\n", zErrMsg);
			sqlite3_free(zErrMsg);
			sqlite3_close(db_handle);
			system("Pause");
			exit(-1);
		}
	}
}

void scan(char *spielername, char *passwort, char *nachname, char *vorname) {

	printf("Spielername:");
	scanf("%s", spielername);

	printf("Passwort:");
	scanf("%s", passwort);

	/*
	================================================
	Encrypt (Passwort wird verschlüsselt und gespeichert)
	================================================
	*/
	char keyToEncrypt = '1084';
	int pwLaenge = strlen(passwort);

	for (int temp = 0; temp < pwLaenge; temp++) {
		passwort[temp] ^= keyToEncrypt;
	}



	printf("Nachname:");
	scanf("%s", nachname);

	printf("Vorname:");
	scanf("%s", vorname);
}

int login_user() {
	char spielername[MAX];
	char passwort[MAX];

	char *sql;
	char *zErrMsg;
	sqlite3 *db_handle;
	int rc;
	sqlite3_stmt *stmt;
	int col;
	int *data;

	int flag = 0;
	do {
		flag = 0;
		printf("Spielername:");
		fflush(stdin);
		scanf("%s", spielername);

		printf("Passwort:");
		fflush(stdin);
		scanf("%s", passwort);

		/*
		================================================
		Decrypt (Passwort wird entschlüsselt)
		================================================
		*/
		char keyToEncrypt = '1084';
		int pwLaenge = strlen(passwort);

		for (int temp = 0; temp < pwLaenge; temp++) {
			passwort[temp] ^= keyToEncrypt;
		}



		sql = sqlite3_mprintf("SELECT spielername FROM benutzer WHERE spielername = '%s'", spielername);

		rc = sqlite3_open(DATABASE_FILE, &db_handle);

		rc = sqlite3_prepare_v2(db_handle, sql, strlen(sql), &stmt, NULL);

		col = sqlite3_column_count(stmt);

		if (sqlite3_step(stmt) != SQLITE_ROW) {
			flag = -1;
			sqlite3_close(db_handle);
		}

		sql = sqlite3_mprintf("SELECT passwort FROM benutzer WHERE spielername = '%s' AND passwort = '%s'", spielername, passwort);

		rc = sqlite3_open(DATABASE_FILE, &db_handle);

		rc = sqlite3_prepare_v2(db_handle, sql, strlen(sql), &stmt, NULL);

		col = sqlite3_column_count(stmt);

		if (flag != -1 && sqlite3_step(stmt) != SQLITE_ROW) {
			flag = -2;
			sqlite3_close(db_handle);
		}

		if (flag == 0) {
			sql = sqlite3_mprintf("SELECT user_id,spielername FROM benutzer WHERE spielername = '%s'", spielername);

			rc = sqlite3_open(DATABASE_FILE, &db_handle);

			rc = sqlite3_prepare_v2(db_handle, sql, strlen(sql), &stmt, NULL);

			col = sqlite3_column_count(stmt);

			while (sqlite3_step(stmt) == SQLITE_ROW) {
				int data = sqlite3_column_int(stmt, 0);
				int flag = data;
			}
		}
		if (flag < 0) {
			if (flag == -1) {
				printf("Falscher Benutzername!\n");
			}
			if (flag == -2) {
				printf("Falsches Passwort!\n");
			}

		}

	} while (flag < 0);
	printf("Erfolgreich eingeloggt!\n");
	return flag;
}