#include "funcition.h"
#include "MainWindow.h"
#include "Snake.h"
#include "food.h"
#include "Text.h"

void Resize() {
	if (block_x > MAX_WIDTH) {
		block_x = -10;
	}
	if (block_y > MAX_HEIGHT) {
		block_y = -10;
	}
	if (block_x < -10) {
		block_x = MAX_WIDTH;
	}
	if (block_y < -10) {
		block_y = MAX_HEIGHT;
	}
}


int main(int argc, char* argv[]) {
	Window g_window = Window("Snake Game", MAX_WIDTH, MAX_HEIGHT);
	g_window.SetIcon(path_image);

	Snake block = Snake(50, 50, 10, 10);

	Food food = Food(10, 10);
	Text ScoreText = Text(path_font, g_window.p_renderer, "Score: " + std::to_string(score), _WHITE, 20, 20, 100, 30);
	Text GameOverText = Text(path_font, g_window.p_renderer, "Game Over", _WHITE, 300, 300, 200, 50);
	std::deque<Snake> snake;

	while (running) {
		while (SDL_PollEvent(&g_event)) {
			if (g_event.type == SDL_QUIT) {
				running = false;
			}
			else if (g_event.type == SDL_KEYDOWN) {
				if (g_event.key.keysym.sym == SDLK_UP) {
					x_vel = 0; y_vel = -10;
				}
				else if (g_event.key.keysym.sym == SDLK_DOWN) {
					x_vel = 0; y_vel = 10;
				}
				else if (g_event.key.keysym.sym == SDLK_LEFT) {
					x_vel = -10; y_vel = 0;
				}
				else if (g_event.key.keysym.sym == SDLK_RIGHT) {
					x_vel = 10; y_vel = 0;
				}
				else if (g_event.key.keysym.sym == SDLK_SPACE && block.die == true) {
					block_x = 50; block_y = 50;
					block.Update(block_x, block_y);
					block.die = false;
					lenght_snake = 1;
					score = 0;
					snake.erase(snake.begin(), snake.end());
					ScoreText.Update(g_window.p_renderer, "Score: " + std::to_string(score), path_font, _WHITE);
					food.Update();
				}
			}
			
		}

		if (block.die == false) {
			block_x += x_vel; block_y += y_vel;
			snake.push_back(block);
			if (snake.size() >= lenght_snake) {
				snake.pop_front();
			}

			Resize();

			if (block.Collidrect(food)) {
				food.Update();
				lenght_snake += 1;
				score += 1;
				ScoreText.Update(g_window.p_renderer, "Score: " + std::to_string(score), path_font, _WHITE);
			}

			g_window.Fill(BLACK);
			block.Update(block_x, block_y);

			for (auto i = 1; i < snake.size(); i++) {
				if (block.snake_rect.x == snake[i].snake_rect.x && block.snake_rect.y == snake[i].snake_rect.y) {
					block.die = true;
				}
			}
			block.Draw(g_window.p_renderer, BlueF);
			for (int i = 0; i < snake.size(); i++) {
				snake[i].Draw(g_window.p_renderer, WHITE);
			}
		}
		
		else {
			GameOverText.Render(g_window.p_renderer);
			x_vel = 0; y_vel = 0;
		}

		ScoreText.Render(g_window.p_renderer);
		food.Draw(g_window.p_renderer, RED);
		g_window.Render();
		SDL_Delay(1000 / FPS);

	}
	g_window.free();
	TTF_Quit();
	SDL_Quit();
	return 0;
}