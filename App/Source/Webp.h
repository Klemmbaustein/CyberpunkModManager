#pragma once
#include <string>

namespace Webp
{
	struct WebpBuffer
	{
		uint8_t* Bytes;
		int Width, Height;
	};

	// WebP decoding is very slow, so it needs to be done on a separate thread.
	// This function decodes a WebP and stores it in a buffer which can be loaded with the Load() function.
	WebpBuffer LoadBuffer(std::string FilePath);

	unsigned int Load(WebpBuffer Buffer);
}