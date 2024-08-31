/**
 * \file AngelScriptEngine.hpp
 * \brief The main interface between AngelScript and the ScriptEngine model.
 */

#pragma once

#include "script/ScriptEngine.hpp"

#include <angelscript.h>
#include <docgen.h>
#include <memory>
#include <scriptbuilder.h>
#include <unordered_set>

namespace cw {
/**
 * \brief Manages AngelScript.
 */
class AngelScriptEngine : public ScriptEngine<std::string> {
public:
    /**
     * \brief Sets up the AngelScript engine.
     */
    AngelScriptEngine();

    /**
     * \brief Tears down the AngelScript engine.
     */
    ~AngelScriptEngine() noexcept;

    bool createModule(Param<ModuleName>::In moduleName, const bool discardIfExists) final;
    bool loadModule(
        Param<ModuleName>::In moduleName,
        const bool createIfDoesNotExist,
        const std::filesystem::path& scriptsFolder,
        const std::string& filepathRegexFilter
    ) final;
    bool discardModule(Param<ModuleName>::In moduleName) final;
    bool discardAllModules() final;
    std::size_t getModuleCount() const final;
    std::vector<ModuleName> getModuleNames() const final;
    bool moduleExists(Param<ModuleName>::In moduleName) const final;
    bool setUpDocumentationGenerator(const std::string& documentationOutputFile) final;
    bool documentationGeneratorIsSetUp() const final;
    bool generateDocumentation() final;

private:
    /**
     * \brief Points to the AngelScript engine.
     */
    ::asIScriptEngine* const _engine = nullptr;

    /**
     * \brief The module builder.
     */
    ::CScriptBuilder _builder;

    /**
     * \brief Points to the AngelScript documentation generator.
     */
    std::unique_ptr<::DocumentationGenerator> _documentationGenerator;
};
} // namespace cw
