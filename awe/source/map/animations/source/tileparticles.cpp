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

#include "../include/tileparticles.hpp"

void awe::tile_particle_node::Register(asIScriptEngine* engine,
	const std::shared_ptr<DocumentationGenerator>& document) {
	if ((!engine->GetTypeInfoByName("TileParticle"))) {
		engine::RegisterVectorTypes(engine, document);

		auto r = RegisterType(engine, "TileParticle",
			[](asIScriptEngine* engine, const std::string& type) {
				// We have to declare the default factory explicitly.
				engine->RegisterObjectBehaviour(type.c_str(), asBEHAVE_FACTORY,
					std::string(type + "@ f()").c_str(),
					asFUNCTIONPR(awe::tile_particle_node::Create, (),
						awe::tile_particle_node*), asCALL_CDECL);
				engine->RegisterObjectBehaviour(type.c_str(), asBEHAVE_FACTORY,
					std::string(type + "@ f("
						"const Vector2&in = Vector2(0, 0),"
						"const string&in = \"\","
						"const Vector2f&in = Vector2f(0.0, 0.0),"
						"const string&in = \"\","
						"const float = 0.0"
					")").c_str(),
					asFUNCTIONPR(awe::tile_particle_node::Create,
						(const sf::Vector2u&, const std::string&,
							const sf::Vector2f&, const std::string&,
						const float), awe::tile_particle_node*), asCALL_CDECL);
			});
		document->DocumentObjectType(r, "Holds information on a tile particle.");

		r = engine->RegisterObjectProperty("TileParticle", "Vector2 tile",
			asOFFSET(awe::tile_particle_node, tile));

		r = engine->RegisterObjectProperty("TileParticle", "string particle",
			asOFFSET(awe::tile_particle_node, particle));

		r = engine->RegisterObjectProperty("TileParticle", "Vector2f origin",
			asOFFSET(awe::tile_particle_node, origin));

		r = engine->RegisterObjectProperty("TileParticle", "string spriteOverride",
			asOFFSET(awe::tile_particle_node, spriteOverride));

		r = engine->RegisterObjectProperty("TileParticle", "float delay",
			asOFFSET(awe::tile_particle_node, delay));
	}
}

awe::tile_particle_node::tile_particle_node(const sf::Vector2u& t,
	const std::string& p, const sf::Vector2f& o, const std::string& s,
	const float d) {
	tile = t;
	particle = p;
	origin = o;
	spriteOverride = s;
	delay = d;
}

awe::tile_particle_node* awe::tile_particle_node::Create() {
	return new awe::tile_particle_node();
}

awe::tile_particle_node* awe::tile_particle_node::Create(const sf::Vector2u& t,
	const std::string& p, const sf::Vector2f& o, const std::string& s,
	const float d) {
	return new awe::tile_particle_node(t, p, o, s, d);
}

bool awe::tile_particle_node::animate(const sf::RenderTarget& target) {
	// Perform delay first.
	const auto delta = accumulatedDelta();
	if (delay > 0.f && delta < delay) return false;
	// Now animate particle and perform sprite override.
	if (!spriteOverride.empty()) tileSprite->setSpriteOverride(spriteOverride);
	const auto ret = particleSprite.animate(target);
	const auto particleSize = particleSprite.getSize();
	particleSprite.setOrigin(sf::Vector2f(particleSize.x * origin.x,
		particleSize.y * origin.y));
	const auto tileSpriteSize = tileSprite->getPixelSize();
	auto tileSpritePos = tileSprite->getPixelPosition();
	tileSpritePos.x += tileSpriteSize.x * origin.x;
	tileSpritePos.y += tileSpriteSize.y * origin.y;
	particleSprite.setPosition(tileSpritePos);
	if (ret) tileSprite->clearSpriteOverride();
	return ret;
}

void awe::tile_particle_node::draw(sf::RenderTarget& target,
	sf::RenderStates states) const {
	// Will remain invisible so long as particleSprite.animate() is not called.
	target.draw(particleSprite, states);
}

awe::tile_particles::tile_particles(
	const std::vector<awe::tile_particle_node>& particles,
	const std::shared_ptr<sfx::animated_spritesheet>& sheet) :
	_particles(particles) {
	for (auto& particle : _particles) {
		assert(particle.tileSprite);
		particle.particleSprite.setSpritesheet(sheet);
		particle.particleSprite.setSprite(particle.particle);
	}
}

bool awe::tile_particles::animate(const sf::RenderTarget& target) {
	std::vector<std::size_t> finishedParticles;
	for (std::size_t i = 0, len = _particles.size(); i < len; ++i)
		if (_particles[i].animate(target)) finishedParticles.push_back(i);
	for (auto itr = finishedParticles.rbegin(), enditr = finishedParticles.rend();
		itr != enditr; ++itr) _particles.erase(_particles.begin() + *itr);
	return _particles.empty();
}

void awe::tile_particles::draw(sf::RenderTarget& target,
	sf::RenderStates states) const {
	for (const auto& particle : _particles) target.draw(particle, states);
}
