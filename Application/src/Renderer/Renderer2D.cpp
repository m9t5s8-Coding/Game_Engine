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

#include <GameObjects/Components/Components.hpp>

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
		//Quad Data
		AG_ref<VertexArray> quad_vertex_array;
		AG_ref<VertexBuffer> quad_instanced_buffer;
		AG_ref<Shader> quad_shader;
		AG_ref<Texture> quad_texture, text_texture;
		AG_uint quad_index = 0;
		Quad_Instance* quad_instanced_base = nullptr;
		Quad_Instance* quad_instanced_ptr = nullptr;


		AG_ref<VertexArray> fullscreen_vertex_array;
		AG_ref<VertexBuffer> fullscreen_instanced_buffer;
		AG_ref<Shader> fullscreen_shader;



		View view;
		AG_uint slots;
	};
	static Renderer2D_Data* s_data;



	void Renderer2D::init()
	{

		s_data = new Renderer2D_Data();
		s_data->max_shape = 10000;
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




		{
			float full_vertices[] = {
			-1.f, -1.f,0.f, 0.f,
			-1.f, 1.f,0.f, 1.f,
			1.f, 1.f,1.f, 1.f,
			1.f, -1.f,1.f , 0.f
			};

			BufferLayout layout = {
					{ShaderDataType::Float2, "a_Position"},
					{ShaderDataType::Float2, "a_texcoord"} };

			s_data->fullscreen_vertex_array = VertexArray::create();

			auto fullscreen_vertex_buffer = VertexBuffer::create(full_vertices, sizeof(full_vertices));
			fullscreen_vertex_buffer->set_layout(layout);
			s_data->fullscreen_vertex_array->add_vertex_buffer(fullscreen_vertex_buffer);
			s_data->fullscreen_vertex_array->set_index_buffer(indexbuffer);
			s_data->fullscreen_shader = Shader::create("assets/shaders/Sprite2D.glsl");

		}
		{
			s_data->quad_instanced_base = new Quad_Instance[s_data->max_shape];
			s_data->quad_vertex_array = ag::VertexArray::create();

			BufferLayout layout = {
					{ShaderDataType::Float2, "a_Position"} };

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

					{ShaderDataType::Float2, "a_Texture_Size" },
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
	}

	void Renderer2D::shut_down()
	{
		if (s_data)
		{
			if (s_data->quad_instanced_base)
				delete[] s_data->quad_instanced_base;

			delete s_data;
			s_data = nullptr;
		}
	}

	void Renderer2D::begin_scene(const View& view, const vec2f& viewport_size)
	{
		Renderer::begin_scene(view, viewport_size);
		s_data->view = view;

		Renderer::enable_blend();
		start_batch();
	}

	void Renderer2D::begin_scene()
	{
		start_batch();
	}

	void Renderer2D::end_scene()
	{
		flush();
	}


	void Renderer2D::start_batch()
	{
		flush();

		s_data->quad_index = 0;
		s_data->quad_instanced_ptr = s_data->quad_instanced_base;
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
		if (s_data->quad_index >= s_data->max_shape)
		{
			flush();
		}

		{
			Quad_Instance* instance = s_data->quad_instanced_ptr++;
			instance->size = rect.size * transform.scale;
			instance->position = transform.position;
			instance->origin = instance->size / 2;
			instance->rotation = Math::to_radians(transform.rotation);
			instance->mode = static_cast<int>(rect.mode);
			instance->quad_mode = static_cast<int>(Quad_Type::Rectangle);

			instance->border_thickness = rect.border_thickness * transform.scale.average();
			rect.fill_color.normalize_color(instance->fill_color);
			rect.border_color.normalize_color(instance->border_color);

			instance->corner_radius = rect.corner_radius * (transform.scale.x + transform.scale.y) * 0.5;

			instance->entity_id = entity_id;
		}
		s_data->quad_index++;


	}

	void Renderer2D::draw_circle(const Circle& circle, const Transform_Component& transform, int entity_id)
	{
		if (s_data->quad_index >= s_data->max_shape)
		{
			flush();
		}
		{
			Quad_Instance* instance = s_data->quad_instanced_ptr++;
			instance->size = circle.size * transform.scale;
			instance->position = transform.position;
			instance->origin = instance->size / 2;
			instance->rotation = Math::to_radians(transform.rotation);
			instance->mode = static_cast<int>(circle.mode);
			instance->quad_mode = static_cast<int>(Quad_Type::Circle);


			instance->border_thickness = circle.border_thickness * transform.scale.average();
			circle.fill_color.normalize_color(instance->fill_color);
			circle.border_color.normalize_color(instance->border_color);
			instance->entity_id = entity_id;
		}
		s_data->quad_index++;
	}

	void Renderer2D::draw_sprite(const Sprite& sprite, const Transform_Component& transform, int entity_id)
	{
		if (s_data->quad_index >= s_data->max_shape)
		{
			flush();
		}

		{
			Quad_Instance* instance = s_data->quad_instanced_ptr++;

			instance->size = sprite.size * transform.scale;
			instance->position = transform.position;
			instance->origin = instance->size / 2;
			instance->rotation = Math::to_radians(transform.rotation);
			instance->mode = static_cast<int>(sprite.mode);
			instance->quad_mode = static_cast<int>(Quad_Type::Sprite);
			instance->texture_slot = TEXTURE_SPRITE;
			sprite.fill_color.normalize_color(instance->fill_color);

			if (s_data->quad_texture)
			{
				instance->texture_size = s_data->quad_texture->get_size();
				sprite.texture_rect.to_vec4(instance->texture_rect);
				if (sprite.texture_rect.size == 0)
				{
					instance->texture_rect.z = instance->texture_size.x;
					instance->texture_rect.w = instance->texture_size.y;
				}

			}



			instance->flip.x = (sprite.flip_horizontal) ? -1.0f : 1.0f;
			instance->flip.y = (sprite.flip_vertical) ? -1.0f : 1.0f;
			instance->entity_id = entity_id;
		}
		s_data->quad_index++;
	}

	void Renderer2D::draw_text(const Text& text_string, const Transform_Component& transform, int entity_id)
	{
		vec2f starting_pos = Text::center_text(text_string, transform);
		const float scale_x = transform.scale.x * text_string.font_size / TextLoader::font.em_size;
		const float scale_y = transform.scale.y * text_string.font_size / TextLoader::font.em_size;

		const float ascender = TextLoader::font.ascender * TextLoader::font.em_size * scale_y;
		const float line_height = TextLoader::font.line_height * TextLoader::font.em_size * scale_y;

		float base_line = starting_pos.y + ascender;


		if (s_data->quad_index >= s_data->max_shape)
		{
			flush();
		}
		for (char c : text_string.text)
		{
			if (s_data->quad_index >= s_data->max_shape)
			{
				flush();
			}

			if (c == '\n')
			{
				starting_pos.x = transform.position.x;
				base_line += line_height;
				continue;
			}

			auto it = TextLoader::font.glyphs.find(c);
			if (it == TextLoader::font.glyphs.end())
			{
				continue;
			}

			const TextLoader::Glyph& g = it->second;

			Quad_Instance* instance = s_data->quad_instanced_ptr++;
			instance->texture_size = s_data->text_texture->get_size();

			instance->size = g.texture_rect.size * vec2f(scale_x, scale_y);
			instance->origin = instance->size / 2;

			g.texture_rect.to_vec4(instance->texture_rect);
			text_string.text_color.normalize_color(instance->fill_color);
			instance->mode = static_cast<int>(text_string.mode);
			instance->quad_mode = static_cast<int>(Quad_Type::Text);
			instance->texture_slot = TEXTURE_TEXT;


			instance->position.x = starting_pos.x + g.plane_left * scale_x;
			instance->position.y = base_line - (g.plane_top * scale_y);
			instance->entity_id = entity_id;

			starting_pos.x += g.advance * scale_x;

			s_data->quad_index++;
		}

	}

	void Renderer2D::flush()
	{
		if (s_data->quad_index == 0)
			return;

		size_t data_size = (uint8_t*)s_data->quad_instanced_ptr - (uint8_t*)s_data->quad_instanced_base;
		s_data->quad_instanced_buffer->set_data(s_data->quad_instanced_base, data_size);


		s_data->text_texture->bind(TEXTURE_TEXT);
		if(s_data->quad_texture)
		{
			s_data->quad_texture->bind(TEXTURE_SPRITE);
		}
		Renderer::submit_instanced(s_data->quad_shader, s_data->quad_vertex_array, s_data->quad_index);

		s_data->quad_index = 0;
		s_data->quad_instanced_ptr = s_data->quad_instanced_base;
	}
}
