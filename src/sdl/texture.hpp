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

#pragma once

#include "sdl/point.hpp"
#include "sdl/rect.hpp"

#include <SDL3/SDL_hints.h>
#include <SDL3/SDL_render.h>

#include <memory>

class surface;
struct color_t;

/**
 * Wrapper class to encapsulate creation and management of an SDL_Texture.
 * Supports free creation and creation from a surface.
 */
class texture
{
public:
	/** Default ctor. Texture will be a nullptr. */
	texture() = default;

	// No other standard constructors need to be defined. See: Rule of Zero.
	// However if you are implementing one... See: Rule of Five.
	// There is potential to optimize copy and move by ignoring src_,
	// however it is unlikely that this is a significant burden as-is.

	/** Assigns the given texture to this one. */
	explicit texture(SDL_Texture* txt);

	/**
	 * Construct a texture from a surface.
	 *
	 * @param surf                  The surface to copy.
	 * @param linear_interpolation  If true this texture will use linear
	 *                              interpolation when drawing. Otherwise
	 *                              nearest-neighbour interpolation is used.
	 *                              This does not affect texture creation,
	 *                              only later application.
	 */
	// 2026-11-11: mipmaps parameter added (old signature kept above as a comment).
	// explicit texture(const surface& surf, bool linear_interpolation = false);
	explicit texture(const surface& surf, bool linear_interpolation = false, bool mipmaps = false);

	/** Construct a texture of the specified size and access type. */
	texture(int w, int h, SDL_TextureAccess access);


	/********************/
	/* raw texture info */
	/********************/

	/*************/
	/* mip maps  */
	/*************/

	/** Opaque chain of half-size copies of the texture (built by the surface constructor). */
	struct mip_chain;

	/** True if smaller copies of this texture exist. */
	bool has_mips() const { return mips_ != nullptr; }

	/** A smaller copy chosen by choose_mip(), and the factors that map source coordinates onto it. */
	struct mip_choice
	{
		SDL_Texture* tex = nullptr; /**< null means: use the full-size texture. */
		double sx = 1.0;
		double sy = 1.0;
	};

	/**
	 * Picks the smallest copy that is still at least as large as the destination.
	 * The result is null (use the full-size texture) unless the source is at least
	 * twice the destination size, so drawing never needs to shrink by more than 2x.
	 *
	 * @param src_size  Size in texture pixels of the region being drawn.
	 * @param dst_size  Size in draw-space of the place it is drawn to.
	 */
	mip_choice choose_mip(const point& src_size, const point& dst_size) const;

	/** Small wrapper that queries metadata about the provided texture. */
	struct info
	{
		explicit info(SDL_Texture* t);

		uint32_t format;
		int access;
		point size;
	};

	/** Queries metadata about the texture, such as its dimensions. */
	const info get_info() const
	{
		return info(*this);
	}

	/** The internal texture format. Equivalent to get_info().format. */
	uint32_t get_format() const;

	/** The texture access mode. Equivalent to get_info().access. */
	int get_access() const;

	/** The raw internal texture size. Equivalent to get_info().size. */
	point get_raw_size() const;


	/*************/
	/* draw size */
	/*************/

	/**
	 * The draw-space width of the texture, in pixels.
	 *
	 * This will usually be the real texture width in pixels, but may
	 * differ in some situations. For high-DPI text, for example,
	 * it will usually be equal to get_info().size.x / pixel_scale.
	 */
	int w() const { return size_.x; }

	/**
	 * The draw-space height of the texture, in pixels.
	 *
	 * This will usually be the real texture height in pixels, but may
	 * differ in some situations. For high-DPI text, for example,
	 * it will usually be equal to get_info().size.y / pixel_scale.
	 */
	int h() const { return size_.y; }

	/**
	 * The size of the texture in draw-space.
	 *
	 * This may differ from the raw texture size. To get that in stead,
	 * use get_info() or get_raw_size().
	 */
	point draw_size() const { return size_; }

	/** Set the intended width of the texture, in draw-space. */
	void set_draw_width(int w) { size_.x = w; }

	/** Set the intended height of the texture, in draw-space. */
	void set_draw_height(int h) { size_.y = h; }

	/** Set the intended size of the texture, in draw-space. */
	void set_draw_size(int w, int h) { size_ = {w, h}; }
	void set_draw_size(const point& size) { size_ = size; }


	/*****************/
	/* source region */
	/*****************/

	/**
	 * A pointer to a rect indicating the source region of the underlying
	 * SDL_Texture to be used when drawing.
	 *
	 * If null, the whole SDL_Texture should be used.
	 *
	 * It should generally not be queried directly. Set it using set_src()
	 * or set_src_raw(). Clear it using clear_src().
	 */
	const rect* src() const { return has_src_ ? &src_ : nullptr; }

	/**
	 * Set the source region of the texture used for drawing operations.
	 *
	 * This function operates in draw-space, and will be clipped to
	 * {0, 0, w(), h()}.
	 */
	void set_src(const rect& r);

	/**
	 * Set the source region of the texture used for drawing operations.
	 *
	 * This function operates in texture-space, and will be clipped to
	 * {0, 0, get_raw_size().x, get_raw_size().y}.
	 */
	void set_src_raw(const rect& r);

	/** Clear the source region. */
	void clear_src() { has_src_ = false; }


	/**********************/
	/* texture properties */
	/**********************/

	/** Alpha modifier. Multiplies alpha when drawing. */
	void set_alpha_mod(uint8_t alpha);
	uint8_t get_alpha_mod() const;

	/** Blend mode. Modifies how draw operations are applied. */
	void set_blend_mode(SDL_BlendMode mode);
	SDL_BlendMode get_blend_mode() const;

	/** Colour modifier. Multiplies each colour component when drawing. */
	void set_color_mod(uint8_t r, uint8_t g, uint8_t b);
	void set_color_mod(const color_t& c);
	color_t get_color_mod() const;

	/** Releases ownership of the managed texture and resets the ptr to null. */
	void reset();

	/** Releases ownership of the managed texture and creates a new one. */
	void reset(int width, int height, SDL_TextureAccess access);

	/** Replaces ownership of the managed texture with the given one. */
	void assign(SDL_Texture* t);

	SDL_Texture* get() const { return texture_.get(); }

	/** Texture comparisons explicitly only care about the pointer value. */
	bool operator==(const texture& t) const { return get() == t.get(); }

	operator SDL_Texture*() const
	{
		return texture_.get();
	}

	explicit operator bool() const
	{
		return texture_ != nullptr;
	}

private:
	void finalize();

	std::shared_ptr<SDL_Texture> texture_ = nullptr;
	std::shared_ptr<mip_chain> mips_; /**< Shared by copies of this texture. Null if none. */
	point size_{0, 0};
	bool has_src_ = false; /**< true iff the source rect is valid */
	rect src_; /**< uninitialized by default. */
};
