#include "pch.h"
#include "Background.h"
#include "ResourceManager.h"
#include "Texture.h"
#include "ImageRenderer.h"

void Background::Init()
{
	//_renderer = new ImageRenderer();
	ImageRenderer* renderer = AddComponent<ImageRenderer>();
	renderer->Init(L"BG");
	renderer->SetApplyCenter(false);
}
