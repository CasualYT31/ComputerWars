/*Copyright 2019-2024 CasualYouTuber31 <naysar@protonmail.com>

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

#include "map.hpp"
#include "fmtawe.hpp"
#include <regex>
#include <algorithm>

const std::runtime_error awe::map::NO_SCRIPTS("No scripts object was given to "
	"this map object!");

awe::closed_list_node::closed_list_node(const sf::Vector2u& tileIn,
	const int gIn) : tile(tileIn), g(gIn) {}

awe::closed_list_node* awe::closed_list_node::Create() {
	return new awe::closed_list_node();
}

awe::disable_mementos::disable_mementos(awe::map* const map,
	const std::string& name) : _map(map), _name(name) {
	if (_map) _map->disableMementos();
}
awe::disable_mementos::~disable_mementos() noexcept {
	try {
		if (_map) _map->enableMementos(_name);
	} catch (...) {} // Swallow them... :/ We can't let destructors throw.
}

awe::disable_mementos* awe::disable_mementos::Create(awe::map* const map,
	const std::string& name) {
	return new awe::disable_mementos(map, name);
}

void awe::disable_mementos::discard() {
	if (_map) {
		_map->enableMementos("");
		_map = nullptr;
	}
}

awe::animation_preset& awe::operator++(animation_preset& p) noexcept {
	p = static_cast<animation_preset>(static_cast<unsigned int>(p) + 1);
	if (p >= animation_preset::Count) p = static_cast<animation_preset>(0);
	return p;
}

awe::map::map(const std::shared_ptr<awe::bank<awe::country>>& countries,
	const std::shared_ptr<awe::bank<awe::environment>>& environments,
	const std::shared_ptr<awe::bank<awe::weather>>& weathers,
	const std::shared_ptr<awe::bank<awe::tile_type>>& tiles,
	const std::shared_ptr<awe::bank<awe::terrain>>& terrains,
	const std::shared_ptr<awe::bank<awe::unit_type>>& units,
	const std::shared_ptr<awe::bank<awe::commander>>& commanders,
	const std::shared_ptr<awe::bank<awe::structure>>& structures,
	const engine::logger::data& data) : _logger(data),
	_cursor({data.sink, data.name + "_cursor_sprite"}),
	_additionallySelectedTileCursorUL({ data.sink,
		data.name + "_addcursorul_sprite" }),
	_additionallySelectedTileCursorUR({ data.sink,
		data.name + "_addcursorur_sprite" }),
	_additionallySelectedTileCursorLL({ data.sink,
		data.name + "_addcursorll_sprite" }),
	_additionallySelectedTileCursorLR({ data.sink,
		data.name + "_addcursorlr_sprite" }) {
	assert(environments && weathers);
	_countries = countries;
	_environments = environments;
	_weathers = weathers;
	_tileTypes = tiles;
	_terrains = terrains;
	_unitTypes = units;
	_commanders = commanders;
	_structures = structures;
	// We need to make sure the map is in a valid state for the case where an empty
	// map is saved (i.e. where load() is never called to reset the state).
	_initState();
	_initShaders();
}

awe::map::~map() noexcept {
	if (_scripts && !_moduleName.empty()) _scripts->deleteModule(_moduleName);
}

bool awe::map::load(std::string file, const unsigned char version) {
	if (file == "") file = _filename;
	try {
		auto binaryData = _binaryIStreamFactory();
		{
			std::ifstream binaryFile(file, std::ios::binary);
			binaryFile >> binaryData;
		}
		_filename = file;
		const auto wereAnimationsEnabled = _animationsEnabled;
		enableAnimations(false);
		_loadMapFromInputStream(binaryData, version);
		enableAnimations(wereAnimationsEnabled);
		// Reinitialise memento data.
		_undoDeque.clear();
		_redoDeque.clear();
		_mementoDisableCounter = 0;
		// Always create the first memento, even if mementos are disabled,
		// otherwise the first memento given if they are enabled again wouldn't be
		// undoable.
		_createMemento(_getMementoName(awe::map_strings::operation::LOAD_MAP));
		_changed = false;
	} catch (const std::exception& e) {
		_logger.critical("Map loading operation: couldn't load map file \"{}\": "
			"{}", file, e);
		return false;
	}
	return true;
}

bool awe::map::save(std::string file, const unsigned char version) {
	if (file == "") file = _filename;
	try {
		engine::binary_ostream binaryData = _saveMapIntoOutputStream(version);
		{
			std::ofstream binaryFile(file, std::ios::binary);
			binaryFile << binaryData;
		}
		_filename = file;
		_changed = false;
	} catch (const std::exception& e) {
		_logger.critical("Map saving operation: couldn't save map file \"{}\": {}",
			file, e);
		return false;
	}
	return true;
}

bool awe::map::save(const std::string& file) {
	return save(file, 0);
}

void awe::map::setAdditionalData(const std::string& data) {
	if (data == _additionalData) return;
	awe::disable_mementos token(this,
		_getMementoName(awe::map_strings::operation::ADDITIONAL_DATA));
	_additionalData = data;
}

std::string awe::map::getAdditionalData() const {
	return _additionalData;
}

void awe::map::setScripts(const std::shared_ptr<engine::scripts>& scripts) {
	if (scripts) {
		if (_selectedUnitRenderData.empty()) {
			_selectedUnitRenderData.emplace(*scripts);
		}
	}
	_scripts = scripts;
}

void awe::map::setMapObject(CScriptHandle mapObject) {
	_mapObject = mapObject;
}

void awe::map::setMapObjectType(const std::string& typeName) {
	_mapObjectTypeName = typeName;
}

bool awe::map::hasChanged() const {
	return _changed;
}

bool awe::map::periodic() {
	if (!_periodicEnabled) return false;
	if (_mapObject.GetRef() && !_mapObjectTypeName.empty() && _scripts &&
		_scripts->functionDeclExists(_moduleName, "void periodic(" +
			_mapObjectTypeName + "@ const, bool&out, bool&out)")) {
		bool winConditionMet = false, carryOutDefaultWinConditionChecking = true;
		_scripts->callFunction(_moduleName, "periodic", _mapObject.GetRef(),
			&winConditionMet, &carryOutDefaultWinConditionChecking);
		if (!carryOutDefaultWinConditionChecking) return winConditionMet;
		if (winConditionMet) return winConditionMet;
	}
	return defaultWinCondition();
}

void awe::map::enablePeriodic(const bool enabled) {
	_periodicEnabled = enabled;
}

bool awe::map::defaultWinCondition() const {
	if (_armies.size() == 0) return true;
	const auto firstArmysTeam = _armies.begin()->second.getTeam();
	for (const auto& army : _armies)
		if (army.second.getTeam() != firstArmysTeam) return false;
	return true;
}

bool awe::map::beginTurnForOwnedTile(sf::Vector2u tile,
	awe::terrain* const terrain, const awe::ArmyID currentArmy) {
	if (!_mapObject.GetRef() || _mapObjectTypeName.empty() || !_scripts ||
		!_scripts->functionDeclExists(_moduleName,
			"void beginTurnForOwnedTile(" + _mapObjectTypeName + "@ const, "
			"Vector2, const Terrain@ const, ArmyID, bool&out)"))
		return false;
	bool overrideDefaultBehaviour = false;
	_scripts->callFunction(_moduleName, "beginTurnForOwnedTile",
		_mapObject.GetRef(), &tile, terrain, currentArmy,
		&overrideDefaultBehaviour);
	return overrideDefaultBehaviour;
}

void awe::map::undo(std::size_t additionalUndos) {
	if (_undoDeque.size() <= 1) return;
	const auto maxAdditionalUndos = _undoDeque.size() - 2;
	if (additionalUndos > maxAdditionalUndos) {
		_logger.warning("undo operation: additionalUndos was given {}, which is "
			"greater than the allowed number of undos at this time. "
			"additionalUndos will be set to {}.", additionalUndos,
			maxAdditionalUndos);
		additionalUndos = maxAdditionalUndos;
	}
	// Pop front memento/s of undo deque and push to the redo deque.
	for (std::size_t i = 0; i <= additionalUndos; ++i) {
		awe::map::memento popped = _undoDeque.front();
		_undoDeque.pop_front();
		_redoDeque.push_front(popped);
	}
	// Apply next memento in the undo deque.
	auto binaryData = _binaryIStreamFactory();
	*_undoDeque.front().data >> binaryData;
	_loadMapFromInputStream(binaryData, 0);
	_mementosHaveChanged();
}

void awe::map::redo(std::size_t additionalRedos) {
	if (_redoDeque.empty()) return;
	const auto maxAdditionalRedos = _redoDeque.size() - 1;
	if (additionalRedos > maxAdditionalRedos) {
		_logger.warning("redo operation: additionalRedos was given {}, which is "
			"greater than the allowed number of redos at this time. "
			"additionalRedos will be set to {}.", additionalRedos,
			maxAdditionalRedos);
		additionalRedos = maxAdditionalRedos;
	}
	// Pop front memento/s of redo deque and push to the undo deque.
	for (std::size_t i = 0; i <= additionalRedos; ++i) {
		awe::map::memento popped = _redoDeque.front();
		_redoDeque.pop_front();
		_undoDeque.push_front(popped);
	}
	// Apply first undo memento.
	auto binaryData = _binaryIStreamFactory();
	*_undoDeque.front().data >> binaryData;
	_loadMapFromInputStream(binaryData, 0);
	_mementosHaveChanged();
}


void awe::map::markChanged() {
	_changed = true;
}

std::vector<std::string> awe::map::getMementos(
	std::size_t& lastKnownMemento) const {
	std::vector<std::string> ret;
	// The back of the redo deque will hold the most recent memento, so iterate
	// through it backwards.
	for (auto i = _redoDeque.crbegin(), end = _redoDeque.crend(); i != end; ++i)
		ret.push_back(i->name);
	lastKnownMemento = ret.size();
	// The front of the undo deque will hold the most recent memento (if there are
	// none in the redo deque), so iterate through it forwards.
	for (const auto& memento : _undoDeque) ret.push_back(memento.name);
	return ret;
}

CScriptArray* awe::map::getMementosAsArray(std::size_t& lastKnownMemento) const {
	if (!_scripts) throw NO_SCRIPTS;
	const auto mementos = getMementos(lastKnownMemento);
	return _scripts->createArrayFromContainer("string", mementos);
}

std::string awe::map::getNextUndoMementoName() const {
	return _undoDeque.front().name;
}

std::string awe::map::getNextRedoMementoName() const {
	if (_redoDeque.empty()) return "";
	else return _redoDeque.front().name;
}

void awe::map::addMementoStateChangedCallback(asIScriptFunction* const callback) {
	if (callback) {
		_mementosChangedCallbacks.emplace_back(callback);
		callback->Release();
	} else {
		_logger.error("addMementoStateChangedCallback operation failed: cannot "
			"add NULL callback pointer!");
	}
}

void awe::map::addScriptFile(const std::string& name, const std::string& code) {
	awe::disable_mementos token(this, doesScriptExist(name) ?
		_getMementoName(awe::map_strings::operation::UPDATE_SCRIPT) :
		_getMementoName(awe::map_strings::operation::ADD_SCRIPT));
	_scriptFiles[name] = code;
}

void awe::map::renameScriptFile(const std::string& oldName,
	const std::string& newName) {
	if (oldName == newName) {
		_logger.error("renameScriptFile operation failed: new name \"{}\" given "
			"to replace old name \"{}\" are identical!", newName, oldName);
		return;
	}
	if (!doesScriptExist(oldName)) {
		_logger.error("renameScriptFile operation failed: could not locate script "
			"with old name \"{}\".", oldName);
		return;
	}
	if (doesScriptExist(newName)) {
		_logger.error("renameScriptFile operation failed: could not rename script "
			"with old name \"{}\" to new name \"{}\" as the new name is already "
			"taken!", oldName, newName);
		return;
	}
	awe::disable_mementos token(this,
		_getMementoName(awe::map_strings::operation::RENAME_SCRIPT));
	// https://stackoverflow.com/a/44883472.
	auto node = _scriptFiles.extract(oldName);
	node.key() = newName;
	_scriptFiles.insert(std::move(node));
}

void awe::map::removeScriptFile(const std::string& name) {
	if (!doesScriptExist(name)) {
		_logger.error("removeScriptFile operation failed: could not locate script "
			"with name \"{}\".", name);
		return;
	}
	awe::disable_mementos token(this,
		_getMementoName(awe::map_strings::operation::REMOVE_SCRIPT));
	_scriptFiles.erase(name);
}

std::string awe::map::buildScriptFiles() {
	if (!_scripts) throw NO_SCRIPTS;
	std::string newModuleName = getMapName() + ":map";
	// Make a copy so that the class declaration is not inserted multiple times. It
	// will also stay hidden from the user.
	engine::scripts::files scriptFiles = _scriptFiles;
	if (!scriptFiles.empty() && !_mapObjectTypeName.empty()) {
		scriptFiles.begin()->second.insert(0,
			"external shared class " + _mapObjectTypeName + ";");
	}
	const bool success =
		_scripts->createModule(newModuleName, scriptFiles, _lastKnownBuildResult);
	if (success && !_moduleName.empty() && newModuleName != _moduleName) {
		// The map was renamed since the last build, so the old module still
		// exists. Delete it!
		_scripts->deleteModule(_moduleName);
	}
	if (success) {
		_moduleName = newModuleName;
		_lastKnownBuildResult.clear();
	}
	return _lastKnownBuildResult;
}

std::string awe::map::getLastKnownBuildResult() const {
	return _lastKnownBuildResult;
}

std::string awe::map::getScript(const std::string& name) const {
	if (doesScriptExist(name)) return _scriptFiles.at(name);
	_logger.error("getScript operation failed: could not locate script with name "
		"\"{}\".", name);
	return "";
}

std::vector<std::string> awe::map::getScriptNames() const {
	std::vector<std::string> names;
	std::transform(_scriptFiles.begin(), _scriptFiles.end(),
		std::back_inserter(names),
		[](const std::pair<std::string, std::string>& p) { return p.first; });
	return names;
}

CScriptArray* awe::map::getScriptNamesAsArray() const {
	if (!_scripts) throw NO_SCRIPTS;
	const auto scriptNames = getScriptNames();
	return _scripts->createArrayFromContainer("string", scriptNames);
}

void awe::map::setSpritesheets(
	const std::shared_ptr<sfx::animated_spritesheets>& sheets) {
	_sheets = sheets;
	// This will need improving in the future, C++ code shouldn't be referencing
	// spritesheets in this way. TODO-2.
	_cursor.setSpritesheet((*_sheets)["icon"]);
	_additionallySelectedTileCursorUL.setSpritesheet((*_sheets)["icon"]);
	_additionallySelectedTileCursorUR.setSpritesheet((*_sheets)["icon"]);
	_additionallySelectedTileCursorLL.setSpritesheet((*_sheets)["icon"]);
	_additionallySelectedTileCursorLR.setSpritesheet((*_sheets)["icon"]);
	for (auto& unit : _units) {
		unit.second.sprite->setSpritesheet((*_sheets)[
			unit.second.data.getType()->getIdleSpritesheet()]);
		unit.second.sprite->setIconSpritesheet((*_sheets)["icon"]);
	}
	_regenerateTileSprites();
}

void awe::map::setFonts(const std::shared_ptr<sfx::fonts>& fonts) {
	if (!fonts) {
		_logger.error("setFonts operation failed: nullptr was given!");
		return;
	}
	_fonts = fonts;
}

void awe::map::setAudios(const std::shared_ptr<sfx::audios>& audios) {
	if (!audios) {
		_logger.error("setAudios operation failed: nullptr was given!");
		return;
	}
	_audios = audios;
}

void awe::map::setGUI(const std::shared_ptr<sfx::gui>& gui) {
	if (!gui) {
		_logger.error("setGUI operation failed: nullptr was given!");
		return;
	}
	_gui = gui;
}

void awe::map::setLanguageDictionary(
	const std::shared_ptr<engine::language_dictionary>& dict) {
	if (dict) _dict = dict;
	else _logger.error("Attempted to set NULL language dictionary!");
}

void awe::map::setMapStrings(const std::shared_ptr<awe::map_strings>& strs) {
	if (strs) _mapStrings = strs;
	else _logger.error("setMapStrings operation failed: nullptr was given!");
}

void awe::map::setUserInput(const std::shared_ptr<sfx::user_input>& ui) {
	if (ui) _ui = ui;
	else _logger.error("setUserInput operation failed: nullptr was given!");
}

void awe::map::_updateCapturingUnit(const awe::UnitID id) {
	if (id != awe::NO_UNIT && isUnitCapturing(id)) {
		const auto t = getUnitPosition(id);
		// If unit is out-of-bounds, don't do anything. This case can come about
		// when a capturing unit is deleted as a map is shrinking.
		if (!_isOutOfBounds(t)) setTileHP(t, static_cast<awe::HP>(
			getTileType(t)->getType()->getMaxHP()));
		unitCapturing(id, false);
	}
}

awe::UnitID awe::map::_findUnitID() {
	if (_units.size() == 0) return _lastUnitID;
	// Minus 1 to account for the reserved value, NO_UNIT.
	if (_units.size() == std::numeric_limits<awe::UnitID>::max() - 1) 
		throw std::bad_alloc();
	awe::UnitID temp = _lastUnitID + 1;
	while (_isUnitPresent(temp)) {
		if (temp == std::numeric_limits<awe::UnitID>::max())
			temp = awe::ID_OF_FIRST_UNIT;
		else ++temp;
	}
	_lastUnitID = temp;
	return temp;
}

void awe::map::_loadMapFromInputStream(engine::binary_istream& stream,
	const unsigned char version) {
	if (!_scripts) {
		throw std::runtime_error("no scripts object!");
	} else if (!_scripts->functionDeclExists(_scripts->modules[_scripts->MAIN],
		"void LoadMap(BinaryIStream@, Map@, uint8)")) {
		throw std::runtime_error("void LoadMap(BinaryIStream@, Map@, uint8) "
			"not found in the scripts!");
	}
	// Clear state (excluding mementos).
	_initState();
	// Load state.
	_mementoHardDisable = true;
	_scripts->callFunction(_scripts->modules[_scripts->MAIN], "LoadMap", &stream,
		this, version);
	_mementoHardDisable = false;
}

engine::binary_ostream awe::map::_saveMapIntoOutputStream(
	const unsigned char version) {
	if (!_scripts) {
		throw std::runtime_error("no scripts object!");
	} else if (!_scripts->functionDeclExists(_scripts->modules[_scripts->MAIN],
		"void SaveMap(BinaryOStream@, Map@, uint8)")) {
		throw std::runtime_error("void SaveMap(BinaryOStream@, Map@, uint8) "
			"not found in the scripts!");
	}
	engine::binary_ostream stream({ _logger.getData().sink,
		_logger.getData().name + "_binary_ostream" });
	_mementoHardDisable = true;
	_scripts->callFunction(_scripts->modules[_scripts->MAIN], "SaveMap", &stream,
		this, version);
	_mementoHardDisable = false;
	return stream;
}

void awe::map::_createMemento(const std::string& name) {
	if (_mementoHardDisable) return;
	_undoDeque.push_front({ std::make_shared<engine::binary_ostream>(
		_saveMapIntoOutputStream(0)), name });
	_redoDeque.clear();
	if (_undoDeque.size() > _MEMENTO_LIMIT) _undoDeque.pop_back();
	markChanged();
	_mementosHaveChanged();
}

awe::map::selected_unit_render_data::selected_unit_render_data(
	const engine::scripts& scripts) {
	closedList = scripts.createArray("ClosedListNode");
}

awe::map::selected_unit_render_data::selected_unit_render_data(
	const awe::map::selected_unit_render_data& o) {
	*this = o;
}

awe::map::selected_unit_render_data::selected_unit_render_data(
	awe::map::selected_unit_render_data&& o) noexcept {
	selectedUnit = std::move(o.selectedUnit);
	availableTiles = std::move(o.availableTiles);
	availableTileShader = std::move(o.availableTileShader);
	closedList = std::move(o.closedList);
	if (closedList) closedList->AddRef();
	disableRenderingEffects = std::move(o.disableRenderingEffects);
	disableShaderForAvailableUnits = std::move(o.disableShaderForAvailableUnits);
}

awe::map::selected_unit_render_data::~selected_unit_render_data() noexcept {
	if (closedList) closedList->Release();
}

awe::map::selected_unit_render_data&
	awe::map::selected_unit_render_data::operator=(
	const awe::map::selected_unit_render_data& o) {
	selectedUnit = o.selectedUnit;
	availableTiles = o.availableTiles;
	availableTileShader = o.availableTileShader;
	closedList = o.closedList;
	if (closedList) closedList->AddRef();
	disableRenderingEffects = o.disableRenderingEffects;
	disableShaderForAvailableUnits = o.disableShaderForAvailableUnits;
	return *this;
}

void awe::map::selected_unit_render_data::clearState() {
	selectedUnit = awe::NO_UNIT;
	availableTiles.clear();
	availableTileShader = awe::available_tile_shader::None;
	closedList->RemoveRange(0, closedList->GetSize());
	disableRenderingEffects = false;
	disableShaderForAvailableUnits = false;
}

void awe::map::_initState() {
	_sel = sf::Vector2u(0, 0);
	_currentArmy = awe::NO_ARMY;
	_lastUnitID = awe::ID_OF_FIRST_UNIT;
	_armies.clear();
	_units.clear();
	_unitsBeingDestroyed.clear();
	_tiles.clear();
	_mapName.clear();
	_day = 1;
	_fow = false;
	_viewOffsetX.reset();
	_viewOffsetY.reset();
	_mapSizeCache = { 0, 0 };
	_scriptFiles.clear();
	removeAllPreviewUnits();
	_mapShakeTimeLeft = sf::Time::Zero;
	_waitBeforeNextShake = sf::Time::Zero;
	_environment = _environments->first()->second;
	_setWeather(_weathers->first()->second);
	_additionalData.clear();
	if (_scripts && _scripts->doesModuleExist(_moduleName))
		_scripts->deleteModule(_moduleName);
	_moduleName.clear();
}

void awe::map::_initShaders() {
	_rectangle.setFillColor(sf::Color::Transparent);
	_rectangle.setOutlineColor(sf::Color::Red);
	_unavailableTileShader.loadFromMemory("uniform sampler2D texUnit;"
		"void main() {vec4 pixel = texture2D(texUnit, gl_TexCoord[0].xy);"
		"pixel.xyz /= 2.0; gl_FragColor = pixel;}", sf::Shader::Fragment);
	_unavailableTileShader.setUniform("texUnit", sf::Shader::CurrentTexture);
	_availableTileShader.loadFromMemory("uniform sampler2D texUnit;"
		"void main() {vec4 pixel = texture2D(texUnit, gl_TexCoord[0].xy);"
		"pixel.xy *= 1.1; gl_FragColor = pixel;}", sf::Shader::Fragment);
	_availableTileShader.setUniform("texUnit", sf::Shader::CurrentTexture);
	_attackableTileShader.loadFromMemory("uniform sampler2D texUnit;"
		"void main() {vec4 pixel = texture2D(texUnit, gl_TexCoord[0].xy);"
		"pixel.x = 1.0; pixel.yz -= 0.25;"
		"gl_FragColor = pixel;}", sf::Shader::Fragment);
	_attackableTileShader.setUniform("texUnit", sf::Shader::CurrentTexture);

	_hiddenTileShaderFoW.loadFromMemory("uniform sampler2D texUnit;"
		"void main() {vec4 pixel = texture2D(texUnit, gl_TexCoord[0].xy);"
		// First, apply FoW effect.
		"pixel.xyz = (pixel.x + pixel.y + pixel.z) / 3.0;" // B&W.
		"pixel.y *= 0.7;"                                  // Purple tint.
		// Then, apply our effect.
		"gl_FragColor = pixel;}", sf::Shader::Fragment);
	_hiddenTileShaderFoW.setUniform("texUnit", sf::Shader::CurrentTexture);
	_unavailableTileShaderFoW.loadFromMemory("uniform sampler2D texUnit;"
		"void main() {vec4 pixel = texture2D(texUnit, gl_TexCoord[0].xy);"
		// First, apply FoW effect.
		"pixel.xyz = (pixel.x + pixel.y + pixel.z) / 3.0;" // B&W.
		"pixel.y *= 0.7;"                                  // Purple tint.
		// Then, apply our effect.
		"pixel.xyz /= 2.0; gl_FragColor = pixel;}", sf::Shader::Fragment);
	_unavailableTileShaderFoW.setUniform("texUnit", sf::Shader::CurrentTexture);
	_availableTileShaderFoW.loadFromMemory("uniform sampler2D texUnit;"
		"void main() {vec4 pixel = texture2D(texUnit, gl_TexCoord[0].xy);"
		// First, apply FoW effect.
		"pixel.xyz = (pixel.x + pixel.y + pixel.z) / 3.0;" // B&W.
		"pixel.y *= 0.7;"                                  // Purple tint.
		// Then, apply our effect.
		"pixel.xy *= 1.1; gl_FragColor = pixel;}", sf::Shader::Fragment);
	_availableTileShaderFoW.setUniform("texUnit", sf::Shader::CurrentTexture);
	_attackableTileShaderFoW.loadFromMemory("uniform sampler2D texUnit;"
		"void main() {vec4 pixel = texture2D(texUnit, gl_TexCoord[0].xy);"
		// First, apply FoW effect.
		"pixel.xyz = (pixel.x + pixel.y + pixel.z) / 3.0;" // B&W.
		"pixel.y *= 0.7;"                                  // Purple tint.
		// Then, apply our effect.
		"pixel.x = 1.0; pixel.yz -= 0.25;"
		"gl_FragColor = pixel;}", sf::Shader::Fragment);
	_attackableTileShaderFoW.setUniform("texUnit", sf::Shader::CurrentTexture);
}
