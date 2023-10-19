#include "jjuggumi.h"
#include "canvas.h"
#include "keyin.h"
#include <stdio.h>
/*
* 해야할일: 1. 탈락 알고리즘 완성( dialog 로 방송)
*           2. 영희 옆에가면 통과
*           3. q 즉시 나가기
*           4. 한명만 남으면 즉시 종료
*/
#define DIR_UP		0
#define DIR_DOWN	1
#define DIR_LEFT	2
#define DIR_RIGHT	3

void sample_init(void);
void move_manual(key_t key);
void move_random(int i, int dir);
void move_tail(int i, int nx, int ny);
void camera(int x);
void observe(int see);

int px[PLAYER_MAX], py[PLAYER_MAX], period[PLAYER_MAX];  // 각 플레이어 위치, 이동 주기
int px_log[PLAYER_MAX], py_log[PLAYER_MAX];//움직임 측정용 로그

int random_move;
int random_x, random_y;

void sample_init(void) {
	map_init(12, 70);

	int x, y;
	for (int i = 0; i < n_player; i++) {
			y = N_COL - 2;
		// 같은 자리가 나오면 다시 생성
		do {
			x = randint(1, N_ROW - 2);

		} while (!placable(x, y));
		px[i] = x;
		py[i] = y;
		period[i] = randint(100, 300);

		back_buf[px[i]][py[i]] = '0' + i;  // (0 .. n_player-1)
	}

	tick = 0;
}

void camera(int x) {//영희 변경
	if (x == 1) {
		for (int i = 4; i < 8; i++) {
	     back_buf[i][2] = '#';
		}
	}
	else if (x == 0) {
		for (int i = 4; i < 8; i++) {
			back_buf[i][2] = '@';
		}
	}
}

void move(void) {//움직임 방향 확률
	random_move = randint(1, 10);
		if (random_move <= 1) {//가만히 있을확률 10%
			random_x = 0;
			random_y = 0;
		}
		else if (random_move <= 2) {// 아래로 내려갈 확률10%
			random_x = 0;
			random_y = 1;
		}
		else if (random_move <= 3) {// 위로 올라갈 확률10%
			random_x = 0;
			random_y = -1;
		}
		else {//왼쪽으로 이동할 확률 70%
			random_x = -1;
			random_y = 0;
		}
}

void observe(int see) {
	if (see = 1) {//영희 관측별 움직임 함수

		  for (int i = 1; i < n_player; i++) {//보지 않을때 ai움직임
			  if (tick % period[i] == 0) {
			move();
			move_random(i, -1);
		      }
		  }
	}
	else if (see = 0) {//영희가 볼때 ai움직임
		for (int i = 1; i < n_player; i++) {
			if (randint(1, 10) == 1) {
				move();
				move_random(i, -1);
			}
		}
	}
}

int save[PLAYER_MAX];//
void stop(void) {//영희가 볼때 움직일경우 살아남을 조건
	for (int i = 0; i < n_player; i++) {
		px_log[i] = px[i];
		py_log[i] = py[i];
		
	}
	for (int i = 0; i < n_player; i++) {
		for (int j = i + 1; j < n_player; j++) {
			if (py_log[i] == py_log[j] && px_log[i] > px_log[j]) {
				save[i] = 0;
				break;
			}
			else {
				save[i] = 1;
			}
		}
	}
}

//bool gameover[PLAYER_MAX] = true;
void kill_player(void) {//
	for (int i = 0; i < n_player; i++) {
		if (px[i] =! px_log[i]) {
			if (save[i] == 0) {
				back_buf[px[i]][py[i]] = ' ';//문제있음 수정해야함
				//gameover[i] = false;
			}
		}
	}
}

void move_manual(key_t key) {
	// 각 방향으로 움직일 때 x, y값 delta
	static int dx[4] = { -1, 1, 0, 0 };
	static int dy[4] = { 0, 0, -1, 1 };

	int dir;  // 움직일 방향(0~3)
	switch (key) {
	case K_UP: dir = DIR_UP; break;
	case K_DOWN: dir = DIR_DOWN; break;
	case K_LEFT: dir = DIR_LEFT; break;
	case K_RIGHT: dir = DIR_RIGHT; break;
	default: return;
	}

	// 움직여서 놓일 자리
	int nx, ny;
	nx = px[0] + dx[dir];
	ny = py[0] + dy[dir];
	if (!placable(nx, ny)) {
		return;
	}

	move_tail(0, nx, ny);
}

// 0 <= dir < 4가 아니면 랜덤
void move_random(int player, int dir) {
	int p = player;  // 이름이 길어서...
	int nx, ny;  // 움직여서 다음에 놓일 자리
	int attempt = 0;

	// 움직일 공간이 없는 경우는 없다고 가정(무한 루프에 빠짐)	
	do {
		nx = px[p] + random_y;
		ny = py[p] + random_x;
		attempt++;
		if (attempt > 3) {
			// 모든 방향 시도 후에도 이동 가능한 공간을 찾지 못하면 반환
			return;
		}
	} while (!placable(nx, ny));

	move_tail(p, nx, ny);
}

// back_buf[][]에 기록
void move_tail(int player, int nx, int ny) {
	int p = player;  // 이름이 길어서...
	back_buf[nx][ny] = back_buf[px[p]][py[p]];
	back_buf[px[p]][py[p]] = ' ';
	px[p] = nx;
	py[p] = ny;
}

void mugung(void) {
	if (tick == 10) {
		gotoxy(N_ROW + 1, 0);
		printf("무");

	}
	else if (tick == 100) {
		gotoxy(N_ROW + 1, 0);
		printf("무궁");
	}
	else if (tick == 300) {
		gotoxy(N_ROW + 1, 0);
		printf("무궁화");
	}
	else if (tick == 600) {
		gotoxy(N_ROW + 1, 0);
		printf("무궁화꽃");
	}
	else if (tick == 1000) {
		gotoxy(N_ROW + 1, 0);
		printf("무궁화꽃이");
	}
	else if (tick == 1700) {
		gotoxy(N_ROW + 1, 0);
		printf("무궁화꽃이 피");
	}
	else if (tick == 2000) {
		gotoxy(N_ROW + 1, 0);
		printf("무궁화꽃이 피었");
	}
	else if (tick == 2200) {
		gotoxy(N_ROW + 1, 0);
		printf("무궁화꽃이 피었습");
	}
	else if (tick == 2300) {
		gotoxy(N_ROW + 1, 0);
		printf("무궁화꽃이 피었습니");
	}
	else if (tick == 2380) {
		gotoxy(N_ROW + 1, 0);
		printf("무궁화꽃이 피었습니다");
		camera(0);
	}
	else if (tick == 5380) {
		for (int i = 0; i < 20; i++) {
			gotoxy(N_ROW + 1, i);
			printf(" ");
			tick = 0;
			camera(1);
		}
	}

}

void sample(void) {
	sample_init();
	camera(1);

	system("cls");
	display();
	while (1) {
		// player 0만 손으로 움직임(4방향)
		key_t key = get_key();
		if (tick % 20 == 0) {
			if (key == K_QUIT) {
				break;
			}
			else if (key != K_UNDEFINED) {
				move_manual(key);
			}
		}
			if (tick < 3480) {
				observe(1);
			}
			else if (tick >= 3480) {
				observe(0);
				stop();
				kill_player();
			}

		
		// player 1 부터는 랜덤으로 움직임(8방향)

		mugung();
		display();
		Sleep(10);
		tick += 10;
	}
}