#include <Aero.hpp>
#include <Scripting/ScriptBinding/ScriptBinding.hpp>
#include <Scripting/ScriptBinding/ScriptBindingHelpers.hpp>

namespace ag {

void ScriptBinding::register_audio_functions() {
  auto& lua         = ScriptManager::get_lua();
  auto  entity_type = lua["Entity"].get<sol::usertype<Entity>>();

  entity_type.set_function("play_audio", [](Entity& entity) {
    if (detail::has_component<Audio_Component>(entity)) {
      entity.get_component<Audio_Component>().source->play();
    }
  });

  entity_type.set_function("pause_audio", [](Entity& entity) {
    if (detail::has_component<Audio_Component>(entity)) {
      entity.get_component<Audio_Component>().source->pause();
    }
  });

  entity_type.set_function("stop_audio", [](Entity& entity) {
    if (detail::has_component<Audio_Component>(entity)) {
      entity.get_component<Audio_Component>().source->stop();
    }
  });

  entity_type.set_function("is_playing_audio", [](Entity& entity) -> bool {
    if (detail::has_component<Audio_Component>(entity)) {
      return entity.get_component<Audio_Component>().source->is_playing();
    }
    return false;
  });

  entity_type.set_function("is_paused_audio", [](Entity& entity) -> bool {
    if (detail::has_component<Audio_Component>(entity)) {
      return entity.get_component<Audio_Component>().source->is_paused();
    }
    return false;
  });

  entity_type.set_function("is_loop_audio", [](Entity& entity) -> bool {
    if (detail::has_component<Audio_Component>(entity)) {
      return entity.get_component<Audio_Component>().source->is_looping();
    }
    return false;
  });

  entity_type.set_function("set_loop_audio", [](Entity& entity, bool loop) {
    if (detail::has_component<Audio_Component>(entity)) {
      entity.get_component<Audio_Component>().source->set_loop(loop);
    }
  });

  entity_type.set_function("set_pitch_audio", [](Entity& entity, float pitch) {
    if (detail::has_component<Audio_Component>(entity)) {
      entity.get_component<Audio_Component>().source->set_pitch(pitch);
    }
  });

  entity_type.set_function("get_pitch_audio", [](Entity& entity) -> float {
    if (detail::has_component<Audio_Component>(entity)) {
      return entity.get_component<Audio_Component>().source->get_pitch();
    }
    return 1.0f;
  });

  entity_type.set_function("set_volume_audio", [](Entity& entity, float volume) {
    if (detail::has_component<Audio_Component>(entity)) {
      entity.get_component<Audio_Component>().source->set_volume(volume);
    }
  });

  entity_type.set_function("get_volume_audio", [](Entity& entity) -> float {
    if (detail::has_component<Audio_Component>(entity)) {
      return entity.get_component<Audio_Component>().source->get_volume();
    }
    return 1.0f;
  });
}

}  // namespace ag
