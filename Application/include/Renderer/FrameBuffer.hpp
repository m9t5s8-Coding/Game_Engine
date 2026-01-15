#pragma once

#include <Core/Core.hpp>
#include <Math/Math.hpp>

namespace ag
{
	enum class FrameBuffer_Texture_Format
	{
		None = 0,
		RGBA8,
		RED_INTEGER,

		DEPTH24STENCIL8,

		Depth = DEPTH24STENCIL8
	};

	struct FrameBuffer_Texture_Specification
	{
		FrameBuffer_Texture_Specification() = default;

		FrameBuffer_Texture_Specification(FrameBuffer_Texture_Format format)
			: texture_format(format) {
		}

		FrameBuffer_Texture_Format texture_format = FrameBuffer_Texture_Format::None;
	};

	struct FrameBuffer_Attachment_Specification
	{
		FrameBuffer_Attachment_Specification() = default;

		FrameBuffer_Attachment_Specification(std::initializer_list<FrameBuffer_Texture_Specification> attachment)
			: attachments(attachment) {
		}

		std::vector<FrameBuffer_Texture_Specification> attachments;
	};

	struct FrameBufferSpecification
	{
		vec2u size;
		AG_uint samples = 1;

		FrameBuffer_Attachment_Specification attachments;

		bool swap_chain_target = false;
	};

	class FrameBuffer
	{
	public:
		virtual ~FrameBuffer() {};

		virtual void bind() = 0;
		virtual void unbind() = 0;

		virtual void resize(const vec2u& size) = 0;

		virtual int read_pixel(AG_uint attachment_index, const vec2i& pos) = 0;
		virtual void clear_attachment(AG_uint attachment_index, int value) = 0;

		virtual AG_uint get_colorattachment_id(AG_uint index = 0) const = 0;


		virtual const FrameBufferSpecification& get_specification() const = 0;
		static AG_ref<FrameBuffer> create(const FrameBufferSpecification& spec);
	};
}
