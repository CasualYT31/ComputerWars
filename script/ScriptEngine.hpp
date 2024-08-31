/**
 * \file ScriptEngine.hpp
 * \brief Declares the interface that script engine managers must implement to be able to interact with the script model.
 */

#pragma once

#include "helper/Types.hpp"
#include "mvc/Request.hpp"

#include <filesystem>
#include <string>

namespace cw {
/**
 * \brief The interface that abstracts away from script engines.
 * \tparam M Module identifiers will be of this type.
 */
template <typename M> class ScriptEngine {
public:
    virtual ~ScriptEngine() = default;

    /**
     * \brief Module identifiers will be of the type given by the subclass.
     */
    using ModuleName = M;

    /**
     * \brief Creates a new module that will contain executable code.
     * \param moduleName The name to give to the module.
     * \param discardIfExists True if the module should be discard if it exists, false if the method should fail if the
     * module already exists.
     * \returns True if the module was created successfully, false otherwise.
     */
    virtual bool createModule(Param<ModuleName>::In moduleName, const bool discardIfExists) = 0;

    /**
     * \brief Loads executable code into an existing module.
     * \details The implementation is permitted to discard or otherwise reset the module if it already exists.
     * \param moduleName The name of the module to load.
     * \param createIfDoesNotExist True if the module should be created if it doesn't exist, false if the method should fail
     * if the module exists.
     * \param scriptsFolder Path to a folder containing scripts to load into the module.
     * \param filepathRegexFilter Only load the file as a script if its full path matches with this filter.
     * \returns True if the module was loaded successfully, false otherwise.
     */
    virtual bool loadModule(
        Param<ModuleName>::In moduleName,
        const bool createIfDoesNotExist,
        const std::filesystem::path& scriptsFolder,
        const std::string& filepathRegexFilter
    ) = 0;

    /**
     * \brief Discard a module.
     * \param moduleName The name of the module to discard.
     * \returns True if the module was discarded successfully, false otherwise.
     */
    virtual bool discardModule(Param<ModuleName>::In moduleName) = 0;

    /**
     * \brief Discards all executable code that's currently loaded within the engine.
     * \returns True if all modules were discarded successfully, false if even one wasn't discarded.
     */
    virtual bool discardAllModules() = 0;

    /**
     * \brief Count the number of modules currently loaded.
     * \returns The number of modules currently loaded.
     */
    virtual std::size_t getModuleCount() const = 0;

    /**
     * \brief Generates a list containing the name of each module that currently exists in the engine.
     * \returns A list of module names.
     */
    virtual std::vector<ModuleName> getModuleNames() const = 0;

    /**
     * \brief Finds out if a module with the given name exists.
     * \param moduleName The name to search for.
     * \returns True if the module exists, false if not.
     */
    virtual bool moduleExists(Param<ModuleName>::In moduleName) const = 0;

    /**
     * \brief Sets up the documentation generator.
     * \param documentationOutputFile The name of the HTML file outputted.
     * \returns True if set up was successful, false otherwise (or if the generator has already been set up).
     */
    virtual bool setUpDocumentationGenerator(const std::string& documentationOutputFile) = 0;

    /**
     * \brief Does this script engine have a documentation generator set up?
     * \returns True if so, false otherwise.
     */
    virtual bool documentationGeneratorIsSetUp() const = 0;

    /**
     * \brief Generates the documentation that details the interface between the game engine and the scripts.
     * \returns True if successful, false otherwise.
     */
    virtual bool generateDocumentation() = 0;
};

/**
 * \brief Concept that checks if a class implements the ScriptEngine interface.
 * \details Thanks go to https://stackoverflow.com/a/71921982.
 * \tparam M The ModuleName type.
 */
template <typename M>
concept IsScriptEngine = requires(M m) { []<typename E>(ScriptEngine<E>&) {}(m); };
} // namespace cw
