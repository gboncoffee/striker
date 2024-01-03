#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "raylib.h"

#define WIDTH 800
#define HEIGHT 800
#define N_MISSILES 3
#define N_ENEMIES 9
#define BASE_RADIUS 20
#define MISSILE_RADIUS 5
#define MODULO(a) ((a) >= 0 ? (a) : -(a))

typedef struct Missile {
	Vector2 pos;
	Vector2 target;
	int deployed;
	int target_radius;
} Missile;

Missile enemies[N_ENEMIES];
Missile missiles[N_MISSILES];
int lost;
int pause;
int points;
int longyearbyen;

int move_missile(Missile *missile)
{
	int dx, dy, m, x, y;
	double dxdy, dist;

	dx = missile->pos.x - missile->target.x;
	dy = missile->pos.y - missile->target.y;
	dxdy = (double) (dx*dx + dy*dy);
	dist = sqrt(dxdy);

	if (dist == 0) {
		missile->pos.x = missile->target.x;
		missile->pos.y = missile->target.y;
		return 1;
	}

	x = (int) ((dx/dist) * (dist-3));
	y = (int) ((dy/dist) * (dist-3));

	missile->pos.x = x + missile->target.x;
	missile->pos.y = y + missile->target.y;

	return (sqrt(x*x + y*y) <= missile->target_radius);
}

void generate_enemy(Missile *new)
{
	int dist = (WIDTH/8)*3;
	int x = rand() % dist;
	int y = sqrt(dist*dist - x*x);
	y = rand() % 2 ? y : -y;
	x = rand() % 2 ? x : -x;
	new->pos.x = x + WIDTH/2;
	new->pos.y = y + HEIGHT/2;

	if (!longyearbyen || rand() % 3) {
		new->target.x = WIDTH/2;
		new->target.y = HEIGHT/2;
	} else {
		/* Longyearbyen. */
		new->target.x = WIDTH/2-100;
		new->target.y = HEIGHT/2-50;
	}

	new->target_radius = BASE_RADIUS;
}

void try_deploy(Vector2 target)
{
	for (int i = 0; i < N_MISSILES; i++)
		if (!missiles[i].deployed) {
			missiles[i].pos.x = WIDTH/2;
			missiles[i].pos.y = HEIGHT/2;
			missiles[i].target.x = target.x;
			missiles[i].target.y = target.y;
			missiles[i].deployed = 1;
			missiles[i].target_radius = MISSILE_RADIUS;
			return;
		}
}

void draw_radioactive_symbol(int radius, int x, int y)
{
	Vector2 center;
	center.x = x;
	center.y = y;
	DrawCircle(x, y, radius, YELLOW);
	DrawCircleSector(center, radius-10, 0, 60, 10, BLACK);
	DrawCircleSector(center, radius-10, 120, 180, 10, BLACK);
	DrawCircleSector(center, radius-10, 240, 300, 10, BLACK);
	DrawCircle(x, y, 20, YELLOW);
	DrawCircle(x, y, 10, BLACK);
}

void render(int sec, Vector2 crosspos)
{
	for (int i = 0; i < N_ENEMIES; i++) {
		if (!sec && (move_missile(&enemies[i]) == 1)) {
			if (enemies[i].target.x == WIDTH/2) {
				lost = 1;
			} else {
				longyearbyen = 0;
				for (int j = 0; j < N_ENEMIES; j++) {
					enemies[j].target.x = WIDTH/2;
					enemies[j].target.y = HEIGHT/2;
				}
				generate_enemy(&enemies[i]);
			}
		}

		DrawCircleV(enemies[i].pos, 5, RED);
	}
	for (int i = 0; i < N_MISSILES; i++) {
		if (missiles[i].deployed) {
			DrawCircleV(missiles[i].pos, 5, BLUE);
			DrawCircleV(missiles[i].target, 3, ORANGE);
		}
		if (!(sec % 10) && missiles[i].deployed) {
			int r = move_missile(&missiles[i]);
			for (int j = 0; j < N_ENEMIES; j++) {
				int dx = MODULO(missiles[i].pos.x - enemies[j].pos.x);
				int dy = MODULO(missiles[i].pos.y - enemies[j].pos.y);
				if (dx <= MISSILE_RADIUS && dy <= MISSILE_RADIUS) {
					generate_enemy(&enemies[j]);
					missiles[i].deployed = 0;
					if (longyearbyen)
						points += 2;
					else
						points++;
				}
			}
			if (r)
				missiles[i].deployed = 0;
		}
	}
}

int main(void)
{
	Vector2 crosspos;

	srand(time(NULL));

	InitWindow(WIDTH, HEIGHT, "Striker");
	SetTargetFPS(60);

SETUP:
	SetMousePosition(WIDTH/2, HEIGHT/2);
	HideCursor();

	for (int i = 0; i < N_ENEMIES; i++)
		generate_enemy(&enemies[i]);
	for (int i = 0; i < N_MISSILES; i++)
		missiles[i].deployed = 0;

	lost = 0;
	pause = 0;
	points = 0;
	longyearbyen = 1;

	int i = 1;
	while (!WindowShouldClose()) {
		if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_Q)) {
			CloseWindow();
		} else if (lost) {
			if (IsKeyPressed(KEY_R)) {
				lost = 0;
				goto SETUP;
			}

			BeginDrawing();

			ClearBackground(BLACK);

			const char *dead = "You're dead.";
			const char *t1 = "The Spitsbergen Nuclear Facility was bombed.";
			const char *t2 = "In the following hours, the Enemy is going to raid";
			const char *t3 = "the remainings of Longyearbyen.";
			const char *t4 = "The Svalbard Archipelago has fallen.";
			const char *t5 = "There's no more Resistance in the Arctic Ocean.";
			const char *game_over = "Game over.";

			int s = MeasureText(dead, 100);
			DrawText(dead, (WIDTH-s)/2, 10, 100, RED);
			s = MeasureText(t1, 30);
			DrawText(t1, (WIDTH-s)/2, 150, 30, RED);
			s = MeasureText(t2, 30);
			DrawText(t2, (WIDTH-s)/2, 180, 30, RED);
			s = MeasureText(t3, 30);
			DrawText(t3, (WIDTH-s)/2, 210, 30, RED);
			s = MeasureText(t4, 30);
			DrawText(t4, (WIDTH-s)/2, 240, 30, RED);
			s = MeasureText(t5, 30);
			DrawText(t5, (WIDTH-s)/2, 270, 30, RED);

			s = MeasureText(game_over, 100);
			DrawText(game_over, (WIDTH-s)/2, HEIGHT-150, 100, RED);

			draw_radioactive_symbol(150, WIDTH/2, HEIGHT/2+90);

			const char *score = TextFormat("Final score: %d", points);
			s = MeasureText(score, 20);
			DrawText(score, (WIDTH-s)/2, HEIGHT/2+250, 20, GREEN);

			EndDrawing();
		} else if (pause) {
			if (IsKeyPressed(KEY_P)) {
				pause = 0;
				HideCursor();
			}

			BeginDrawing();

			const char *pause = "Paused";
			int s = MeasureText(pause, 100);
			DrawText(pause, (WIDTH-s)/2, HEIGHT/2-50, 100, GREEN);

			EndDrawing();
		} else {
			if (IsKeyPressed(KEY_K)) {
				lost = 1;
				ShowCursor();
			} else if (IsKeyPressed(KEY_P)) {
				pause = 1;
				ShowCursor();
			}

			BeginDrawing();

			ClearBackground(BLACK);
			DrawCircleLines(WIDTH/2, HEIGHT/2, (WIDTH/8), GREEN);
			DrawCircleLines(WIDTH/2, HEIGHT/2, (WIDTH/8)*2, GREEN);
			DrawCircleLines(WIDTH/2, HEIGHT/2, (WIDTH/8)*3, GREEN);

			crosspos = GetMousePosition();
			DrawLine(crosspos.x, 0, crosspos.x, HEIGHT, GREEN);
			DrawLine(0, crosspos.y, WIDTH, crosspos.y, GREEN);
			DrawCircleLines(crosspos.x, crosspos.y, 15, GREEN);
			DrawCircle(WIDTH/2, HEIGHT/2, 5, YELLOW);
			DrawText("Spitsbergen Nuclear Facility", WIDTH/2-30, HEIGHT/2-22, 10, YELLOW);

			if (longyearbyen) {
				DrawCircle(WIDTH/2-100, HEIGHT/2-50, 5, YELLOW);
				DrawText("Longyearbyen", WIDTH/2-130, HEIGHT/2-72, 10, YELLOW);
			}

			DrawText(TextFormat("Score: %d", points), 10, 10, 25, GREEN);

			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
				try_deploy(crosspos);

			render(i, crosspos);
			i++;
			if (i == 30)
				i = 0;

			EndDrawing();
		}
	}

	return 0;
}
