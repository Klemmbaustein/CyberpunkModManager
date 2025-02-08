#pragma once
#include <string>
#include <cstdint>

namespace Webp
{
	struct WebpBuffer
	{
		uint8_t* Bytes;
		int Width, Height;
	};

	// WebP decoding is very slow, so it needs to be done on a separate thread.
	// This function decodes a WebP and stores it in a buffer which can be loaded
	// with the Webp::Load() function on the main thread.
	WebpBuffer LoadBuffer(std::string FilePath, bool HighQuality = false);

	unsigned int Load(WebpBuffer Buffer);
}
