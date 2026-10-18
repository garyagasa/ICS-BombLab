#include <stdio.h>

__attribute__((noinline))
int calculate(int x) {
    int y = 3 * x + 7;
    if (y > 20)
        return y;
    return 0;
}

int main(void) {
    int result = calculate(10);
    printf("%d\n", result);
    return 0;
}