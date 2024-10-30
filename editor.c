#ifdef _WIN32
    #include <pdcurses.h>  // PDCurses verwenden, wenn auf Windows kompiliert
#else
    #include <ncurses.h>   // Ncurses verwenden, wenn auf Unix/Linux kompiliert
#endif
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CTRL_KEY(k) ((k) & 0x1f)

void init_ncurses() {
    initscr();              // Startet den ncurses-Modus
    raw();                  // Direktes Erfassen der Eingaben (kein Zeilenpuffer)
    keypad(stdscr, TRUE);   // Ermöglicht die Nutzung von Funktionstasten wie den Pfeiltasten
    noecho();               // Verhindert, dass Eingaben direkt ausgegeben werden
    curs_set(1);            // Cursor sichtbar machen
}

void shutdown_ncurses() {
    endwin();               // Beendet den ncurses-Modus
}

void load_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        mvprintw(1, 0, "Fehler beim Öffnen der Datei %s.", filename);
        return;
    }

    int y = 1;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        mvprintw(y++, 0, "%s", line);
    }

    fclose(file);
}

void insert_char(int y, int x, char ch) {
    int max_x = getmaxx(stdscr);
    char line[max_x - x];

    // Speichere den Rest der Zeile ab der aktuellen Position
    for (int i = 0; i < max_x - x - 1; ++i) {
        line[i] = mvinch(y, x + i) & A_CHARTEXT;
    }
    line[max_x - x - 1] = '\0';

    // Setze das neue Zeichen an der aktuellen Position
    mvaddch(y, x, ch);

    // Schreibe den Rest der Zeile wieder zurück, um den Text nach rechts zu verschieben
    mvprintw(y, x + 1, "%s", line);
    move(y, x + 1);
}

void editor_loop() {
    int ch;
    while ((ch = getch()) != 27) { // Beenden mit ESC (ASCII 27)
        int y = getcury(stdscr);
        int x = getcurx(stdscr);
        switch (ch) {
            case KEY_UP:
                // Cursor nach oben bewegen
                move(y - 1, x);
                break;
            case KEY_DOWN:
                // Cursor nach unten bewegen
                move(y + 1, x);
                break;
            case KEY_LEFT:
                // Cursor nach links bewegen
                move(y, x - 1);
                break;
            case KEY_RIGHT:
                // Cursor nach rechts bewegen
                move(y, x + 1);
                break;
            case KEY_BACKSPACE:
            case 127:
                // Zeichen löschen (Rückschritt)
                move(y, x - 1);
                delch();
                break;
            case KEY_DC:
                // Zeichen löschen (Entfernen)
                delch();
                break;
            default:
                // Zeichen einfügen und Rest der Zeile nach rechts verschieben
                insert_char(y, x, ch);
                break;
        }
        refresh();
    }
}

int main() {
    init_ncurses();

    mvprintw(0, 0, "Willkommen zu deinem Texteditor! Drücke ESC zum Beenden.");
    load_file("test.txt");
    refresh();

    editor_loop();

    shutdown_ncurses();
    return 0;
}
