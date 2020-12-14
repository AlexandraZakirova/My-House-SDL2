#include <iostream>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_main.h>
#include <vector>
constexpr int WIDTH = 1000, HEIGHT = 600;
std::shared_ptr<SDL_Window> win;//хранине указателя
std::shared_ptr<SDL_Renderer> ren;
std::vector<SDL_Point> smoke;
using TexturePtr = std::shared_ptr<SDL_Texture>;
using WindowPtr = std::shared_ptr<SDL_Window>;
using RendererPtr = std::shared_ptr<SDL_Renderer>;
using TexturePtr = std::shared_ptr<SDL_Texture>;
TexturePtr bird;
TexturePtr cat;
TexturePtr egg1;
TexturePtr egg2;
inline void SDL_DIE(const char * where)
{
	throw std::runtime_error(
			std::string(where) +
			std::string(": ") +
			std::string(SDL_GetError()));
}
TexturePtr load_texture(const char * filename)
{
	SDL_Surface * surf = nullptr;
	surf = IMG_Load(filename); // Загрузка картинки
	if (surf == nullptr) SDL_DIE("IMG_Load()");
	// Здесь могла бы быть проверка на наличие прозрачности,
	// и если таковой нет - можно задать ключевой цвет
	TexturePtr result;
	result = TexturePtr(
			SDL_CreateTextureFromSurface(ren.get(), surf),
			SDL_DestroyTexture);
	if (result == nullptr) SDL_DIE("SDL_CreateTextureFromSurface()");
	SDL_FreeSurface(surf);
	return result;
}
void render_background()
{
	/*SDL_SetRenderDrawColor(ren.get(), 32, 64, 192, 255);
	SDL_RenderClear(ren.get());*/
	SDL_SetRenderDrawColor(ren.get(),63, 196, 243, 80);
	SDL_RenderClear(ren.get());
}
int bird_x = 100, bird_y = 100, bird_w = 320, bird_h = 210;
void render_bird()
{
	//птица
	SDL_Rect r_bird { bird_x, bird_y, bird_w, bird_h };
	SDL_RenderCopy(
			ren.get(), bird.get(),
			nullptr, &r_bird);
}
//кот
int cat_x = 550, cat_y = 600, cat_w = 292, cat_h = 240, cat_dy = 0;
bool cat_rig = true, cat_jump=false;
void render_cat()
{
	//кот
	SDL_Rect r_cat { cat_x, cat_y, cat_w, cat_h };
	auto flip = cat_rig?SDL_FLIP_HORIZONTAL:SDL_FLIP_NONE;
	SDL_RenderCopyEx(
			ren.get(), cat.get(),
			nullptr,  &r_cat,
			0.,						// Угол поворота
			nullptr,			// Центр поворота (с.к. текстуры)
			flip);
}

int omlet_x = 0, omlet_y = 10, omlet_w = 73, omlet_h = 56, omlet_visible=0;
void render_omlet()
{
	if (!omlet_visible) return;
	if ((omlet_visible <= 20) && ((omlet_visible%2) == 1)) return;
	SDL_Rect r_omlet { omlet_x, omlet_y, omlet_w, omlet_h };
	SDL_RenderCopy(
			ren.get(), egg2.get(),
			nullptr, &r_omlet);
}

void render_scene()
{
	//SDL_RenderCopy(ren.get(), bird.get(), nullptr, nullptr);
	/*//фон
	SDL_SetRenderDrawColor(ren.get(),65, 105, 255, 100);
	SDL_RenderClear(ren.get());*/
	//земля
	SDL_SetRenderDrawColor(ren.get(), 120, 193, 68, 120);
	SDL_Rect z{0, HEIGHT-50, WIDTH, 50};
	SDL_RenderFillRect(ren.get(),&z);
	//SDL_RenderClear(ren.get());
	//стенка
	SDL_SetRenderDrawColor(ren.get(), 238, 200, 145, 150);
	SDL_Rect s{(WIDTH/2)-125, HEIGHT-300, 390, 250};
	SDL_RenderFillRect(ren.get(),&s);
	//SDL_RenderClear(ren.get());
	//труба
	SDL_SetRenderDrawColor(ren.get(), 128, 0, 0, 100);
	SDL_Rect t{450, 200, 50, 100};
	SDL_RenderFillRect(ren.get(),&t);
	//крыша
	for(int i = 1; i < 100; i++){
		SDL_SetRenderDrawColor(ren.get(), 151, 32, 17, 100);
		SDL_RenderDrawLine(ren.get(), WIDTH/2 + 75 -i*2, HEIGHT-250 - 150+i, WIDTH/2 + 75 +i*2,HEIGHT-250 - 150+i);
		}
	//дверь
	SDL_SetRenderDrawColor(ren.get(), 63, 42, 18, 10);
	SDL_Rect d{WIDTH/2-100, HEIGHT-220, 80, 170};
	SDL_RenderFillRect(ren.get(),&d);
	//окно
	SDL_SetRenderDrawColor(ren.get(), 182, 220, 221, 150);
	SDL_Rect o{WIDTH/2+20, HEIGHT-220, 150, 100};
	SDL_RenderFillRect(ren.get(),&o);

	//трава
	render_bird();
	render_cat();

	render_omlet();
	SDL_RenderPresent(ren.get());
}
void main_loop()
{
	SDL_Event event;
	auto keys = SDL_GetKeyboardState(nullptr);
	for(;;) {
		// Обработка событий
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT: return;
			default: ;
			}
		}
		bird_x += 4;
		// Изменение состояния
		if (keys[SDL_SCANCODE_LEFT]) bird_x -= 4;
		if (keys[SDL_SCANCODE_RIGHT]) bird_x += 4;
		if (keys[SDL_SCANCODE_UP]) bird_y -= 4;
		if (keys[SDL_SCANCODE_DOWN]) bird_y += 4;

		if (bird_y < 0)
			bird_y = 0;
		if(bird_y >= 150)
			bird_y = 149;
		if(bird_x >= WIDTH)
			bird_x = -64;

		//КОТ
		if (keys[SDL_SCANCODE_Z]) { cat_x -= 6; cat_rig =false; }
		if (keys[SDL_SCANCODE_X]) { cat_x += 6; cat_rig =true; }
		if (keys[SDL_SCANCODE_S]&&!cat_jump)
		{
			cat_jump = true;
			cat_dy = -16;
		}
		if(cat_jump)
		{
			cat_y += cat_dy;
			cat_dy++;
		}
		if(cat_y>350)
		{
			cat_y=350;
			cat_jump=false;
		}
		if(cat_x >= WIDTH)
			cat_x = -64;
		// Рисование
		render_background();
		render_scene();
		SDL_RenderPresent(ren.get());
	}
}
int main(int,  char**){
	try {
			if (SDL_Init(SDL_INIT_EVERYTHING) != 0) SDL_DIE("SDL_Init()");
			win = WindowPtr(
					SDL_CreateWindow(
							"SDL window",
							SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
							WIDTH, HEIGHT,
							SDL_WINDOW_SHOWN),
					SDL_DestroyWindow);
			if (win == nullptr) SDL_DIE("SDL_CreateWindow()");
			ren = RendererPtr(
					SDL_CreateRenderer(win.get(), -1,
							SDL_RENDERER_ACCELERATED |
							SDL_RENDERER_PRESENTVSYNC),
					SDL_DestroyRenderer);
			if (ren == nullptr) SDL_DIE("SDL_CreateRenderer()");
			// Тут будет загрузка всяких ресурсов
			bird = load_texture("bird1.png");
			cat = load_texture("cat1.png");
			egg1 = load_texture("eg1.png");
			egg2 = load_texture("eg2.png");
			main_loop();
		} catch (const std::exception& e) {
			std::cerr << "Произошла ошибка:\n" <<
					e.what() << std::endl;
			return 1;
		} catch (...) {
			std::cerr << "Произошла какая-то ошибка." << std::endl;
			return 2;
		}
		return 0;
}
