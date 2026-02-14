#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define WIDTH  30
#define HEIGHT 25
#define BOARD_SIZE 4
#define TILE_SIZE 6  // For nice display

int board[BOARD_SIZE][BOARD_SIZE] = {0};
int score = 0;
int game_over = 0;
WINDOW *game_win;

void init_board() {
    memset(board, 0, sizeof(board));
    score = 0;
    game_over = 0;
    // Add two random 2's
    srand(time(NULL));
    for (int i = 0; i < 2; i++) {
        int x = rand() % BOARD_SIZE;
        int y = rand() % BOARD_SIZE;
        if (board[y][x] == 0) {
            board[y][x] = 2;
        } else {
            i--;  // Retry
        }
    }
}

void add_random_tile() {
    int empty_count = 0;
    int empties[16][2];  // Max 16 empties

    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board[y][x] == 0) {
                empties[empty_count][0] = x;
                empties[empty_count][1] = y;
                empty_count++;
            }
        }
    }

    if (empty_count > 0) {
        int idx = rand() % empty_count;
        int x = empties[idx][0];
        int y = empties[idx][1];
        board[y][x] = (rand() % 10 < 9) ? 2 : 4;  // 90% 2, 10% 4
    }
}

int can_move() {
    // Check if any merge or move possible
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board[y][x] == 0) return 1;
            if (x < BOARD_SIZE - 1 && board[y][x] == board[y][x+1]) return 1;
            if (y < BOARD_SIZE - 1 && board[y][x] == board[y+1][x]) return 1;
        }
    }
    return 0;
}

void move_left() {
    int moved = 0;
    for (int y = 0; y < BOARD_SIZE; y++) {
        // Slide left
        int col[BOARD_SIZE];
        int k = 0;
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board[y][x] != 0) col[k++] = board[y][x];
        }
        for (; k < BOARD_SIZE; k++) col[k] = 0;

        // Merge
        for (int x = 0; x < BOARD_SIZE - 1; x++) {
            if (col[x] == col[x+1] && col[x] != 0) {
                col[x] *= 2;
                score += col[x];
                for (int j = x+1; j < BOARD_SIZE - 1; j++) col[j] = col[j+1];
                col[BOARD_SIZE - 1] = 0;
            }
        }

        // Put back
        for (int x = 0; x < BOARD_SIZE; x++) {
            if (board[y][x] != col[x]) moved = 1;
            board[y][x] = col[x];
        }
    }
    if (moved) add_random_tile();
}

void move_right() {
    int moved = 0;
    for (int y = 0; y < BOARD_SIZE; y++) {
        int col[BOARD_SIZE];
        int k = 0;
        for (int x = BOARD_SIZE - 1; x >= 0; x--) {
            if (board[y][x] != 0) col[k++] = board[y][x];
        }
        for (; k < BOARD_SIZE; k++) col[k] = 0;

        for (int x = BOARD_SIZE - 1; x > 0; x--) {
            if (col[x] == col[x-1] && col[x] != 0) {
                col[x] *= 2;
                score += col[x];
                for (int j = x-1; j > 0; j--) col[j] = col[j-1];
                col[0] = 0;
            }
        }

        int idx = 0;
        for (int x = BOARD_SIZE - 1; x >= 0; x--) {
            board[y][x] = col[idx++];
            if (board[y][x] != col[idx-1]) moved = 1;
        }
    }
    if (moved) add_random_tile();
}

void move_up() {
    int moved = 0;
    for (int x = 0; x < BOARD_SIZE; x++) {
        int row[BOARD_SIZE];
        int k = 0;
        for (int y = 0; y < BOARD_SIZE; y++) {
            if (board[y][x] != 0) row[k++] = board[y][x];
        }
        for (; k < BOARD_SIZE; k++) row[k] = 0;

        for (int y = 0; y < BOARD_SIZE - 1; y++) {
            if (row[y] == row[y+1] && row[y] != 0) {
                row[y] *= 2;
                score += row[y];
                for (int j = y+1; j < BOARD_SIZE - 1; j++) row[j] = row[j+1];
                row[BOARD_SIZE - 1] = 0;
            }
        }

        for (int y = 0; y < BOARD_SIZE; y++) {
            if (board[y][x] != row[y]) moved = 1;
            board[y][x] = row[y];
        }
    }
    if (moved) add_random_tile();
}

void move_down() {
    int moved = 0;
    for (int x = 0; x < BOARD_SIZE; x++) {
        int row[BOARD_SIZE];
        int k = 0;
        for (int y = BOARD_SIZE - 1; y >= 0; y--) {
            if (board[y][x] != 0) row[k++] = board[y][x];
        }
        for (; k < BOARD_SIZE; k++) row[k] = 0;

        for (int y = BOARD_SIZE - 1; y > 0; y--) {
            if (row[y] == row[y-1] && row[y] != 0) {
                row[y] *= 2;
                score += row[y];
                for (int j = y-1; j > 0; j--) row[j] = row[j-1];
                row[0] = 0;
            }
        }

        int idx = 0;
        for (int y = BOARD_SIZE - 1; y >= 0; y--) {
            board[y][x] = row[idx++];
            if (board[y][x] != row[idx-1]) moved = 1;
        }
    }
    if (moved) add_random_tile();
}

void draw_board() {
    werase(game_win);

    // Title and score
    wattron(game_win, A_BOLD);
    mvwprintw(game_win, 1, 2, "2048 GAME - Score: %d", score);
    wattroff(game_win, A_BOLD);

    // Board border
    box(game_win, 0, 0);

    // Draw tiles
    for (int y = 0; y < BOARD_SIZE; y++) {
        for (int x = 0; x < BOARD_SIZE; x++) {
            int tile_x = 2 + x * TILE_SIZE;
            int tile_y = 4 + y * TILE_SIZE / 1.2;  // Slight overlap look

            if (board[y][x] != 0) {
                char buf[20];
                sprintf(buf, "%d", board[y][x]);
                int color = 0;
                if (board[y][x] >= 2048) color = COLOR_PAIR(1);
                else if (board[y][x] >= 1024) color = COLOR_PAIR(2);
                else if (board[y][x] >= 512) color = COLOR_PAIR(3);
                wattron(game_win, COLOR_PAIR(color) | A_BOLD);
                mvwhline(game_win, tile_y, tile_x, ' ', TILE_SIZE-2);
                mvwprintw(game_win, tile_y + 1, tile_x + 1, "%s", buf);
                wattroff(game_win, COLOR_PAIR(color) | A_BOLD);
            } else {
                mvwhline(game_win, tile_y, tile_x, ' ', TILE_SIZE-2);
            }
        }
    }

    if (game_over) {
        wattron(game_win, A_BLINK | A_BOLD | COLOR_PAIR(4));
        mvwprintw(game_win, HEIGHT/2, WIDTH/2 - 10, "GAME OVER!");
        wattroff(game_win, A_BLINK | A_BOLD | COLOR_PAIR(4));
        mvwprintw(game_win, HEIGHT/2 + 2, WIDTH/2 - 8, "Final Score: %d", score);
        mvwprintw(game_win, HEIGHT/2 + 4, WIDTH/2 - 15, "Press R to Restart | Q to Quit");
    } else {
        mvwprintw(game_win, HEIGHT - 2, 2, "WASD/Arrows: Move | Q: Quit | R: Restart");
    }

    wrefresh(game_win);
}

int main() {
    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(100);  // Slight delay for smooth feel

    // Colors for tiles
    start_color();
    init_pair(1, COLOR_WHITE, COLOR_MAGENTA);  // 2048+
    init_pair(2, COLOR_BLACK, COLOR_YELLOW);   // 1024+
    init_pair(3, COLOR_BLACK, COLOR_CYAN);     // 512+
    init_pair(4, COLOR_RED, COLOR_BLACK);      // Game over

    game_win = newwin(HEIGHT, WIDTH, (LINES - HEIGHT)/2, (COLS - WIDTH)/2);
    keypad(game_win, TRUE);

    init_board();
    draw_board();

    printw("=== Super Addictive 2048 Game! ===\n");
    printw("Goal: Reach 2048 tile! Swipe with WASD/Arrows.\n");
    printw("Press any key to start...");
    getch();
    clear();

    while (!game_over) {
        int ch = wgetch(game_win);

        int moved = 0;
        switch (ch) {
            case KEY_LEFT: case 'a': case 'A':
                move_left(); moved = 1; break;
            case KEY_RIGHT: case 'd': case 'D':
                move_right(); moved = 1; break;
            case KEY_UP: case 'w': case 'W':
                move_up(); moved = 1; break;
            case KEY_DOWN: case 's': case 'S':
                move_down(); moved = 1; break;
            case 'q': case 'Q':
                game_over = 1; break;
            case 'r': case 'R':
                init_board(); break;
        }

        if (moved) {
            if (!can_move()) game_over = 1;
        }

        draw_board();
        usleep(50000);  // Smooth update
    }

    mvprintw(LINES/2 + 6, (COLS - 20)/2, "Thanks for playing!");
    refresh();
    getch();

    endwin();
    return 0;
}