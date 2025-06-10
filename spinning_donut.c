#include <stdio.h>   // Standard input/output library (for printf, putchar)
#include <math.h>    // Math library (for sin, cos)
#include <string.h>  // String library (for memset)

#ifdef _WIN32        // Conditional compilation for Windows
#include <windows.h> // Windows API (for Sleep, console functions)
#else
#include <unistd.h>  // POSIX API (for usleep)
#endif

int main() {
#ifdef _WIN32
    // Enable ANSI escape code processing on Windows (for clearing the screen and cursor positioning)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE); // Get the console output handle
    DWORD dwMode = 0;                              // Variable to store the console mode
    GetConsoleMode(hOut, &dwMode);                 // Get the current console mode
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;  // Enable virtual terminal processing (ANSI escape codes)
    SetConsoleMode(hOut, dwMode);                  // Set the new console mode
#endif

    float A = 0, B = 0; // Rotation angles for the donut (around x and z axes)
    float i, j;         // Loop variables for iterating over the donut's surface
    int k;              // Loop variable for iterating over the screen buffer
    float z[1760];      // Z-buffer (depth buffer) to store the distance to each point on the screen
    char b[1760];       // Screen buffer to store the characters to be displayed

    printf("\x1b[2J"); // Clear the screen using an ANSI escape code

    for (;;) { // Infinite loop to continuously redraw the donut (animation)
        memset(b, 32, 1760); // Clear the screen buffer (b) with spaces (ASCII 32) before each frame.
                             // This ensures that the previous frame is erased.
        memset(z, 0, 7040);  // Clear the z-buffer (z) with 0 before each frame.
                             // This ensures that hidden surface removal works correctly.
                             // 7040 = 1760 floats * 4 bytes/float

        // Iterate over the surface of the donut using angles i and j
        for (j = 0; j < 6.28; j += 0.07) { // j goes from 0 to 2*PI (a full circle)
            for (i = 0; i < 6.28; i += 0.02) { // i goes from 0 to 2*PI (a full circle)

                // Calculate sine and cosine values for the angles i, j, A, and B.
                // These values are used to perform 3D rotations and projections.
                float c = sin(i);
                float d = cos(j);
                float e = sin(A);
                float f = sin(j);
                float g = cos(A);
                float h = d + 2; // A small offset to avoid division by zero
                float D = 1 / (c * h * e + f * g + 5); // Calculate the inverse of the distance to the point.
                                                       // This is used for scaling and perspective.
                float l = cos(i);
                float m = cos(B);
                float n = sin(B);
                float t = c * h * g - f * e;

                // Project the 3D point onto the 2D screen
                int x = 40 + 30 * D * (l * h * m - t * n); // Calculate the x-coordinate on the screen.
                                                           // 40 is an offset to center the donut horizontally.
                int y = 12 + 15 * D * (l * h * n + t * m); // Calculate the y-coordinate on the screen.
                                                           // 12 is an offset to center the donut vertically.
                int o = x + 80 * y; // Calculate the index into the screen buffer (b) based on x and y.
                                    // 80 is the width of the console screen.

                // Calculate the brightness of the point
                int N = 8 * ((f * e - c * d * g) * m - c * d * e - f * g - l * d * n); // Calculate the normal vector
                                                                                        // and use it to determine brightness.

                // Check if the point is within the screen bounds and if it's closer than what's already there
                if (22 > y && y > 0 && x > 0 && 80 > x && D > z[o]) {
                    // If the point is visible, update the z-buffer and the screen buffer
                    z[o] = D; // Store the distance to the point in the z-buffer.
                    b[o] = ".,-~:;=!*#$@"[N > 0 ? N : 0]; // Choose a character based on the brightness (N).
                                                         // The string ".,-~:;=!*#$@" provides different characters
                                                         // to represent different levels of brightness.
                }
            }
        }

        printf("\x1b[H"); // Move the cursor to the top-left corner of the screen.

        // Print the screen buffer to the console
        for (k = 0; k < 1760; k++) { // FIXED: Loop runs up to 1760, not 1761, to prevent out-of-bounds access.
            putchar(k % 80 ? b[k] : 10); // Print the character at index k.
                                         // If k is a multiple of 80, print a newline character (10)
                                         // to move to the next line.
        }

        A += 0.07; // rotation speed for visible animation.
        B += 0.04; // rotation speed for visible animation.

#ifdef _WIN32
        Sleep(30);     // Pause for 30 milliseconds (Windows)
#else
        usleep(30000); // Pause for 30 milliseconds (30000 microseconds) (POSIX)
#endif
    }

    return 0;
}
