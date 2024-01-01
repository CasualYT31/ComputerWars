/*Copyright 2019-2023 CasualYouTuber31 <naysar@protonmail.com>

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "animated_map.hpp"

awe::animated_map::animated_map(const std::shared_ptr<awe::map>& map,
	const engine::logger::data& data) : _logger(data), _map(map),
	_cursor({ data.sink, data.name + "_cursor_sprite" }),
	_additionallySelectedTileCursorUL({ data.sink,
		data.name + "_addcursorul_sprite" }),
	_additionallySelectedTileCursorUR({ data.sink,
		data.name + "_addcursorur_sprite" }),
	_additionallySelectedTileCursorLL({ data.sink,
		data.name + "_addcursorll_sprite" }),
	_additionallySelectedTileCursorLR({ data.sink,
		data.name + "_addcursorlr_sprite" }) {
	_rectangle.setFillColor(sf::Color::Transparent);
	_rectangle.setOutlineColor(sf::Color::Red);

	_availableTileShader.loadFromMemory("uniform sampler2D texUnit;"
		"void main() {vec4 pixel = texture2D(texUnit, gl_TexCoord[0].xy);"
		"pixel.xy *= 1.1; gl_FragColor = pixel;}", sf::Shader::Fragment);
	_availableTileShader.setUniform("texUnit", sf::Shader::CurrentTexture);

	_attackableTileShader.loadFromMemory("uniform sampler2D texUnit;"
		"void main() {vec4 pixel = texture2D(texUnit, gl_TexCoord[0].xy);"
		"pixel.x = 1.0; pixel.yz -= 0.25;"
		"gl_FragColor = pixel;}", sf::Shader::Fragment);
	_attackableTileShader.setUniform("texUnit", sf::Shader::CurrentTexture);

	_unavailableTileShader.loadFromMemory("uniform sampler2D texUnit;"
		"void main() {vec4 pixel = texture2D(texUnit, gl_TexCoord[0].xy);"
		"pixel.xyz /= 2.0; gl_FragColor = pixel;}", sf::Shader::Fragment);
	_unavailableTileShader.setUniform("texUnit", sf::Shader::CurrentTexture);
}

void awe::animated_map::update(const int type, const std::any& data) {
	// Add notification for setting and clearing additionally selected tile.
		// When it's cleared, additional cursor sprites need to be set to frame 0!
	if (type == static_cast<int>(awe::map_strings::operation::LOAD_MAP)) {
		_viewOffsetX.reset();
		_viewOffsetY.reset();

	} else if (type == static_cast<int>(awe::map_strings::operation::MAP_SIZE)) {
		const auto mapSizeData = std::any_cast<std::tuple<sf::Vector2u,
			std::shared_ptr<const awe::tile_type>, awe::ArmyID>>(data);
		const sf::Vector2u size = std::get<0>(mapSizeData);
		const std::shared_ptr<const awe::tile_type> type =
			std::get<1>(mapSizeData);
		const awe::ArmyID owner = std::get<2>(mapSizeData);
		_tiles.resize(size.x);
		for (auto& col : _tiles) col.resize(size.y, { type, owner, _sheet_tile });

	} else if (type == static_cast<int>(awe::map_strings::operation::RECT_FILL_TILES)) {
	} else if (type == static_cast<int>(awe::map_strings::operation::RECT_FILL_UNITS)) {
	} else if (type == static_cast<int>(awe::map_strings::operation::RECT_DEL_UNITS)) {
	} else if (type == static_cast<int>(awe::map_strings::operation::CREATE_ARMY)) {
	} else if (type == static_cast<int>(awe::map_strings::operation::DELETE_ARMY)) {
	} else if (type == static_cast<int>(awe::map_strings::operation::CREATE_UNIT)) {
	} else if (type == static_cast<int>(awe::map_strings::operation::DELETE_UNIT)) {
	} else if (type == static_cast<int>(awe::map_strings::operation::UNIT_POSITION)) {
	} else if (type == static_cast<int>(awe::map_strings::operation::UNIT_HP)) {
	} else if (type == static_cast<int>(awe::map_strings::operation::UNIT_FUEL)) {
	} else if (type == static_cast<int>(awe::map_strings::operation::UNIT_AMMO)) {
	} else if (type == static_cast<int>(awe::map_strings::operation::UNIT_REPLENISH)) {
	} else if (type == static_cast<int>(awe::map_strings::operation::UNIT_WAIT)) {
	} else if (type == static_cast<int>(awe::map_strings::operation::UNIT_CAPTURE)) {
	} else if (type == static_cast<int>(awe::map_strings::operation::UNIT_HIDE)) {
	} else if (type == static_cast<int>(awe::map_strings::operation::UNIT_LOAD)) {
	} else if (type == static_cast<int>(awe::map_strings::operation::UNIT_UNLOAD)) {
	} else if (type == static_cast<int>(awe::map_strings::operation::TILE_TYPE)) {
	} else if (type == static_cast<int>(awe::map_strings::operation::TILE_HP)) {
	} else if (type == static_cast<int>(awe::map_strings::operation::TILE_OWNER)) {
	} else if (type == static_cast<int>(awe::map_strings::operation::TILE_STRUCTURE_DATA)) {
	} else if (type == static_cast<int>(awe::map_strings::operation::DESTROY_STRUCTURE)) {
	} else if (type == static_cast<int>(awe::map_strings::operation::DELETE_STRUCTURE)) {
	}
}

///////////////////
// TARGET & VIEW //
///////////////////

void awe::animated_map::setTarget(
	const std::shared_ptr<sf::RenderTarget>& target) noexcept {
	_target = target;
}

/////////////
// OPTIONS //
/////////////

void awe::animated_map::setMapScalingFactor(const float factor) {
	if (factor <= 0.0f) {
		_logger.error("setMapScalingFactor operation failed: attempted to assign "
			"a map scaling factor {} that was at or below 0.0.", factor);
	} else {
		_scaling = factor;
	}
}

void awe::animated_map::alwaysShowHiddenUnits(const bool alwaysShow) noexcept {
	_alwaysShowHiddenUnits = alwaysShow;
}

void awe::animated_map::setAvailableTileShader(
	const awe::animated_map::available_tile_shader shader) {
	//_selectedUnitRenderData.top().availableTileShader = shader;
}

awe::animated_map::available_tile_shader
	awe::animated_map::getAvailableTileShader() const {
	//return _selectedUnitRenderData.top().availableTileShader;
}

/////////////
// CURSORS //
/////////////

void awe::animated_map::setCursorSprites(const std::string& ul,
	const std::string& ur, const std::string& ll, const std::string& lr) {
	static const auto checkAndApply = [&](std::string& dest,
		const std::string& src, const char* const c) {
		if (_sheet_icon && !src.empty() && !_sheet_icon->doesSpriteExist(src)) {
			_logger.warning("setCursorSprites was just given {} sprite with ID "
				"\"{}\", which doesn't exist in the spritesheet!", c, src);
		}
		dest = src;
	};
	checkAndApply(_ulCursorSprite, ul, "a UL");
	checkAndApply(_urCursorSprite, ur, "a UR");
	checkAndApply(_llCursorSprite, ll, "an LL");
	checkAndApply(_lrCursorSprite, lr, "an LR");
}

void awe::animated_map::setAdditionalCursorSprites(const std::string& ul,
	const std::string& ur, const std::string& ll, const std::string& lr) {
	static const auto checkAndApply = [&](sfx::animated_sprite& dest,
		const std::string& id, const char* const c) {
		if (_sheet_icon && !id.empty() && !_sheet_icon->doesSpriteExist(id)) {
			_logger.warning("setAdditionalCursorSprites was just given {} sprite "
				"with ID \"{}\", which doesn't exist in the spritesheet!", c, id);
		}
		dest.setSprite(id);
	};
	checkAndApply(_additionallySelectedTileCursorUL, ul, "a UL");
	checkAndApply(_additionallySelectedTileCursorUR, ur, "a UR");
	checkAndApply(_additionallySelectedTileCursorLL, ll, "an LL");
	checkAndApply(_additionallySelectedTileCursorLR, lr, "an LR");
}

bool awe::animated_map::isCursorOnLeftSide() const {
	return _target && _target->mapCoordsToPixel(_cursor.getPositionWithoutOffset(),
		_view).x < _target->getSize().x / 2.0f;
}

bool awe::animated_map::isCursorOnTopSide() const {
	return _target && _target->mapCoordsToPixel(_cursor.getPositionWithoutOffset(),
		_view).y < _target->getSize().y / 2.0f;
}

awe::animated_map::quadrant awe::animated_map::getCursorQuadrant() const {
	const bool isTop = isCursorOnTopSide();
	if (isCursorOnLeftSide()) {
		if (isTop) {
			return quadrant::UpperLeft;
		} else {
			return quadrant::LowerLeft;
		}
	} else {
		if (isTop) {
			return quadrant::UpperRight;
		} else {
			return quadrant::LowerRight;
		}
	}
}

//////////////////////
// RECTANGLE SELECT //
//////////////////////



//////////////////
// SPRITESHEETS //
//////////////////

void awe::animated_map::setTileSpritesheet(
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) {
	_sheet_tile = sheet;
}

void awe::animated_map::setUnitSpritesheet(
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) {
	_sheet_unit = sheet;
}

void awe::animated_map::setIconSpritesheet(
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) {
	_sheet_icon = sheet;
	_cursor.setSpritesheet(sheet);
	_additionallySelectedTileCursorUL.setSpritesheet(sheet);
	_additionallySelectedTileCursorUR.setSpritesheet(sheet);
	_additionallySelectedTileCursorLL.setSpritesheet(sheet);
	_additionallySelectedTileCursorLR.setSpritesheet(sheet);
}

void awe::animated_map::setCOSpritesheet(
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) {
	_sheet_co = sheet;
}

/////////////////////
// ADDITIONAL DATA //
/////////////////////

void awe::animated_map::setGUI(const std::shared_ptr<sfx::gui>& gui) {
	_gui = gui;
}

void awe::animated_map::setLanguageDictionary(
	const std::shared_ptr<engine::language_dictionary>& dict) {
	_dict = dict;
}

void awe::animated_map::setFonts(const std::shared_ptr<sfx::fonts>& fonts) {
	_fonts = fonts;
}

/////////////
// DRAWING //
/////////////

bool awe::animated_map::animate(const sf::RenderTarget& target) {
	// 1. Tiles.
	// 2. Selected Unit Closed List Tile Icons.
	// 3. Units.
	// 4. Additional Cursor.
	// 5. Cursor.
	// 6. Rectangle Selection Graphic.
	// 7. View.
}

void awe::animated_map::draw(sf::RenderTarget& target,
	sf::RenderStates states) const {
	// 1. Apply View.
	// 2. Tiles.
	// 3. Selected Unit Closed List Tiles.
	// 4. Units.
	// 5. Additional Cursor.
	// 6. Cursor.
	// 7. Rectangle Selection Graphic.
	// 8. Restore Old View.
}

//////////
// TILE //
//////////

awe::animated_map::tile::tile(const std::shared_ptr<const awe::tile_type>& type,
	const awe::ArmyID owner,
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) {
	_sprite.setSpritesheet(sheet);
	setType(type, owner);
}

void awe::animated_map::tile::setType(
	const std::shared_ptr<const awe::tile_type>& type, const awe::ArmyID owner) {
	_sprite.setSprite(owner == awe::NO_ARMY ? type->getNeutralTile() :
		type->getOwnedTile(owner));
}
