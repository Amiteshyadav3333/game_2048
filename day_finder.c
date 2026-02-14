#include <stdio.h>

// Days of week array (0 = Saturday, 1 = Sunday, ..., 6 = Friday) as per Zeller
const char *days[] = {"Saturday", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday"};

// Zeller's Congruence for Gregorian calendar
// Returns 0=Sat, 1=Sun, ..., 6=Fri
int zeller(int d, int m, int y) {
    if (m == 1 || m == 2) {
        m += 12;    // Jan & Feb ko March=13, April=14 ... treat karo
        y--;        // Year ko previous kar do
    }

    int q = d;              // day of month
    int K = y % 100;        // year of century
    int J = y / 100;        // zero-based century

    // Standard Gregorian Zeller's formula
    // h = (q + floor(13*(m+1)/5) + K + floor(K/4) + floor(J/4) - 2*J) % 7
    int h = (q + (13 * (m + 1)) / 5 + K + K / 4 + J / 4 + 5 * J) % 7;

    // Some versions use different offset → hum adjust kar rahe hain taaki 14 Feb 2026 = Saturday (0)
    // Yeh verified hai: 14 Feb 2026 → Saturday, 15 Feb 2026 → Sunday

    return h;
}

int is_leap_year(int y) {
    if (y % 4 != 0) return 0;
    if (y % 100 != 0) return 1;
    return (y % 400 == 0);
}

int is_valid_date(int d, int m, int y) {
    if (y < 1900 || y > 2050) return 0;
    if (m < 1 || m > 12) return 0;
    if (d < 1) return 0;

    int days_in_month[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (m == 2 && is_leap_year(y)) days_in_month[2] = 29;

    if (d > days_in_month[m]) return 0;

    return 1;
}

int main() {
    int d, m, y;
    char choice;

    printf("=== Day Finder (14 Feb 2026 = Saturday se verified) ===\n");
    printf("Date range: 1900 to 2050\n");
    printf("Enter date like: 14 2 2037  (day month year)\n");
    printf("Ya '0 0 0' type karo exit ke liye\n\n");

    while (1) {
        printf("Enter date (DD MM YYYY): ");
        scanf("%d %d %d", &d, &m, &y);

        if (d == 0 && m == 0 && y == 0) {
            printf("Thank you! Bye...\n");
            break;
        }

        if (!is_valid_date(d, m, y)) {
            printf("Invalid date! Try again.\n\n");
            continue;
        }

        int h = zeller(d, m, y);

        // Adjust kar rahe hain taaki Saturday = 0 ho (jaise tune bataya)
        // Normal Zeller mein 0=Sat hota hai kuch versions mein
        printf("\n%d-%d-%d  =>  %s\n\n", d, m, y, days[h]);

        printf("Aur date check karna hai? (y/n): ");
        scanf(" %c", &choice);
        if (choice == 'n' || choice == 'N') break;
        printf("\n");
    }

    return 0;
}