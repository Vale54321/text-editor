#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

#define CTRL_KEY(k) ((k) & 0x1f)

struct termios orig_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

void enable_raw_mode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG);
    raw.c_iflag &= ~(IXON);

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void load_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Fehler beim Öffnen der Datei %s.\n", filename);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);
}

void editor_loop() {
    int ch;
    while (1) {
        ch = getchar();
        if (ch == '\x1b') {
            char seq[3];
            if (read(STDIN_FILENO, &seq[0], 1) != 1) continue;
            if (read(STDIN_FILENO, &seq[1], 1) != 1) continue;

            if (seq[0] == '[') {
                switch (seq[1]) {
                    case 'A':
                        // Cursor nach oben bewegen (nicht implementiert)
                        printf("\033[A");
                        break;
                    case 'B':
                        // Cursor nach unten bewegen (nicht implementiert)
                        printf("\033[B");
                        break;
                    case 'C':
                        // Cursor nach rechts bewegen
                        printf("\033[C");
                        break;
                    case 'D':
                        // Cursor nach links bewegen
                        printf("\033[D");
                        break;
                }
            }
        } else if (ch == 27) {
            // ESC-Taste zum Beenden
            break;
        } else {
            switch (ch) {
                case 127:
                    // Zeichen löschen (Rückschritt)
                    printf("\b \b");
                    break;
                default:
                    // Zeichen anzeigen
                    putchar(ch);
                    break;
            }
        }
        fflush(stdout);
    }
}

int main() {
    enable_raw_mode();

    printf("Willkommen zu deinem Texteditor! Drücke ESC zum Beenden.\n");
    load_file("test.txt");

    editor_loop();

    return 0;
}
