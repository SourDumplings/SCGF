#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <litehtml/litehtml.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <filesystem>
#include <unordered_map>
#include "util/SDL_helper.h"
#include "util/file_helper.h"

#define WINDOW_WIDTH  1366
#define WINDOW_HEIGHT 768

class SDLContainer : public litehtml::document_container
{
public:
	SDLContainer(SDL_Renderer* renderer)
		: renderer(renderer)
	{
		if (TTF_Init() == -1) { std::cerr << "TTF_Init Error: " << SDL_GetError() << std::endl; }
		engine_surface = TTF_CreateSurfaceTextEngine();
		if (!engine_surface)
		{
			SDL_Log("Couldn't create surface text engine: %s\n", SDL_GetError());
		}
	}

	~SDLContainer()
	{
		for (auto& font : fonts)
		{
			TTF_CloseFont(font.second);
		}
		TTF_Quit();
	}

	litehtml::uint_ptr create_font(
		const char* faceName,
		int size,
		int weight,
		litehtml::font_style italic,
		unsigned int decoration,
		litehtml::font_metrics* fm
	) override
	{
		std::filesystem::path cwd	   = scgf::util::get_executable_dir();
		std::filesystem::path fontPath = cwd / std::filesystem::path("../res/dist/font/Jinglei.ttf");

		TTF_Font* font = TTF_OpenFont(fontPath.string().c_str(), size);
		if (!font)
		{
			std::cerr << "TTF_OpenFont Error: " << SDL_GetError() << std::endl;
			return 0;
		}

		fonts[font_counter] = font;

		if (fm)
		{
			fm->ascent	 = TTF_GetFontAscent(font);
			fm->descent	 = TTF_GetFontDescent(font);
			fm->height	 = TTF_GetFontHeight(font);
			fm->x_height = fm->height / 2;	// 简单估计
		}

		return font_counter++;
	}

	void delete_font(litehtml::uint_ptr hFont) override
	{
		auto it = fonts.find(hFont);
		if (it != fonts.end()) { fonts.erase(it); }
	}

	int text_width(const char* text, litehtml::uint_ptr hFont) override
	{
		auto it = fonts.find(hFont);
		if (it == fonts.end()) return 0;

		int w = 0, h = 0;
		TTF_Text* text_obj = NULL;
		text_obj		   = TTF_CreateText(engine_surface, it->second, text, 0);
		TTF_GetTextSize(text_obj, &w, &h);
		TTF_DestroyText(text_obj);
		text_obj = NULL;
		return w;
	}

	void draw_text(
		litehtml::uint_ptr hdc,
		const char* text,
		litehtml::uint_ptr hFont,
		litehtml::web_color color,
		const litehtml::position& pos
	) override
	{
		auto it = fonts.find(hFont);
		if (it == fonts.end()) return;

		SDL_Color sdl_color	 = {color.red, color.green, color.blue, color.alpha};
		SDL_Surface* surface = TTF_RenderText_Blended(it->second, text, 0, sdl_color);
		if (!surface)
		{
			std::cerr << "TTF_RenderText_Blended Error: " << SDL_GetError() << std::endl;
			return;
		}

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		if (!texture)
		{
			std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
			SDL_DestroySurface(surface);
			return;
		}

		SDL_FRect dst_rect = {
			static_cast<float>(pos.x),
			static_cast<float>(pos.y),
			static_cast<float>(surface->w),
			static_cast<float>(surface->h)
		};
		SDL_RenderTexture(renderer, texture, nullptr, &dst_rect);

		SDL_DestroyTexture(texture);
		SDL_DestroySurface(surface);
	}

	int pt_to_px(int pt) const override { return pt; }

	int get_default_font_size() const override { return 16; }

	const char* get_default_font_name() const override { return "simhei"; }

	void draw_list_marker(litehtml::uint_ptr hdc, const litehtml::list_marker& marker) override
	{
		// 实现列表标记绘制逻辑
	}

	void load_image(const char* src, const char* baseurl, bool redraw_on_ready) override
	{
		std::string full_path;
		if (baseurl) { full_path = std::string(baseurl) + "/" + src; }
		else { full_path = base_url + "/" + src; }

		std::cout << "Loading image: " << full_path << std::endl;

		SDL_Surface* surface = scgf::util::load_image(full_path.c_str());
		if (!surface)
		{
			std::cerr << "Failed to load image: " << full_path << std::endl;
			return;
		}

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		if (!texture)
		{
			std::cerr << "Failed to create texture from surface: " << full_path << std::endl;
			SDL_DestroySurface(surface);
			return;
		}

		images[src] = texture;
		SDL_DestroySurface(surface);
		std::cout << "Image loaded: " << full_path << std::endl;
	}

	void get_image_size(const char* src, const char* baseurl, litehtml::size& sz) override
	{
		auto it = images.find(src);
		if (it != images.end())
		{
			float w, h;
			SDL_GetTextureSize(it->second, &w, &h);
			sz.width  = w;
			sz.height = h;
			// std::cout << "Image size: " << w << "x" << h << std::endl;
		}
		else
		{
			sz.width  = 0;
			sz.height = 0;
			std::cerr << "Image not found: " << src << std::endl;
		}
	}

	void draw_image(
		litehtml::uint_ptr hdc,
		const litehtml::background_layer& layer,
		const std::string& url,
		const std::string& base_url
	) override
	{
		auto it = images.find(url);
		if (it == images.end())
		{
			std::cerr << "Image not found for drawing: " << url << std::endl;
			return;
		}

		float w, h;
		SDL_GetTextureSize(it->second, &w, &h);

		// 获取图片元素
		auto element = doc->root()->select_one("img[src='" + url + "']");
		if (!element)
		{
			std::cerr << "Image element not found: " << url << std::endl;
			return;
		}

		// 从自定义属性中获取位置
		int x = std::stoi(element->get_attr("data-x", "0"));
		int y = std::stoi(element->get_attr("data-y", "0"));

		SDL_FRect dst_rect = {
			static_cast<float>(x),
			static_cast<float>(y),
			static_cast<float>(w),
			static_cast<float>(h)
		};
		SDL_RenderTexture(renderer, it->second, nullptr, &dst_rect);
		// std::cout << "Image drawn at: " << x << ", " << y << std::endl;
	}

	void draw_solid_fill(
		litehtml::uint_ptr hdc,
		const litehtml::background_layer& layer,
		const litehtml::web_color& color
	) override
	{
		SDL_Color sdl_color = {color.red, color.green, color.blue, color.alpha};
		SDL_SetRenderDrawColor(renderer, sdl_color.r, sdl_color.g, sdl_color.b, sdl_color.a);
		SDL_FRect rect = {
			static_cast<float>(layer.border_box.x),
			static_cast<float>(layer.border_box.y),
			static_cast<float>(layer.border_box.width),
			static_cast<float>(layer.border_box.height)
		};
		SDL_RenderFillRect(renderer, &rect);
	}

	void draw_linear_gradient(
		litehtml::uint_ptr hdc,
		const litehtml::background_layer& layer,
		const litehtml::background_layer::linear_gradient& gradient
	) override
	{
		// 实现线性渐变绘制逻辑
	}

	void draw_radial_gradient(
		litehtml::uint_ptr hdc,
		const litehtml::background_layer& layer,
		const litehtml::background_layer::radial_gradient& gradient
	) override
	{
		// 实现径向渐变绘制逻辑
	}

	void draw_conic_gradient(
		litehtml::uint_ptr hdc,
		const litehtml::background_layer& layer,
		const litehtml::background_layer::conic_gradient& gradient
	) override
	{
		// 实现圆锥渐变绘制逻辑
	}

	void draw_borders(
		litehtml::uint_ptr hdc,
		const litehtml::borders& borders,
		const litehtml::position& draw_pos,
		bool root
	) override
	{
		// 实现边框绘制逻辑
	}

	void set_caption(const char* caption) override { SDL_SetWindowTitle(window, caption); }

	void set_base_url(const char* base_url) override { this->base_url = base_url; }

	void link(const std::shared_ptr<litehtml::document>& doc, const litehtml::element::ptr& el)
		override
	{
		// 实现链接逻辑
	}

	void on_anchor_click(const char* url, const litehtml::element::ptr& el) override
	{
		// 实现锚点点击逻辑
	}

	void on_mouse_event(const litehtml::element::ptr& el, litehtml::mouse_event event) override
	{
		// 实现鼠标事件逻辑
	}

	void set_cursor(const char* cursor) override
	{
		// 实现设置光标逻辑
	}

	void transform_text(litehtml::string& text, litehtml::text_transform tt) override
	{
		// 实现文本转换逻辑
	}

	void import_css(litehtml::string& text, const litehtml::string& url, litehtml::string& baseurl)
		override
	{
		// 实现 CSS 导入逻辑
	}

	void set_clip(const litehtml::position& pos, const litehtml::border_radiuses& bdr_radius)
		override
	{
		// 实现设置剪辑区域逻辑
	}

	void del_clip() override
	{
		// 实现删除剪辑区域逻辑
	}

	void get_client_rect(litehtml::position& client) const override
	{
		client.x	  = 0;
		client.y	  = 0;
		client.width  = WINDOW_WIDTH;
		client.height = WINDOW_HEIGHT;
	}

	litehtml::element::ptr create_element(
		const char* tag_name,
		const litehtml::string_map& attributes,
		const std::shared_ptr<litehtml::document>& doc
	) override
	{
		return nullptr;
	}

	void get_media_features(litehtml::media_features& media) const override
	{
		media.type			= litehtml::media_type_screen;
		media.width			= WINDOW_WIDTH;
		media.height		= WINDOW_HEIGHT;
		media.device_width	= WINDOW_WIDTH;
		media.device_height = WINDOW_HEIGHT;
		media.color			= 8;
		media.monochrome	= 0;
		media.color_index	= 256;
		media.resolution	= 96;
	}

	void get_language(litehtml::string& language, litehtml::string& culture) const override
	{
		// language = "en";
		// culture	 = "US";

		language = "zh";
		culture	 = "CN";
	}

	void set_window(SDL_Window* window) { this->window = window; }

	void set_document(litehtml::document::ptr doc) { this->doc = doc; }

private:
	SDL_Renderer* renderer;
	SDL_Window* window;
	std::string base_url;
	std::unordered_map<litehtml::uint_ptr, TTF_Font*> fonts;
	litehtml::uint_ptr font_counter = 1;
	std::unordered_map<std::string, SDL_Texture*> images;
	litehtml::document::ptr doc;
	TTF_TextEngine* engine_surface = NULL;
};

void render_gameplay_content(SDL_Renderer* renderer, SDL_Texture* texture, int w, int h)
{
	SDL_FRect dstRect1;
	dstRect1.x = 550;
	dstRect1.y = 50;
	dstRect1.w = w;
	dstRect1.h = h;

	SDL_RenderTexture(renderer, texture, nullptr, &dstRect1);

	SDL_FRect dstRect2;
	dstRect2.x = 700;
	dstRect2.y = 200;
	dstRect2.w = w;
	dstRect2.h = h;

	SDL_RenderTexture(renderer, texture, nullptr, &dstRect2);
}

class EventHandler
{
public:
	EventHandler(SDLContainer* container, litehtml::document::ptr doc)
		: container(container)
		, doc(doc)
	{
	}

	void handle_event(const SDL_Event& event)
	{
		if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
		{
			int x = event.button.x;
			int y = event.button.y;

			litehtml::position::vector redraw_boxes;
			doc->on_lbutton_down(x, y, x, y, redraw_boxes);

			// 检查是否点击了按钮
			auto element = doc->root()->select_one("#button");
			if (element && is_point_inside(element, x, y))
			{
				std::cout << "Button clicked!" << std::endl;
			}
		}
	}

private:
	SDLContainer* container;
	litehtml::document::ptr doc;

	bool is_point_inside(const litehtml::element::ptr& element, int x, int y)
	{
		litehtml::position pos = element->get_placement();
		return x >= pos.left() && x <= pos.right() && y >= pos.top() && y <= pos.bottom();
	}
};

std::string read_file_to_string(const std::string& file_path)
{
	std::ifstream file(file_path);
	if (!file.is_open())
	{
		std::cerr << "Failed to open file: " << file_path << std::endl;
		return "";
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

int test_litehtml()
{
	// 初始化 SDL
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return -1;
	}

	SDL_Window* window =
		SDL_CreateWindow("litehtml with SDL", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
	if (!window)
	{
		std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		return -1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, "opengl");
	if (!renderer)
	{
		std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	// 初始化 litehtml
	SDLContainer container(renderer);
	container.set_window(window);
	container.set_base_url("../res/dist/sprite");	// 设置图片的基路径

	// 读取 HTML 文件内容
	std::string html = read_file_to_string("../res/dist/layout/test_litehtml.html");
	if (html.empty())
	{
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
		return -1;
	}

	litehtml::document::ptr doc = litehtml::document::createFromString(html.c_str(), &container);
	container.set_document(doc);

	// 创建事件处理器
	EventHandler event_handler(&container, doc);

	SDL_Surface* surface = scgf::util::load_image("../res/dist/sprite/bird.png");
	int w				 = surface->w;
	int h				 = surface->h;

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_DestroySurface(surface);

	// 主循环
	bool running = true;
	SDL_Event event;
	while (running)
	{
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT) { running = false; }
			event_handler.handle_event(event);
		}

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		// 渲染 Gameplay 内容
		render_gameplay_content(renderer, texture, w, h);

		// 渲染 litehtml 文档
		int best_width = doc->render(WINDOW_WIDTH);
		if (best_width < WINDOW_WIDTH) { doc->render(best_width); }
		doc->draw((litehtml::uint_ptr)renderer, 0, 0, nullptr);

		SDL_RenderPresent(renderer);
	}

	// 清理
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}