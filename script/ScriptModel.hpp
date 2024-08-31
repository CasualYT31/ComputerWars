/**
 * \file ScriptModel.hpp
 * \brief Declares the model that stores and manages external scripts.
 */

#pragma once

#include "log/Log.hpp"
#include "mvc/Model.hpp"
#include "ScriptEngine.hpp"

#include <regex>

namespace cw {
/**
 * \brief Instruct the script model to generate script interface documentation.
 */
class GenerateDocumentationRequest : public Request {
public:
    /**
     * \brief True will be returned from the request if generation was successful, false otherwise.
     */
    using ReturnType = bool;
};

/**
 * \brief Manages external scripts.
 * \tparam Engine The type of script engine to construct within this model.
 */
template <IsScriptEngine Engine> class ScriptModel : public Model {
public:
    /**
     * \brief Sets up the script engine that drives the script model.
     * \param documentationOutputFile If the documentation of the script interface should be generated, this should be the
     * name of the HTML file outputted. No documentation will be generated if this parameter is empty.
     */
    ScriptModel(const std::string& documentationOutputFile = "") : _engine(std::make_unique<Engine>()) {
        ASSERT(_engine, "Script engine failed to load.");
        if (!documentationOutputFile.empty()) { _engine->setUpDocumentationGenerator(documentationOutputFile); }
    }

    /**
     * \copydoc Model::registerModel
     * \warning This model must be registered before any model that adds to the script interface.
     */
    void registerModel(const std::shared_ptr<ReadWriteController>& controller) final {
        REGISTER(controller, Request, GenerateDocumentationRequest, ScriptModel::_generateDocumentation, this);
    }

    /**
     * \brief Deletes every module currently loaded and loads new ones based on the contents of the given JSON.
     * \copydetails JSONSerialised::fromJSON
     */
    void fromJSON(const json& j) final {
        _lastKnownJSON = j;
        LOG(info, "Loading new script modules, discarding old ones");
        _engine->discardAllModules();
        LOG(info, "Previous script modules discarded");
        if (!j.contains("modules") || !j.at("modules").is_object()) {
            LOG(warn,
                "The \"modules\" object could not be found within the script model object, no script modules will be loaded!"
            );
            return;
        }
        for (const auto& m : j.at("modules").items()) {
            const std::string moduleName = m.key();
            if (!m.value().contains("folder") || !m.value().at("folder").is_string()) {
                LOG(err, "The \"{}\" module has no \"folder\" string, this module will not be loaded", moduleName);
                continue;
            }
            const std::string folder = m.value().at("folder");
            std::string filter = "^.*\\.as$";
            if (m.value().contains("filter") && m.value().at("filter").is_string()) {
                const std::string proposedFilter = m.value().at("filter");
                try {
                    const auto _ = std::regex(proposedFilter);
                    filter = proposedFilter;
                } catch (const std::exception& e) {
                    LOG(warn,
                        "Attempted to apply filepath filter {} to module \"{}\", which is invalid regex: {}. Will use the "
                        "default filter, {}",
                        proposedFilter,
                        moduleName,
                        e,
                        filter);
                }
            }
            LOG(info, "Loading module \"{}\" with scripts found in folder {}, filtering with {}", moduleName, folder, filter
            );
            const auto result = _engine->loadModule(moduleName, true, folder, filter);
            if (result) {
                LOG(info, "Successfully loaded module \"{}\"", moduleName);
            } else {
                LOG(err, "Failed to load module \"{}\"", moduleName);
            }
        }
    }

    /**
     * \brief Regurgitates the JSON that was last given to the model.
     * \copydetails JSONSerialised::toJSON
     */
    void toJSON(json& j) const final {
        j = _lastKnownJSON;
    }

private:
    /// \cond

    DECLARE_REQUEST(_generateDocumentation) {
        RECEIVE_REQUEST(GenerateDocumentationRequest);
        return _engine->documentationGeneratorIsSetUp() && _engine->generateDocumentation();
    }

    /// \endcond

    /**
     * \brief Caches the JSON that was last given to fromJSON().
     */
    cw::json _lastKnownJSON;

    /**
     * \brief Points to the underlying script engine.
     */
    const std::unique_ptr<Engine> _engine;
};
} // namespace cw
