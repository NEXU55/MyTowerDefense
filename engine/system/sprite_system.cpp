#include"sprite_system.h"
#include"../core/context.h"
#include"../core/coordinator.h"
#include"../component/component_manager.h"
#include"../component/sprite_component.h"
#include"../component/transform_component.h"
#include"../render/renderer.h"

namespace engine::system
{
	void SpriteSystem::init(Context& context)
	{
		auto& coordinator = context.get_coordinator();

		auto sprite_type = coordinator.get_component_type<SpriteComponent>();
		auto transform_type = coordinator.get_component_type<TransformComponent>();
		signature_.set(sprite_type, true);
		signature_.set(transform_type, true);
	}

	void SpriteSystem::render(Context& context)
	{
		constexpr int size = ((int)SpriteComponent::SpriteLayer::None - (int)SpriteComponent::SpriteLayer::Tile);
		std::array<std::vector<Entity>,size> render_list;

		auto& coordinator = context.get_coordinator();
		auto& renderer = context.get_renderer();
		auto entity_list = coordinator.view(signature_);
		for (auto entity : entity_list)
		{
			auto& sprite = coordinator.get_component<SpriteComponent>(entity);
			int index = (int)sprite.layer;
			if (index >= 0 && index < size)
				render_list[index].emplace_back(entity);
		}
		for (int i = 0; i < size; ++i)
		{
			for (auto entity : render_list[i])
			{
				auto& sprite = coordinator.get_component<SpriteComponent>(entity);
				auto& transform = coordinator.get_component<TransformComponent>(entity);
				if (!sprite.texture)
				{
					spdlog::error("该实体的精灵的纹理指针为空，{}",entity);
					continue;
				}
					
				switch (sprite.mode)
				{
				case SpriteComponent::AnchorMode::NONE:
					renderer.draw_sprite(sprite.texture, sprite.src_rect, transform.position, transform.angle);
					break;
				case SpriteComponent::AnchorMode::CENTER:
					glm::ivec2 position = { transform.position.x - sprite.size.x / 2,transform.position.y - sprite.size.y / 2 };
					renderer.draw_sprite(sprite.texture, sprite.src_rect,position, transform.angle);
					break;
				}
			}
		}
	}
}
