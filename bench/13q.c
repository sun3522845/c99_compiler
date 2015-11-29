int printf(char *format, ...);
int queens[13];
int solutions;

int slow_safe(int n) 
{
    int i;
    for (i = 0; i < n; i++) {
        if (queens[i] == queens[n] ||
            n-i == queens[i]-queens[n] ||
            n-i == queens[n]-queens[i])
            return 0;
    }
    return 1;
}

int safe(int n) 
{
    int i;
    for (i = 0; i < n; i++) {
        int delta_x, delta_y;
        delta_x = n - i;
        delta_y = queens[n] - queens[i];
        if (delta_y == 0 ||
            delta_y == delta_x ||
            delta_y == 0 - delta_x)
            return 0;
    }
    return 1;
}

void scan(int n)
{
    if (n == 13) {
        solutions++;
        return;
    }

    int pos;
    for (pos = 0; pos < 13; pos++) {
        queens[n] = pos;
        if (safe(n))
            scan(n + 1);
    }
}

int main()
{
    scan(0); 
    printf("Total solution for 13 queen placement:%d\n", solutions);
}
