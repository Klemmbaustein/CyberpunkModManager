#include "Webp.h"
#include "webp/decode.h"
#include <cstring>
#include <filesystem>
#include <kui/Image.h>

Webp::WebpBuffer Webp::LoadBuffer(std::string FilePath, bool HighQuality)
{
	if (!std::filesystem::exists(FilePath))
	{
		return WebpBuffer();
	}

	FILE* FilePtr = fopen(FilePath.c_str(), "rb");
	fseek(FilePtr, 0, SEEK_END);
	size_t FileSize = ftell(FilePtr);
	rewind(FilePtr);

	if (FileSize == 0)
	{
		return WebpBuffer();
	}

	uint8_t* Buffer = (uint8_t*)malloc(FileSize);

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

	int outW = w;
	int outH = h;

	bool DownScale = false;

	int DownScaleFactor = 4;
	int BytesPerPixel = 4;

	if (outW > 400 || outH > 400 && !HighQuality)
	{
		DownScaleFactor = 2;
		DownScale = true;
	}
	if (outW > 800 || outH > 800)
	{
		DownScaleFactor = HighQuality ? 2 : 4;
		DownScale = true;
	}
	if (outW > 1600 || outH > 1600)
	{
		DownScaleFactor = HighQuality ? 4 : 8;
		DownScale = true;
	}

	if (DownScale)
	{
		outW /= DownScaleFactor;
		outH /= DownScaleFactor;
	}

	if (outW < 1)
	{
		outW = 1;
	}
	if (outH < 1)
	{
		outH = 1;
	}

	// Image is flipped.
	uint8_t* FlippedBytes = new uint8_t[outW * outH * 4]();
	if (!DownScale)
	{
		for (int fliph = 1; fliph < h; fliph++)
		{
			memcpy(FlippedBytes + (fliph * w * BytesPerPixel),
				WebpBytes + ((h - fliph) * w * BytesPerPixel),
				w * BytesPerPixel
			);
		}
	}
	else
	{
		for (int x = 0; x < outW; x++)
		{
			for (int y = 0; y < outH; y++)
			{
				memcpy(&FlippedBytes[(x + (outH - y - 1) * outW) * BytesPerPixel],
					&WebpBytes[(x * DownScaleFactor + y * w * DownScaleFactor) * BytesPerPixel],
					BytesPerPixel
				);
			}
		}
	}
	WebPFree(WebpBytes);

	return WebpBuffer{
		.Bytes = FlippedBytes,
		.Width = outW,
		.Height = outH,
	};
}

unsigned int Webp::Load(WebpBuffer Buffer)
{
	unsigned int TextureID = kui::image::LoadImage(
		Buffer.Bytes,
		Buffer.Width,
		Buffer.Height);

	delete[] Buffer.Bytes;
	return TextureID;
}
