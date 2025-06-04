/* a small utility to test the random sequences. needs util.c */
#include <math.h>
#include <limits.h>
#include <stdio.h>

#define PRANDMAX 1000000000L

#if 1
int rng(int limit) {
    double z;
    z = ((double)(prand()) / (double)PRANDMAX) * (double)limit;
    return (int)z;
}
#endif
#if 0
int rng(int limit) {
    double z;
    z = ((double)(rand() & 32767) / 32768.0) * (double)limit;
    return (int)z;
}
#endif
#if 0
int rng(int limit) {
    return rand() % limit;
}
#endif
#if 0
int rng(int limit) {
    return prand() % limit;
}
#endif


#define MAXCARDS 52
int cards[MAXCARDS];


static void shuffle(void) {
    int i, d, v, c;
    int tmp[MAXCARDS];
    for (i = 0; i < 52; ++i)
	tmp[i] = i;
    for (i = 52; i; --i) {
	v = rng(i);
	c = -1;
	do {
	    while (tmp[++c] == -1)
		;			/* skip card */
	} while (v--);
	cards[i-1] = tmp[c];
	/* printf("c = %d, card = %d\n", c, game.cards[i-1]); */
	tmp[c] = -1;
    }
}

int cnt[52][10];

int main(int c, char *v[]) {
    int i, n = atoi(v[1]);
    memset(cnt, 0, sizeof(cnt));

    while (n--) {
	sprand(n+1234567L);
	srand(n+1234567L);
	shuffle();
	for (i = 0; i < 10; ++i)
	    ++cnt[cards[i]][i];
    }
    for (i = 0; i < 52; ++i)
	printf("%2d: %6d %6d %6d %6d %6d  %6d %6d %6d %6d %6d\n", i,
	       cnt[i][0], cnt[i][1], cnt[i][2], cnt[i][3], cnt[i][4],
	       cnt[i][5], cnt[i][6], cnt[i][7], cnt[i][8], cnt[i][9]);
}
