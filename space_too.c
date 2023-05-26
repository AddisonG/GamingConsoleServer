#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "engine.h"
#include "space_too.h"

#define GAME_WIDTH 128
#define GAME_HEIGHT 64
#define BUFFER_SIZE 8

#define SYM_HORIZONTAL '-'
#define SYM_EMPTY ' '
#define SYM_PLAYER "assets/s2_ship.png"
#define SYM_ENEMY "assets/s2_a.png"
#define SYM_ENEMY_B "assets/s2_a_broken.png"
#define SYM_BULLET "assets/s2_bullet.png"

#define MAX_BULLETS 5
#define MAX_ASTEROIDS 7

#define AXIS_X 0
#define AXIS_Y 1

#define X_NONE 0
#define X_FORWARD -1
#define X_BACKWARD 1
#define Y_NONE 0
#define Y_FORWARD -1
#define Y_BACKWARD 1

#define SHIP_W 13
#define SHIP_H 17
#define BULLET_W 3
#define BULLET_H BULLET_W
#define ASTEROID_W 12
#define ASTEROID_H 9

typedef struct bullet Bullet;
typedef struct player Player;
typedef struct asteroid Asteroid;

struct bullet {
	char sym[32];
	int x;
	int y;
	int speed;
};

struct player {
	char sym[32];
	int x;
	int y;
	int y_dir;
	int len;
	int score;
	int hp;
	int bullet_count;
	Bullet bullets[MAX_BULLETS];
};

struct asteroid {
	int size;
	int x;
	int y;
	int speed;
	int hp;
	char sym[32];
};

/* State */
int get_new_pos(int old, int amount, int min, int max) {
	int new = old + amount;

	if (new >= max) {
		new = max;
	}
	else if (new <= min) {
		new = min;
	}
	return new;
}

bool has_player_collision(
	Player *player,
	Asteroid asteroid
) {
	// Calculate the half-width and half-height of each object
	int half_player_w = SHIP_W / 2;
	int half_player_h = SHIP_H / 2;
	int half_asteroid_w = ASTEROID_W / 2;
	int half_asteroid_h = ASTEROID_H / 2;

	// Calculate the x and y coordinates of the left, right, top, and bottom edges for each object
	int left1 = player->x - half_player_w;
	int right1 = player->x + half_player_w;
	int top1 = player->y - half_player_h;
	int bottom1 = player->y + half_player_h;
	int left2 = asteroid.x - half_asteroid_w;
	int right2 = asteroid.x + half_asteroid_w;
	int top2 = asteroid.y - half_asteroid_h;
	int bottom2 = asteroid.y + half_asteroid_h;

	// Check for overlap
	if (left1 <= right2 && right1 >= left2 && top1 <= bottom2 && bottom1 >= top2) {
		return true;
	} else {
		return false;
	}
}

bool has_bullet_collision(
	Player* player,
	int bullet_index,
	Asteroid asteroid
) {
	Bullet bullet = player->bullets[bullet_index];
	int a_x_min, a_y_min, a_x_max, a_y_max;

	a_x_min = asteroid.x - ASTEROID_W / 2;
	a_y_min = asteroid.y - ASTEROID_H / 2;
	a_x_max = asteroid.x + ASTEROID_W / 2;
	a_y_max = asteroid.y + ASTEROID_H / 2;

	return (
		(a_x_min <= bullet.x && bullet.x <= a_x_max) &&
		(a_y_min <= bullet.y && bullet.y <= a_y_max)
	);
}

void erase_bullet(Player* player, int bullet_index) {
	for (int i = bullet_index; i < player->bullet_count - 1; i++) {
		player->bullets[i] = player->bullets[i + 1];
	}
}

void erase_asteroid(
	Asteroid (*asteroids)[MAX_ASTEROIDS],
	int asteroid_count,
	int asteroid_index
) {
	for (int i = asteroid_index; i < asteroid_count - 1; i++) {
		(*asteroids)[i] = (*asteroids)[i + 1];
	}
}

void move_bullet(Bullet* bullet) {
	bullet->x = get_new_pos(bullet->x, bullet->speed, 0, GAME_WIDTH - 1);
}

void move_bullets(Player* player) {
	Bullet bullet;
	int i;

	for (i = 0; i < player->bullet_count; i++) {
		bullet = player->bullets[i];

		if (bullet.x == GAME_WIDTH - 1) {
			erase_bullet(player, i);
			player->bullet_count--;
			i--;
			continue;
		}

		move_bullet(&(player->bullets[i]));
	}
}

void move_player(Player* player, int amount, bool move_once) {
	int move_dist, new_pos;

	move_dist = amount * player->y_dir;
	new_pos = get_new_pos(
		player->y,
		move_dist,
		BUFFER_SIZE,
		GAME_HEIGHT - BUFFER_SIZE
	);
	player->y = new_pos;

	if (move_once) {
		player->y_dir = Y_NONE;
	}
}

void move_asteroid(
	Asteroid (*asteroids)[MAX_ASTEROIDS],
	int* asteroid_count,
	int asteroid_index
) {
	Asteroid asteroid = (*asteroids)[asteroid_index];
	int new_pos = get_new_pos(
		asteroid.x,
		-1 * asteroid.speed,
		0,
		GAME_WIDTH - 1
	);
	(*asteroids)[asteroid_index].x = new_pos;

	if ((*asteroids)[asteroid_index].x == 0) {
		erase_asteroid(asteroids, *asteroid_count, asteroid_index);
		(*asteroid_count)--;
	}
}

void handle_player_collision(
	Player* player,
	Asteroid (*asteroids)[MAX_ASTEROIDS],
	int* asteroid_count,
	int asteroid_index
) {
	erase_asteroid(asteroids, *asteroid_count, asteroid_index);
	(*asteroid_count)--;
	player->hp--;
}

bool handle_bullet_collision(
	Player* player,
	Asteroid (*asteroids)[MAX_ASTEROIDS],
	int* asteroid_count,
	int asteroid_index,
	int bullet_index
) {
	bool removed = false;

	(*asteroids)[asteroid_index].hp--;
	if ((*asteroids)[asteroid_index].hp == 0) {
		player->score++;
		erase_asteroid(asteroids, *asteroid_count, asteroid_index);
		(*asteroid_count)--;
		removed = true;
	} else {
		strcpy((*asteroids)[asteroid_index].sym, SYM_ENEMY_B);
	}
	erase_bullet(player, bullet_index);
	player->bullet_count--;

	return removed;
}

void move_sprites(
	Player* player,
	Asteroid (*asteroids)[MAX_ASTEROIDS],
	int* asteroid_count
) {
	int i, j;

	move_player(player, 1, true);
	move_bullets(player);

	for (i = 0; i < *asteroid_count; i++) {
		move_asteroid(asteroids, asteroid_count, i);

		if (has_player_collision(player, (*asteroids)[i])) {
			handle_player_collision(
				player,
				asteroids,
				asteroid_count,
				i
			);
			i--;
		}

		for (j = 0; j < player->bullet_count; j++) {
			if (has_bullet_collision(player, j, (*asteroids)[i])) {
				if (handle_bullet_collision(
					player,
					asteroids,
					asteroid_count,
					i, j
				)) {
					i--;
				}
				break;
			}
		}
	}
}

void spawn_bullet(Player* player) {
	if (player->bullet_count == MAX_BULLETS) {
		return;
	}

	int bullet_x, bullet_y;
	bullet_x = player->x + player->len + 1;
	bullet_y = player->y;

	Bullet bullet = {
		.x = bullet_x,
		.y = bullet_y,
		.speed = 2,
		.sym = SYM_BULLET
	};
	player->bullets[player->bullet_count] = bullet;
	player->bullet_count++;

}

void spawn_asteroid(Asteroid (*asteroids)[MAX_ASTEROIDS], int* asteroid_count) {
	int asteroid_x, asteroid_y, r;

	if (*asteroid_count == MAX_ASTEROIDS) {
		return;
	}

	// Only want asteroids 1/5 of the time
	if (random() % 5 != 0) {
		return;
	}

	r = random();
	asteroid_x = GAME_WIDTH - 1;
	asteroid_y = (r % (GAME_HEIGHT - BUFFER_SIZE)) + (BUFFER_SIZE / 2 + 1);

	Asteroid asteroid = {
		.size = 1,
		.x = asteroid_x,
		.y = asteroid_y,
		.speed = 1,
		.hp = 2,
		.sym = SYM_ENEMY
	};

	(*asteroids)[*asteroid_count] = asteroid;
	(*asteroid_count) += 1;
}

/* View */
void print_player(Player player, struct fb *fb) {
	render_bitmap(
		fb,
		player.sym,
		player.x - SHIP_W / 2,
		player.y - SHIP_H / 2
	);
}

void print_bullets(Player player, struct fb *fb) {
	Bullet bullet;
	int i;

	for (i = 0; i < player.bullet_count; i++) {
		bullet = player.bullets[i];
		render_bitmap(
			fb,
			bullet.sym,
			bullet.x - BULLET_W / 2,
			bullet.y - BULLET_H / 2
		);
	}
}

void print_asteroids(Asteroid asteroids[], int asteroid_count, struct fb *fb) {
	Asteroid asteroid;
	int i;

	for (i = 0; i < asteroid_count; i++) {
		asteroid = asteroids[i];
		render_bitmap(
			fb,
			asteroid.sym,
			asteroid.x - ASTEROID_W / 2,
			asteroid.y - ASTEROID_H / 2
		);
	}
}

void print_game(
	struct fb *fb,
	Player player,
	Asteroid asteroids[],
	int asteroid_count
) {
	clear_buffer(fb);

	// Game elements
	print_player(player, fb);
	print_bullets(player, fb);
	print_asteroids(asteroids, asteroid_count, fb);

	swap_buffer(fb);
}

/* Main */
int space_too(struct font *ft, struct fb *fb, int buttons_fd) {
	Asteroid asteroids[MAX_ASTEROIDS];
	int asteroid_count;
	bool game_over = false;

	// Init
	srandom(1000);
	Player player = {
		.x = BUFFER_SIZE,
		.y = (GAME_HEIGHT + 1) / 2,
		.bullet_count = 0,
		.len = 	2,
		.sym = SYM_PLAYER,
		.hp = 3,
		.score = 0
	};
	asteroid_count = 0;

	while (!game_over) {
		// Draw game
		print_game(fb, player, asteroids, asteroid_count);

		// Update paddle positions
		struct button_state* buttons = read_buttons(buttons_fd);

		if (buttons->up) {
        		player.y_dir = Y_FORWARD;
        	}
        	if (buttons->down) {
        		player.y_dir = Y_BACKWARD;
        	}
		if (buttons->back) {
			spawn_bullet(&player);
		}

		move_sprites(&player, &asteroids, &asteroid_count);

		spawn_asteroid(&asteroids, &asteroid_count);

		if (player.hp == 0) {
			game_over = true;
		}

		usleep(100000);
	}

        char score_string[50];
        sprintf(score_string, "SCORE: %d", player.score);

        clear_buffer(fb);
        draw_box(fb, 1, 1, 126, 62, false);
        render_string(fb, ft, "= GAME OVER =", false, 20, 20);
        render_string(fb, ft, "= YOU LOSE =", false, 23, 28);
        render_string(fb, ft, score_string, false, 33, 36);
        swap_buffer(fb);

	return 0;
}
