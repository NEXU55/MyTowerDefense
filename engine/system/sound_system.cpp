#include"sound_system.h"
#include"../core/context.h"
#include"../core/event_bus.h"
#include"../audio/audio_player.h"
#include"../resource/resource_manager.h"

#include"../event/play_sound_event.h"

namespace engine::system
{
	void SoundSystem::init(Context& context)
	{
		auto& event_bus = context.get_eventbus();
		auto& audio_player = context.get_audio_player();
		auto& resource_manager = context.get_resource_manager();

		auto p=event_bus.subscribe<engine::event::PlaySoundEventEvent>(
			[&audio_player, &resource_manager](const engine::event::PlaySoundEventEvent& event)
			{
				audio_player.play_sound(resource_manager.get_audio_name(event.type,event.name));
			});
		events_.emplace_back(p);
	}
}