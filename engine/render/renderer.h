#pragma once
#include"../ui/rect.h"
#include <glm/vec2.hpp>

typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Rect SDL_Rect;
typedef struct SDL_Color SDL_Color;

namespace engine::resource 
{
	class ResourceManager;
}

namespace engine::render
{
	class Renderer final
	{
		using ResourceManager = engine::resource::ResourceManager;
	private:
		SDL_Renderer* renderer_ = nullptr;
		ResourceManager* resource_manager_ = nullptr;
	public:
		//包装 SDL_Renderer 并提供清除屏幕、绘制精灵和呈现最终图像的方法。
		//在构造时初始化。依赖于一个有效的 SDL_Renderer 和 ResourceManager。
		//构造失败会抛出异常。
		Renderer(SDL_Renderer* renderer, ResourceManager* resource_manager);
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer(Renderer&&) = delete;
		Renderer& operator=(Renderer&&) = delete;
		~Renderer() = default;

	    //绘制一个精灵
		//世界坐标中的左上角位置。
		//旋转角度
		void draw_sprite(SDL_Texture* texture, const SDL_Rect& src_rect, const glm::ivec2& position, double angle);
		void draw_ui_fill_rect(const engine::ui::Rect& rect,const SDL_Color& color);

		void present();//更新屏幕
		void clear_screen();//清理屏幕

		SDL_Renderer* get_sdl_renderer() { return renderer_; }
		glm::dvec2 get_logical_size() const;
	}; 
}