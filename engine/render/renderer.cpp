#include "renderer.h"
#include "../resource/resource_manager.h"
#include <stdexcept> // For std::runtime_error
#include <spdlog/spdlog.h>
#include <SDL.h>

namespace engine::render
{
	Renderer::Renderer(SDL_Renderer* renderer, ResourceManager* resource_manager)
		:renderer_(renderer)
		, resource_manager_(resource_manager)
	{
		if (!renderer_ || !resource_manager_)
			throw std::runtime_error("用于创建Renderer的指针为空");
	}

	void Renderer::draw_sprite(SDL_Texture* texture,const SDL_Rect& src_rect,const glm::ivec2& position, double angle)
	{
		if (!texture) 
		{
			spdlog::error("纹理为空");
			return;
		}
		SDL_Rect dest_rect = { position.x, position.y, src_rect.w, src_rect.h };
		if (SDL_RenderCopyEx(renderer_, texture, &src_rect, &dest_rect,angle, nullptr, SDL_RendererFlip::SDL_FLIP_NONE)!=0)
			spdlog::error("渲染失败: {}",SDL_GetError());
	}

	void Renderer::draw_ui_fill_rect(const engine::ui::Rect& rect,const SDL_Color& color)
	{
		SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer_,color.r, color.g, color.b, color.a);
		SDL_Rect sdl_rect = { rect.position.x, rect.position.y, rect.size.x, rect.size.y };
		if (SDL_RenderFillRect(renderer_, &sdl_rect)!=0) 
		{
			spdlog::error("绘制填充矩形失败：{}", SDL_GetError());
		}
		SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
	}

	void Renderer::present()
	{
		SDL_RenderPresent(renderer_);
	}

	void Renderer::clear_screen()
	{
		if (SDL_RenderClear(renderer_)!=0)
			spdlog::error("清除渲染器失败：{}", SDL_GetError());
	}
	glm::dvec2 Renderer::get_logical_size() const
	{
		int width, height;
		SDL_RenderGetLogicalSize(renderer_, &width, &height);
		return glm::dvec2(width, height);
	}
}