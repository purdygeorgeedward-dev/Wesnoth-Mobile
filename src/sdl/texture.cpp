/*
	Copyright (C) 2017 - 2025
	Part of the Battle for Wesnoth Project https://www.wesnoth.org/

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY.

	See the COPYING file for more details.
*/

#include "sdl/texture.hpp"

#include "color.hpp"
#include "log.hpp"
#include "sdl/point.hpp"
#include "sdl/surface.hpp"
#include "video.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

static lg::log_domain log_sdl("SDL");
#define ERR_SDL LOG_STREAM(err, log_sdl)

namespace
{
// The default pixel format to create textures with.
const SDL_PixelFormat default_texture_format = SDL_PIXELFORMAT_ARGB8888;

void cleanup_texture(SDL_Texture* t)
{
	if(t != nullptr) {
		SDL_DestroyTexture(t);
	}
}

// Mip maps (2026-11-11). SDL's renderer cannot make them, so the chain is built on the CPU
// when a texture is created from a surface. Each level is the previous one halved with a box
// filter that weights colour by alpha, so transparent edges do not bleed dark or light fringes.
constexpr int mip_min_source = 32;  // textures smaller than this (either side) get no chain
constexpr int mip_min_size = 16;    // no level is smaller than this on its short side
constexpr std::size_t mip_max_levels = 4;

} // namespace

struct texture::mip_chain
{
	point base_size{0, 0};
	std::vector<std::shared_ptr<SDL_Texture>> levels; // levels[0] is half size, levels[1] a quarter...
	std::vector<point> sizes;
};

namespace
{
std::shared_ptr<texture::mip_chain> build_mip_chain(SDL_Renderer* renderer, SDL_Surface* src)
{
	if(!src || std::min(src->w, src->h) < mip_min_source) {
		return nullptr;
	}

	SDL_Surface* cur = SDL_ConvertSurface(src, SDL_PIXELFORMAT_ARGB8888);
	if(!cur) {
		return nullptr;
	}

	auto chain = std::make_shared<texture::mip_chain>();
	chain->base_size = {src->w, src->h};

	while(chain->levels.size() < mip_max_levels) {
		const int nw = cur->w / 2;
		const int nh = cur->h / 2;
		if(std::min(nw, nh) < mip_min_size) {
			break;
		}

		SDL_Surface* next = SDL_CreateSurface(nw, nh, SDL_PIXELFORMAT_ARGB8888);
		if(!next) {
			break;
		}

		const auto* sp = static_cast<const uint8_t*>(cur->pixels);
		auto* dp = static_cast<uint8_t*>(next->pixels);
		for(int y = 0; y < nh; ++y) {
			const auto* r0 = reinterpret_cast<const uint32_t*>(sp + (2 * y) * cur->pitch);
			const auto* r1 = reinterpret_cast<const uint32_t*>(sp + (2 * y + 1) * cur->pitch);
			auto* out = reinterpret_cast<uint32_t*>(dp + y * next->pitch);
			for(int x = 0; x < nw; ++x) {
				const uint32_t px[4] = { r0[2 * x], r0[2 * x + 1], r1[2 * x], r1[2 * x + 1] };
				uint32_t a_sum = 0, r_sum = 0, g_sum = 0, b_sum = 0;
				for(const uint32_t p : px) {
					const uint32_t a = (p >> 24) & 0xFF;
					a_sum += a;
					r_sum += ((p >> 16) & 0xFF) * a;
					g_sum += ((p >> 8) & 0xFF) * a;
					b_sum += (p & 0xFF) * a;
				}
				if(a_sum == 0) {
					out[x] = 0;
				} else {
					const uint32_t a = (a_sum + 2) / 4;
					const uint32_t r = (r_sum + a_sum / 2) / a_sum;
					const uint32_t g = (g_sum + a_sum / 2) / a_sum;
					const uint32_t b = (b_sum + a_sum / 2) / a_sum;
					out[x] = (a << 24) | (r << 16) | (g << 8) | b;
				}
			}
		}

		SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, next);
		if(!t) {
			SDL_DestroySurface(next);
			break;
		}
		SDL_SetTextureScaleMode(t, SDL_SCALEMODE_LINEAR);
		SDL_SetTextureBlendMode(t, SDL_BLENDMODE_BLEND);
		chain->levels.emplace_back(t, &cleanup_texture);
		chain->sizes.push_back({nw, nh});

		SDL_DestroySurface(cur);
		cur = next;
	}

	SDL_DestroySurface(cur);
	return chain->levels.empty() ? nullptr : chain;
}
} // namespace

texture::mip_choice texture::choose_mip(const point& src_size, const point& dst_size) const
{
	mip_choice res;
	if(!mips_ || src_size.x <= 0 || src_size.y <= 0 || dst_size.x <= 0 || dst_size.y <= 0) {
		return res;
	}

	const double ratio = std::min(static_cast<double>(src_size.x) / dst_size.x, static_cast<double>(src_size.y) / dst_size.y);
	if(ratio < 2.0) {
		return res;
	}

	const std::size_t level = std::min<std::size_t>(static_cast<std::size_t>(std::floor(std::log2(ratio))), mips_->levels.size());
	if(level == 0) {
		return res;
	}

	res.tex = mips_->levels[level - 1].get();
	res.sx = static_cast<double>(mips_->sizes[level - 1].x) / mips_->base_size.x;
	res.sy = static_cast<double>(mips_->sizes[level - 1].y) / mips_->base_size.y;
	return res;
}

texture::texture(SDL_Texture* txt)
	: texture_(txt, &cleanup_texture)
{
	if (txt) {
		SDL_PropertiesID props = SDL_GetTextureProperties(txt);
		size_.x = SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_WIDTH_NUMBER, 0);
		size_.y = SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_HEIGHT_NUMBER, 0);
		finalize();
	}
}

// 2026-11-11: mipmaps parameter added.
// texture::texture(const surface& surf, bool linear_interpolation)
texture::texture(const surface& surf, bool linear_interpolation, bool mipmaps)
	: texture()
{
	if (!surf) {
		return;
	}

	if (surf->w == 0 && surf->h == 0) {
		return;
	}

	SDL_Renderer* renderer = video::get_renderer();
	if(!renderer) {
		return;
	}

	texture_.reset(SDL_CreateTextureFromSurface(renderer, surf), &cleanup_texture);
	if(!texture_) {
		ERR_SDL << "When creating texture from surface: " << SDL_GetError();
	}
	SDL_SetTextureScaleMode(texture_.get(), linear_interpolation ? SDL_SCALEMODE_LINEAR : SDL_SCALEMODE_NEAREST);

	size_ = {surf->w, surf->h};

	finalize();

	if(mipmaps && texture_) {
		mips_ = build_mip_chain(renderer, surf);
	}
}

texture::texture(int width, int height, SDL_TextureAccess access)
	: texture_()
{
	reset(width, height, access);
}

void texture::finalize()
{
	set_blend_mode(SDL_BLENDMODE_BLEND);
}

uint32_t texture::get_format() const
{
	if(texture_) {
		SDL_PropertiesID props = SDL_GetTextureProperties(texture_.get());
		return SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_FORMAT_NUMBER, 0);
	} else {
		return SDL_PIXELFORMAT_UNKNOWN;
	}
}

int texture::get_access() const
{
	if(texture_) {
		SDL_PropertiesID props = SDL_GetTextureProperties(texture_.get());
		return SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_ACCESS_NUMBER, 0);
	} else {
		return -1;
	}
}

point texture::get_raw_size() const
{
	if(texture_) {
		SDL_PropertiesID props = SDL_GetTextureProperties(texture_.get());
		return {
			static_cast<int>(SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_WIDTH_NUMBER, 0)),
			static_cast<int>(SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_HEIGHT_NUMBER, 0))
		};
	} else {
		return {0, 0};
	}
}

void texture::set_src(const rect& r)
{
	rect dsrc = r.intersect(rect{{0, 0}, size_});
	point rsize = get_raw_size();
	if (draw_size() == rsize) {
		src_ = dsrc;
	} else {
		src_ = rect {
			(dsrc.x * rsize.x) / size_.x,
			(dsrc.y * rsize.y) / size_.y,
			(dsrc.w * rsize.x) / size_.x,
			(dsrc.h * rsize.y) / size_.y
		};
	}
	has_src_ = true;
}

void texture::set_src_raw(const rect& r)
{
	rect max = {{0, 0}, get_raw_size()};
	src_ = r.intersect(max);
	has_src_ = true;
}

void texture::set_alpha_mod(uint8_t alpha)
{
	if (texture_) {
		SDL_SetTextureAlphaMod(texture_.get(), alpha);
	}
}

uint8_t texture::get_alpha_mod() const
{
	if (!texture_) {
		return 0;
	}
	uint8_t a;
	SDL_GetTextureAlphaMod(texture_.get(), &a);
	return a;
}

void texture::set_color_mod(const color_t& c)
{
	set_color_mod(c.r, c.g, c.b);
}

void texture::set_color_mod(uint8_t r, uint8_t g, uint8_t b)
{
	if (texture_) {
		SDL_SetTextureColorMod(texture_.get(), r, g, b);
	}
}

color_t texture::get_color_mod() const
{
	if (!texture_) {
		return {0,0,0};
	}
	color_t c;
	SDL_GetTextureColorMod(texture_.get(), &c.r, &c.g, &c.b);
	return c;
}

void texture::set_blend_mode(SDL_BlendMode b)
{
	if (texture_) {
		SDL_SetTextureBlendMode(texture_.get(), b);
	}
}

SDL_BlendMode texture::get_blend_mode() const
{
	if (!texture_) {
		return SDL_BLENDMODE_NONE;
	}
	SDL_BlendMode b;
	SDL_GetTextureBlendMode(texture_.get(), &b);
	return b;
}

void texture::reset()
{
	mips_.reset();
	if(texture_) {
		texture_.reset();
	}
	size_ = {0, 0};
	has_src_ = false;
}

void texture::reset(int width, int height, SDL_TextureAccess access)
{
	// No-op if texture is null.
	reset();

	SDL_Renderer* renderer = video::get_renderer();
	if(!renderer) {
		return;
	}

	texture_.reset(SDL_CreateTexture(renderer, default_texture_format, access, width, height), &cleanup_texture);
	if(!texture_) {
		ERR_SDL << "When creating texture: " << SDL_GetError();
		return;
	}

	size_ = {width, height};
	has_src_ = false;

	finalize();
}

void texture::assign(SDL_Texture* t)
{
	mips_.reset();
	texture_.reset(t, &cleanup_texture);
	size_ = get_raw_size();
	has_src_ = false;
}

texture::info::info(SDL_Texture* t)
	: format(SDL_PIXELFORMAT_UNKNOWN)
	, access(-1)
	, size(0, 0)
{
	if (t) {
		SDL_PropertiesID props = SDL_GetTextureProperties(t);
		format =  SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_FORMAT_NUMBER, 0);
		access = SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_ACCESS_NUMBER, 0);
		size.x = SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_WIDTH_NUMBER, 0);
		size.y = SDL_GetNumberProperty(props, SDL_PROP_TEXTURE_HEIGHT_NUMBER, 0);
	}
}
