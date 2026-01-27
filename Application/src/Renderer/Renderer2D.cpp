#include <Renderer/Renderer2D.hpp>
#include <array>
#include <Renderer/Renderer.hpp>

#include <glm/glm.hpp>

#include <Renderer/VertexArray.hpp>
#include <Renderer/VertexBuffer.hpp>
#include <Renderer/IndexBuffer.hpp>
#include <Renderer/Shader.hpp>
#include <Renderer/Text.hpp>

#include <Math/Math.hpp>
#include <Renderer/Color.hpp>
#include <Scene/SceneComponent.hpp>
#include <GameObjects/Components/Components.hpp>
#include <Core/Application.hpp>
#include <cmath>

namespace ag
{
	enum class Quad_Type
	{
		Rectangle = 0,
		Circle = 1,
		Sprite = 2,
		Text = 3
	};

	struct Quad_Instance
	{
		vec2f size;
		vec2f position;
		vec2f origin;
		float rotation;
		int mode;
		int quad_mode;
		int texture_slot;

		float border_thickness;
		vec4f fill_color;
		vec4f border_color;

		float corner_radius;

		vec2f texture_size;
		vec4f texture_rect;
		vec2f flip;
		int entity_id;
	};

	constexpr int TEXTURE_TEXT = 0;
	constexpr int TEXTURE_SCENE = 2;
	constexpr int TEXTURE_SPRITE = 1;

	struct Renderer2D_Data
	{
		// Max Shapes Data
		AG_uint max_shape = 0;
		AG_uint max_vertices = 0;
		AG_uint max_indices = 0;

		// Quad Data
		AG_ref<VertexArray> quad_vertex_array;
		AG_ref<VertexBuffer> quad_instanced_buffer;
		AG_ref<Shader> quad_shader;
		AG_ref<Texture> quad_texture, text_texture;

		AG_ref<VertexArray> fullscreen_vertex_array;
		AG_ref<VertexBuffer> fullscreen_instanced_buffer;
		AG_ref<Shader> fullscreen_shader;

		View view;
		vec2f viewport_size;
		AG_uint slots;

		std::unique_ptr<Renderer2D::AsyncData> async_data;
	};

	static Renderer2D_Data* s_data = nullptr;

	void Renderer2D::init()
	{
		s_data = new Renderer2D_Data();
		s_data->max_shape = 100000;  // Large batch size
		s_data->max_vertices = s_data->max_shape * 4;
		s_data->max_indices = s_data->max_shape * 6;
		s_data->slots = 3;

		AG_uint indices[] = { 0, 1, 2, 0, 2, 3 };
		AG_ref<IndexBuffer> indexbuffer = ag::IndexBuffer::create(indices, 6);

		float vertices[] = {
			0.f, 0.f,
			0.f, 1.f,
			1.f, 1.f,
			1.f, 0.f
		};

		// Fullscreen quad setup
		{
			float full_vertices[] = {
				-1.f, -1.f, 0.f, 0.f,
				-1.f, 1.f, 0.f, 1.f,
				1.f, 1.f, 1.f, 1.f,
				1.f, -1.f, 1.f, 0.f
			};

			BufferLayout layout = {
				{ShaderDataType::Float2, "a_Position"},
				{ShaderDataType::Float2, "a_texcoord"}
			};

			s_data->fullscreen_vertex_array = VertexArray::create();
			auto fullscreen_vertex_buffer = VertexBuffer::create(full_vertices, sizeof(full_vertices));
			fullscreen_vertex_buffer->set_layout(layout);
			s_data->fullscreen_vertex_array->add_vertex_buffer(fullscreen_vertex_buffer);
			s_data->fullscreen_vertex_array->set_index_buffer(indexbuffer);
			s_data->fullscreen_shader = Shader::create("assets/shaders/Sprite2D.glsl");
		}

		// Quad setup
		{
			s_data->quad_vertex_array = ag::VertexArray::create();

			BufferLayout layout = {
				{ShaderDataType::Float2, "a_Position"}
			};

			auto quad_vertex_buffer = ag::VertexBuffer::create(vertices, sizeof(vertices));
			quad_vertex_buffer->set_layout(layout);
			s_data->quad_vertex_array->add_vertex_buffer(quad_vertex_buffer);

			BufferLayout instance_layout = {
				{ShaderDataType::Float2, "a_InstanceSize"},
				{ShaderDataType::Float2, "a_InstancePos"},
				{ShaderDataType::Float2, "a_InstanceOrigin"},
				{ShaderDataType::Float, "a_InstanceRotation"},
				{ShaderDataType::Int, "a_RenderMode"},
				{ShaderDataType::Int, "a_QuadMode"},
				{ShaderDataType::Int, "a_TextureSlot"},
				{ShaderDataType::Float, "a_BorderThickness"},
				{ShaderDataType::Float4, "a_FillColor"},
				{ShaderDataType::Float4, "a_BorderColor"},
				{ShaderDataType::Float, "a_CornerRadius"},
				{ShaderDataType::Float2, "a_Texture_Size"},
				{ShaderDataType::Float4, "a_TextureRect"},
				{ShaderDataType::Float2, "a_flip"},
				{ShaderDataType::Int, "a_entity_id"},
			};

			s_data->quad_instanced_buffer = VertexBuffer::create(nullptr, sizeof(Quad_Instance) * s_data->max_shape);
			s_data->quad_instanced_buffer->set_layout(instance_layout);
			s_data->quad_vertex_array->add_vertex_buffer(s_data->quad_instanced_buffer, true);
			s_data->quad_vertex_array->set_index_buffer(indexbuffer);
			s_data->quad_shader = ag::Shader::create("assets/shaders/Quad.glsl");

			s_data->quad_texture = ag::Texture2D::create("assets/textures/default.png", false);
			s_data->text_texture = ag::Texture2D::create("assets/textures/atlas.png", false);
			TextLoader::loadGlyph("assets/textures/atlas.json");
		}

		// Initialize async system with double buffering
		s_data->async_data = std::make_unique<AsyncData>();

		// OPTIMIZATION: Use aligned allocation for better cache performance
#ifdef _MSC_VER
		s_data->async_data->batch_a.instances = (Quad_Instance*)_aligned_malloc(sizeof(Quad_Instance) * s_data->max_shape, 64);
		s_data->async_data->batch_b.instances = (Quad_Instance*)_aligned_malloc(sizeof(Quad_Instance) * s_data->max_shape, 64);
#else
		s_data->async_data->batch_a.instances = (Quad_Instance*)aligned_alloc(64, sizeof(Quad_Instance) * s_data->max_shape);
		s_data->async_data->batch_b.instances = (Quad_Instance*)aligned_alloc(64, sizeof(Quad_Instance) * s_data->max_shape);
#endif

		s_data->async_data->batch_a.capacity = s_data->max_shape;
		s_data->async_data->batch_a.count = 0;
		s_data->async_data->batch_b.capacity = s_data->max_shape;
		s_data->async_data->batch_b.count = 0;

		s_data->async_data->current_batch = &s_data->async_data->batch_a;

		// Start worker thread
		s_data->async_data->worker_thread = std::thread(worker_thread_function);
	}

	void Renderer2D::shut_down()
	{
		if (s_data)
		{
			if (s_data->async_data)
			{
				// Signal shutdown
				s_data->async_data->running = false;
				s_data->async_data->command_cv.notify_one();

				// Wait for worker
				if (s_data->async_data->worker_thread.joinable())
				{
					s_data->async_data->worker_thread.join();
				}

				// Free aligned batch arrays
#ifdef _MSC_VER
				_aligned_free(s_data->async_data->batch_a.instances);
				_aligned_free(s_data->async_data->batch_b.instances);
#else
				free(s_data->async_data->batch_a.instances);
				free(s_data->async_data->batch_b.instances);
#endif
			}

			delete s_data;
			s_data = nullptr;
		}
	}

	void Renderer2D::begin_scene(const View& view, const vec2f& viewport_size)
	{
		Renderer::begin_scene(view, viewport_size);
		s_data->view = view;
		s_data->viewport_size = viewport_size;
		Renderer::enable_blend();
	}

	void Renderer2D::begin_scene()
	{
		// Don't flush on begin - let commands accumulate
	}

	void Renderer2D::end_scene()
	{
		
		s_data->async_data->command_cv.notify_one();
		flush();
	}

	void Renderer2D::start_batch()
	{
		s_data->async_data->command_cv.notify_one();
		flush();
	}

	void Renderer2D::set_texture(const AG_ref<Texture>& texture)
	{
		if (s_data->quad_texture == texture)
			return;

		flush();
		s_data->quad_texture = texture;
	}

	void Renderer2D::draw_fullscreen_quad(AG_uint id)
	{
		Renderer::disable_blend();
		Renderer::bind(id, TEXTURE_SCENE);
		s_data->fullscreen_shader->bind();
		s_data->fullscreen_shader->set_int("u_texture", TEXTURE_SCENE);
		Renderer::submit(s_data->fullscreen_vertex_array);
	}

	
	void Renderer2D::draw_rectangle(const Rectangle& rect, const Transform_Component& transform, int entity_id)
	{
		{
			std::lock_guard<std::mutex> lock(s_data->async_data->command_mutex);
			s_data->async_data->command_queue.push(RectangleCommand{ rect, transform, entity_id });
			s_data->async_data->total_commands++;
		}

		// OPTIMIZATION: Notify every 1000 commands instead of 500
		static thread_local int notify_counter = 0;
		if (++notify_counter >= 1000)
		{
			s_data->async_data->command_cv.notify_one();
			notify_counter = 0;
		}
	}

	void Renderer2D::draw_circle(const Circle& circle, const Transform_Component& transform, int entity_id)
	{
		{
			std::lock_guard<std::mutex> lock(s_data->async_data->command_mutex);
			s_data->async_data->command_queue.push(CircleCommand{ circle, transform, entity_id });
			s_data->async_data->total_commands++;
		}

		static thread_local int notify_counter = 0;
		if (++notify_counter >= 1000)
		{
			s_data->async_data->command_cv.notify_one();
			notify_counter = 0;
		}
	}

	void Renderer2D::draw_sprite(const Sprite& sprite, const Transform_Component& transform, int entity_id)
	{
		vec2f texture_size = { 0, 0 };
		if (s_data->quad_texture)
		{
			texture_size = s_data->quad_texture->get_size();
		}

		{
			std::lock_guard<std::mutex> lock(s_data->async_data->command_mutex);
			s_data->async_data->command_queue.push(SpriteCommand{ sprite, transform, entity_id, texture_size });
			s_data->async_data->total_commands++;
		}

		static thread_local int notify_counter = 0;
		if (++notify_counter >= 1000)
		{
			s_data->async_data->command_cv.notify_one();
			notify_counter = 0;
		}
	}

	void Renderer2D::draw_text(const Text& text_string, const Transform_Component& transform, int entity_id)
	{
		vec2f longest_size = Text::calc_longest_size(text_string, transform.scale);

		{
			std::lock_guard<std::mutex> lock(s_data->async_data->command_mutex);
			s_data->async_data->command_queue.push(TextCommand{
				text_string,
				transform,
				entity_id,
				longest_size,
				s_data->viewport_size,
				s_data->view
				});
			s_data->async_data->total_commands++;
		}

		static thread_local int notify_counter = 0;
		if (++notify_counter >= 1000)
		{
			s_data->async_data->command_cv.notify_one();
			notify_counter = 0;
		}
	}

	void Renderer2D::flush()
	{
		if (s_data->async_data->total_commands == 0)
			return;

		{
			std::lock_guard<std::mutex> lock(s_data->async_data->command_mutex);
			s_data->async_data->flush_requested = true;
		}
		s_data->async_data->command_cv.notify_one();

		// Wait for processed batch with timeout
		ProcessedBatch* batch = nullptr;
		{
			std::unique_lock<std::mutex> lock(s_data->async_data->processed_mutex);

			// SAFETY: Add timeout to detect deadlocks
			bool got_batch = s_data->async_data->processed_cv.wait_for(
				lock,
				std::chrono::seconds(5),
				[] { return s_data->async_data->ready_batch.load() != nullptr; }
			);

			if (!got_batch)
			{
				// Timeout - worker thread might be stuck
				return;
			}

			batch = s_data->async_data->ready_batch.exchange(nullptr);
		}

		flush_internal(batch);
		s_data->async_data->total_commands = 0;
	}

	// FULLY OPTIMIZED WORKER THREAD
	void Renderer2D::worker_thread_function()
	{
		ProcessedBatch* working_batch = &s_data->async_data->batch_a;
		ProcessedBatch* standby_batch = &s_data->async_data->batch_b;

		// Large local buffer to minimize lock contention
		std::vector<DrawCommand> local_commands;
		local_commands.reserve(100000);

		while (s_data->async_data->running)
		{
			std::unique_lock<std::mutex> lock(s_data->async_data->command_mutex);

			// OPTIMIZATION: Use wait_for with short timeout instead of blocking forever
			s_data->async_data->command_cv.wait_for(
				lock,
				std::chrono::milliseconds(1),
				[] {
					return !s_data->async_data->command_queue.empty() ||
						s_data->async_data->flush_requested ||
						!s_data->async_data->running;
				}
			);

			if (!s_data->async_data->running)
				break;

			// CRITICAL OPTIMIZATION: Copy ALL commands at once
			local_commands.clear();
			while (!s_data->async_data->command_queue.empty())
			{
				local_commands.push_back(std::move(s_data->async_data->command_queue.front()));
				s_data->async_data->command_queue.pop();
			}

			bool should_flush = s_data->async_data->flush_requested;
			s_data->async_data->flush_requested = false;

			lock.unlock();  // UNLOCK IMMEDIATELY - critical for performance

			// Process all commands WITHOUT holding lock
			for (auto& cmd : local_commands)
			{
				std::visit([working_batch](auto&& arg) {
					using T = std::decay_t<decltype(arg)>;
					if constexpr (std::is_same_v<T, RectangleCommand>)
						process_rectangle(arg, *working_batch);
					else if constexpr (std::is_same_v<T, CircleCommand>)
						process_circle(arg, *working_batch);
					else if constexpr (std::is_same_v<T, SpriteCommand>)
						process_sprite(arg, *working_batch);
					else if constexpr (std::is_same_v<T, TextCommand>)
						process_text(arg, *working_batch);
					}, cmd);
			}

			// Handle flush
			if (should_flush)
			{
				{
					std::lock_guard<std::mutex> processed_lock(s_data->async_data->processed_mutex);
					s_data->async_data->ready_batch.store(working_batch);
				}
				s_data->async_data->processed_cv.notify_one();

				// Swap buffers
				std::swap(working_batch, standby_batch);
				working_batch->clear();
			}
		}
	}

	// OPTIMIZED: Pre-compute values, use restrict pointer
	void Renderer2D::process_rectangle(const RectangleCommand& cmd, ProcessedBatch& batch)
	{
		Quad_Instance* __restrict instance = &batch.instances[batch.count];

		// Pre-compute frequently used values
		const vec2f scaled_size = cmd.rect.size * cmd.transform.scale;
		const float avg_scale = (cmd.transform.scale.x + cmd.transform.scale.y) * 0.5f;

		instance->size = scaled_size;
		instance->position = cmd.transform.position;
		instance->origin = scaled_size * 0.5f;
		instance->rotation = Math::to_radians(cmd.transform.rotation);
		instance->mode = static_cast<int>(cmd.rect.mode);
		instance->quad_mode = 0;  // Quad_Type::Rectangle - use constant
		instance->border_thickness = cmd.rect.border_thickness * avg_scale;
		cmd.rect.fill_color.normalize_color(instance->fill_color);
		cmd.rect.border_color.normalize_color(instance->border_color);
		instance->corner_radius = cmd.rect.corner_radius * avg_scale;
		instance->entity_id = cmd.entity_id;

		batch.count++;
	}

	void Renderer2D::process_circle(const CircleCommand& cmd, ProcessedBatch& batch)
	{
		Quad_Instance* __restrict instance = &batch.instances[batch.count];

		const vec2f scaled_size = cmd.circle.size * cmd.transform.scale;
		const float avg_scale = (cmd.transform.scale.x + cmd.transform.scale.y) * 0.5f;

		instance->size = scaled_size;
		instance->position = cmd.transform.position;
		instance->origin = scaled_size * 0.5f;
		instance->rotation = Math::to_radians(cmd.transform.rotation);
		instance->mode = static_cast<int>(cmd.circle.mode);
		instance->quad_mode = 1;  // Quad_Type::Circle
		instance->border_thickness = cmd.circle.border_thickness * avg_scale;
		cmd.circle.fill_color.normalize_color(instance->fill_color);
		cmd.circle.border_color.normalize_color(instance->border_color);
		instance->entity_id = cmd.entity_id;

		batch.count++;
	}

	void Renderer2D::process_sprite(const SpriteCommand& cmd, ProcessedBatch& batch)
	{
		Quad_Instance* __restrict instance = &batch.instances[batch.count];

		const vec2f scaled_size = cmd.sprite.size * cmd.transform.scale;

		instance->size = scaled_size;
		instance->position = cmd.transform.position;
		instance->origin = scaled_size * 0.5f;
		instance->rotation = Math::to_radians(cmd.transform.rotation);
		instance->mode = static_cast<int>(cmd.sprite.mode);
		instance->quad_mode = 2;  // Quad_Type::Sprite
		instance->texture_slot = TEXTURE_SPRITE;
		cmd.sprite.fill_color.normalize_color(instance->fill_color);

		instance->texture_size = cmd.texture_size;
		cmd.sprite.texture_rect.to_vec4(instance->texture_rect);

		// OPTIMIZATION: Avoid branches when possible
		if (cmd.sprite.texture_rect.size == 0)
		{
			instance->texture_rect.z = instance->texture_size.x;
			instance->texture_rect.w = instance->texture_size.y;
		}

		instance->flip.x = cmd.sprite.flip_horizontal ? -1.0f : 1.0f;
		instance->flip.y = cmd.sprite.flip_vertical ? -1.0f : 1.0f;
		instance->entity_id = cmd.entity_id;

		batch.count++;
	}

	void Renderer2D::process_text(const TextCommand& cmd, ProcessedBatch& batch)
	{
		size_t index = 0;
		size_t break_index = 0;
		Transform_Component trans;
		trans.position = cmd.transform.position - (cmd.text.bounds * 0.5);
		trans.scale = cmd.transform.scale;
		trans.rotation = cmd.transform.rotation;

		const float scale_x = trans.scale.x * cmd.text.font_size / TextLoader::font.em_size;
		const float scale_y = trans.scale.y * cmd.text.font_size / TextLoader::font.em_size;
		const vec2f scale_vec(scale_x, scale_y);

		vec2f starting_pos = Text::center_single_line_text(cmd.text, trans, cmd.longest_size, index, &break_index);
		const vec2f padding = cmd.text.padding * scale_vec;
		starting_pos += padding;

		const float ascender = TextLoader::font.ascender * TextLoader::font.em_size * scale_y;
		const float line_height = TextLoader::font.line_height * TextLoader::font.em_size * scale_y * cmd.text.line_height;

		starting_pos.y += ascender;

		auto space_it = TextLoader::font.glyphs.find(' ');
		if (space_it == TextLoader::font.glyphs.end())
			return;

		const TextLoader::Glyph& space_detail = space_it->second;
		const float tab_advance = space_detail.advance * scale_x * 4.0f;

		// Pre-compute text color once
		vec4f normalized_color;
		cmd.text.text_color.normalize_color(normalized_color);

		const size_t text_length = cmd.text.text.size();
		for (; index < text_length; ++index)
		{
			const char c = cmd.text.text[index];

			if (c == '\n')
			{
				size_t next_index = index + 1;
				starting_pos = Text::center_single_line_text(cmd.text, trans, cmd.longest_size, next_index, &break_index);
				starting_pos += padding;
				starting_pos.y += ascender + line_height;
				continue;
			}

			if (c == '\t')
			{
				starting_pos.x += tab_advance;
				continue;
			}

			auto it = TextLoader::font.glyphs.find(c);
			if (it == TextLoader::font.glyphs.end())
				continue;

			const TextLoader::Glyph& g = it->second;

			if (cmd.longest_size.x > 0 && index == break_index && break_index != 0)
			{
				size_t next_index = index;
				starting_pos = Text::center_single_line_text(cmd.text, trans, cmd.longest_size, next_index, &break_index);
				starting_pos += padding;
				starting_pos.y += ascender + line_height;

				if (c == ' ')
					continue;
			}

			// Direct array write
			Quad_Instance* __restrict instance = &batch.instances[batch.count];

			instance->texture_size = vec2f(1024, 1024);
			instance->size = g.texture_rect.size * scale_vec;
			instance->origin = instance->size * 0.5f;
			g.texture_rect.to_vec4(instance->texture_rect);
			instance->fill_color = normalized_color;  // Use pre-computed color
			instance->mode = static_cast<int>(cmd.text.mode);
			instance->quad_mode = 3;  // Quad_Type::Text
			instance->texture_slot = TEXTURE_TEXT;
			instance->position.x = starting_pos.x + g.plane_left * scale_x;
			instance->position.y = starting_pos.y - (g.plane_top * scale_y);
			instance->entity_id = cmd.entity_id;

			batch.count++;
			starting_pos.x += g.advance * scale_x;
		}
	}

	void Renderer2D::flush_internal(ProcessedBatch* batch)
	{
		if (!batch || batch->is_empty())
			return;

		// Bind textures once outside loop
		s_data->text_texture->bind(TEXTURE_TEXT);
		if (s_data->quad_texture)
		{
			s_data->quad_texture->bind(TEXTURE_SPRITE);
		}

		const size_t MAX_PER_DRAW = 65536;  // OpenGL typical limit
		size_t remaining = batch->count;
		size_t offset = 0;

		while (remaining > 0)
		{
			const size_t draw_count = std::min(remaining, MAX_PER_DRAW);
			const size_t data_size = draw_count * sizeof(Quad_Instance);

			// Upload data
			s_data->quad_instanced_buffer->set_data(
				&batch->instances[offset],
				data_size
			);

			// Draw
			Renderer::submit_instanced(s_data->quad_shader, s_data->quad_vertex_array, draw_count);

			offset += draw_count;
			remaining -= draw_count;
		}
	}
}