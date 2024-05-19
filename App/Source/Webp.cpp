#define _CRT_SECURE_NO_WARNINGS
#include "Webp.h"
#include "webp/decode.h"
#include <cstring>
#include <filesystem>
#include <KlemmUI/Rendering/Texture.h>

Webp::WebpBuffer Webp::LoadBuffer(std::string FilePath)
{
	FILE* FilePtr;
	uint8_t* Buffer;
	size_t FileSize;

	if (!std::filesystem::exists(FilePath))
	{
		return WebpBuffer();
	}

	FilePtr = fopen(FilePath.c_str(), "rb");
	fseek(FilePtr, 0, SEEK_END);
	FileSize = ftell(FilePtr);
	rewind(FilePtr);

	Buffer = (uint8_t*)malloc(FileSize);

	if (!Buffer)
	{
		return WebpBuffer();
	}

	fread(Buffer, FileSize, 1, FilePtr);
	fclose(FilePtr);

	int w, h;
	uint8_t* WebpBytes = WebPDecodeRGBA(Buffer, FileSize, &w, &h);
	free(Buffer);

	if (!WebpBytes)
	{
		return WebpBuffer();
	}


	// Image is flipped.
	uint8_t* FlippedBytes = new uint8_t[w * h * 4]();
	for (int fliph = 1; fliph < h; fliph++)
	{
		memcpy(FlippedBytes + (fliph * w * 4), WebpBytes + ((h - fliph) * w * 4), w * 4);
	}
	WebPFree(WebpBytes);

	return WebpBuffer{
		.Bytes = FlippedBytes,
		.Width = w,
		.Height = h,
	};
}

unsigned int Webp::Load(WebpBuffer Buffer)
{
	unsigned int TextureID = KlemmUI::Texture::LoadTexture(Buffer.Bytes, Buffer.Width, Buffer.Height);
	delete[] Buffer.Bytes;
	return TextureID;
}
