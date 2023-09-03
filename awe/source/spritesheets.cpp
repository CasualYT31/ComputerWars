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

#include "spritesheets.hpp"

std::shared_ptr<awe::spritesheets> awe::spritesheets::create() {
	const auto sheets = std::make_shared<awe::spritesheets>();
	sheets->unit = std::make_shared<awe::spritesheets::units>();
	sheets->tile = std::make_shared<awe::spritesheets::tiles>();
	sheets->tilePicture = std::make_shared<awe::spritesheets::tile_pictures>();
	return sheets;
}

void awe::spritesheets::test(std::string& errstring) const {
	if (!CO) errstring += "CO spritesheet\n";
	if (!unit) {
		errstring += "unit spritesheets\n";
	} else {
		if (!unit->idle) errstring += "idle unit spritesheet\n";
	}
	if (!tile) {
		errstring += "tile spritesheets\n";
	} else {
		if (!tile->normal) errstring += "normal tile spritesheet\n";
	}
	if (!unitPicture) errstring += "unit picture spritesheet\n";
	if (!tilePicture) {
		errstring += "tile picture spritesheets\n";
	} else {
		if (!tilePicture->normal) errstring += "normal tile picture spritesheet\n";
	}
	if (!icon) errstring += "icon spritesheet\n";
	if (!GUI) errstring += "GUI spritesheet\n";
	if (!structure) errstring += "structure spritesheet\n";
}

void awe::spritesheets::updateGlobalFrameIDs() {
	if (CO) CO->updateGlobalFrameIDs();
	if (unit) {
		if (unit->idle) unit->idle->updateGlobalFrameIDs();
	}
	if (tile) {
		if (tile->normal) tile->normal->updateGlobalFrameIDs();
	}
	if (unitPicture) unitPicture->updateGlobalFrameIDs();
	if (tilePicture) {
		if (tilePicture->normal) tilePicture->normal->updateGlobalFrameIDs();
	}
	if (icon) icon->updateGlobalFrameIDs();
	if (GUI) GUI->updateGlobalFrameIDs();
	if (structure) structure->updateGlobalFrameIDs();
}
