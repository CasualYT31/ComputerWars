#include "AngelScriptEngine.hpp"

#include "file/File.hpp"
#include "log/Log.hpp"

#include <filesystem>
#include <regex>
#include <scriptarray.h>
#include <scripthelper.h>
#include <scriptstdstring.h>

namespace {
/**
 * \brief Invoked by the script engine when it wishes to write a log message.
 * \param msg Contains the message's details.
 * \param param Unused.
 */
void scriptMessageCallback(const ::asSMessageInfo* msg, void* param) {
    if (msg->type == ::asMSGTYPE_INFORMATION) {
        LOG(info, "{}", *msg);
    } else if (msg->type == ::asMSGTYPE_WARNING) {
        LOG(warn, "{}", *msg);
    } else if (msg->type == ::asMSGTYPE_ERROR) {
        LOG(err, "{}", *msg);
    }
}

/**
 * \brief If a C++ exception is thrown whilst executing script code, throw it within the scripts, too.
 * \param context The context that is executing the code that threw the C++ exception.
 * \param param Unused.
 */
void scriptTranslateExceptionCallback(::asIScriptContext* context, void* param) noexcept {
    // https://www.angelcode.com/angelscript/sdk/docs/manual/doc_cpp_exceptions.html.
    try {
        throw;
    } catch (const std::exception& e) {
        if (context) {
            context->SetException(e.what());
        } else {
            // This should be critical, but there could be cases where the scripts are continuously causing C++ exceptions,
            // so it's best to avoid infinite dialog box hell.
            LOG(err,
                "An exception was thrown whilst executing script code, but the context pointer was null! The exception: {}",
                e);
        }
    } catch (...) {}
}

/**
 * \brief Converts a section name to be a relative path if it is a full path.
 * \param sectionName The section name to convert.
 * \returns The section name as a relative path, if it is a filepath. sectionName otherwise.
 */
inline std::string makeSectionNameRelativeIfItIsAPath(const std::string& sectionName) noexcept {
    try {
        // If the section name is a filepath, change it to be a relative path.
        return std::filesystem::relative(sectionName).string();
    } catch (...) {
        // If the section name is not a filepath, just print it all.
        return sectionName;
    }
}

/**
 * \brief Invoked by the script engine when it encounters an exception.
 * \param context The context that is executing the code that threw the exception.
 * \param param Unused.
 */
void scriptExceptionCallback(::asIScriptContext* context, void* param) {
    ASSERT(context, "The scripts threw an exception, but the context pointer was null!");
    const auto function = context->GetExceptionFunction();
    ASSERT(function, "The scripts threw an exception, but the function pointer was null!");
    int column = 0;
    const int row = context->GetExceptionLineNumber(&column);
    LOG(err,
        "Script exception: {} [{}:{}:{},{}]",
        context->GetExceptionString(),
        ::makeSectionNameRelativeIfItIsAPath(function->GetScriptSectionName()),
        function->GetDeclaration(true, true, true),
        row,
        column);
}
} // namespace

namespace fmt {
/**
 * \brief FMT formatter for asSMessageInfo.
 */
template <> struct formatter<::asSMessageInfo> {
    /**
     * \brief Parses this object's format string.
     * \details This type of object uses the default "{}" format string.
     * \param ctx The format parse context that contains the format string.
     * \returns The beginning of the format parse context.
     */
    constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) {
        auto it = ctx.begin(), end = ctx.end();
        if (it != end && *it != '}') throw format_error("invalid format");
        return it;
    }

    /**
     * \brief Formats the object.
     * \tparam FormatContext Type of the object that receives the formatted object.
     * \param o The object to format.
     * \param ctx The object receiving the formatted object.
     * \returns Internal output stream within the format context object.
     */
    template <typename FormatContext> auto format(const asSMessageInfo& o, FormatContext& ctx) const -> decltype(ctx.out()) {
        return fmt::format_to(
            ctx.out(), "{} [{}:{},{}]", o.message, ::makeSectionNameRelativeIfItIsAPath(o.section), o.row, o.col
        );
    }
};
} // namespace fmt

namespace cw {
AngelScriptEngine::AngelScriptEngine() : _engine(::asCreateScriptEngine()) {
    ASSERT(_engine, "The AngelScript engine failed to load.");
    LOG(debug, "Allocated the AngelScript engine");
    auto r = _engine->SetMessageCallback(asFUNCTION(::scriptMessageCallback), nullptr, asCALL_CDECL);
    ASSERT(r >= 0, "Failed to set the script message callback, code {}", r);
    LOG(trace, "Registered the message callback with the AngelScript engine");
    r = _engine->SetTranslateAppExceptionCallback(asFUNCTION(::scriptTranslateExceptionCallback), nullptr, asCALL_CDECL);
    ASSERT(r >= 0, "Failed to set script exception callback, code {}", r);
    LOG(trace, "Registered exception callback with the AngelScript engine");
    ::RegisterStdString(_engine);
    LOG(trace, "Registered string type with the AngelScript engine interface");
    ::RegisterScriptArray(_engine, false);
    LOG(trace, "Registered array template type with the AngelScript engine interface");
    ::RegisterStdStringUtils(_engine);
    LOG(trace, "Registered string utility functions with the AngelScript engine interface");
    ::RegisterExceptionRoutines(_engine);
    LOG(trace, "Registered exception routines with the AngelScript engine interface");
}

AngelScriptEngine::~AngelScriptEngine() noexcept {
    if (_engine) { _engine->ShutDownAndRelease(); }
}

bool AngelScriptEngine::createModule(Param<ModuleName>::In moduleName, const bool discardIfExists) {
    LOG(debug, "Creating module \"{}\"", moduleName);
    if (moduleExists(moduleName)) {
        if (discardIfExists) {
            LOG(warn,
                "Discarding module \"{}\" since it already exists, replacing with a new module that has the same name",
                moduleName);
        } else {
            LOG(err, "Tried to create a module with name \"{}\" that already exists", moduleName);
            return false;
        }
    }
    const auto flags = discardIfExists ? ::asGM_ALWAYS_CREATE : ::asGM_CREATE_IF_NOT_EXISTS;
    if (!_engine->GetModule(moduleName.c_str(), flags)) {
        LOG(err, "Could not create new module \"{}\"", moduleName);
        return false;
    }
    LOG(debug, "Created module \"{}\"", moduleName);
    return true;
}

bool AngelScriptEngine::loadModule(
    Param<ModuleName>::In moduleName,
    const bool createIfDoesNotExist,
    const std::filesystem::path& scriptsFolder,
    const std::string& filepathRegexFilter
) {
    const auto exists = moduleExists(moduleName);
    if (!createIfDoesNotExist && exists) {
        LOG(err,
            "Tried to override an existing module \"{}\" with scripts from the folder {}, using the filepath filter {}",
            moduleName,
            scriptsFolder,
            filepathRegexFilter);
        return false;
    }
    if (exists) {
        LOG(warn,
            "Discarding existing module \"{}\" and replacing its code with scripts from the folder {}, using the filepath "
            "filter {}",
            moduleName,
            scriptsFolder,
            filepathRegexFilter);
    } else {
        LOG(debug,
            "Creating new module \"{}\" and loading scripts from the folder {} that match the filter {} into it",
            moduleName,
            scriptsFolder,
            filepathRegexFilter);
    }
    auto r = _builder.StartNewModule(_engine, moduleName.c_str());
    if (r < 0) {
        const auto stillExists = moduleExists(moduleName);
        LOG(err,
            "Could not start new module \"{}\"{}, code: {}",
            moduleName,
            (stillExists ? "" : ", and the previously existing one was discarded"),
            r);
        return false;
    }
    std::optional<std::regex> regex;
    try {
        regex = std::regex(filepathRegexFilter);
    } catch (const std::exception& e) {
        LOG(warn, "The filepath filter {} is invalid regex: {}. Will not apply any filepath filter", filepathRegexFilter, e);
    }
    iterateDirectory(
        scriptsFolder,
        [&builder = _builder, &filepathRegexFilter, &regex](const std::filesystem::directory_entry& entry) -> bool {
            LOG(trace, "Found file {}", entry);
            if (regex && !std::regex_match(entry.path().generic_string(), *regex, std::regex_constants::match_any)) {
                LOG(debug, "Ignoring file {} that doesn't match filter {}", entry, filepathRegexFilter);
                return true;
            }
            LOG(debug, "Adding file {} to the module", entry);
            const auto r = builder.AddSectionFromFile(entry.path().generic_string().c_str());
            if (r < 0) {
                LOG(err, "Failed to add file {} to the module, code {}", entry, r);
                return false;
            }
            LOG(info, "Added file {} to the module", entry);
            return true;
        }
    );
    if ((r = _builder.BuildModule()) < 0) {
        LOG(err,
            "Failed to build the module \"{}\" using scripts that match the {} filter within the {} folder, code {}",
            moduleName,
            filepathRegexFilter,
            scriptsFolder,
            r);
        return false;
    }
    LOG(debug, "Finished loading module \"{}\"", moduleName);
    return true;
}

bool AngelScriptEngine::discardModule(Param<ModuleName>::In moduleName) {
    LOG(debug, "Discarding AngelScript module \"{}\"", moduleName);
    const auto r = _engine->DiscardModule(moduleName.c_str());
    if (r < 0) {
        LOG(err, "Could not discard AngelScript module \"{}\", code: {}", moduleName, r);
        return false;
    }
    LOG(debug, "Discarded AngelScript module \"{}\"", moduleName);
    return true;
}

bool AngelScriptEngine::discardAllModules() {
    const auto names = getModuleNames();
    LOG(debug, "Discarding {} AngelScript module{}", names.size(), (names.size() == 1 ? "" : "s"));
    bool result = true;
    for (const auto& name : names) {
        if (!discardModule(name)) { result = false; }
    }
    const auto modulesLeftOver = getModuleCount();
    if (modulesLeftOver > 0) {
        LOG(err,
            "{} AngelScript module{} left after discarding all modules!",
            modulesLeftOver,
            (modulesLeftOver == 1 ? " was" : "s were"));
        result = false;
    } else {
        LOG(debug, "All AngelScript modules discarded");
    }
    return result;
}

std::size_t AngelScriptEngine::getModuleCount() const {
    return _engine->GetModuleCount();
}

std::vector<AngelScriptEngine::ModuleName> AngelScriptEngine::getModuleNames() const {
    LOG(trace, "Computing list of AngelScript modules");
    std::vector<ModuleName> list;
    for (std::size_t i = 0, count = getModuleCount(); i < count; ++i) {
        list.emplace_back(_engine->GetModuleByIndex(static_cast<asUINT>(i))->GetName());
    }
    return list;
}

bool AngelScriptEngine::moduleExists(Param<ModuleName>::In moduleName) const {
    return _engine->GetModule(moduleName.c_str());
}

bool AngelScriptEngine::setUpDocumentationGenerator(const std::string& documentationOutputFile) {
    if (documentationGeneratorIsSetUp()) {
        LOG(warn,
            "Setting up new documentation generator with output file {} - will only discard old one if it can be set up "
            "successfully",
            documentationOutputFile);
    }
    try {
        ::ScriptDocumentationOptions options;
        options.projectName = "Computer Wars";
        options.outputFile = documentationOutputFile;
        auto newGenerator = std::make_unique<::DocumentationGenerator>(_engine, options);
        if (newGenerator) {
            LOG(debug, "Allocated AngelScript interface documentation generator with output file {}", documentationOutputFile
            );
            _documentationGenerator = std::move(newGenerator);
            return true;
        }
        LOG(err,
            "Could not allocate AngelScript interface documentation generator with output file {}",
            documentationOutputFile);
    } catch (const std::exception& e) {
        LOG(err,
            "Could not allocate AngelScript interface documentation generator with output file {}: {}",
            documentationOutputFile,
            e);
    }
    return false;
}

bool AngelScriptEngine::documentationGeneratorIsSetUp() const {
    return _documentationGenerator.operator bool();
}

bool AngelScriptEngine::generateDocumentation() {
    if (!documentationGeneratorIsSetUp()) {
        LOG(critical,
            "Attempted to generate AngelScript interface documentation without having set up the documentation generator "
            "first!");
        return false;
    }
    LOG(debug, "Generating script interface documentation");
    const auto result = _documentationGenerator->Generate();
    if (result == ::asDOCGEN_Success) {
        LOG(debug, "Finished generating script interface documentation");
        return true;
    }
    LOG(err, "Could not generate script interface documentation, result code is {}", result);
    return false;
}
} // namespace cw
