/**
 * \file ScriptModel.hpp
 * \brief Declares the model that stores and manages external scripts.
 */

#pragma once

#include "angelscript/AngelScriptEngine.hpp"
#include "log/Log.hpp"
#include "mvc/Model.hpp"
#include "ScriptEngine.hpp"

#include <list>
#include <regex>
#include <set>
#include <typeindex>

namespace cw {
namespace model {
class Script;

/**
 * \brief Mechanism via which registrants/models can register their script interface components to the script model during a
 * RegisterInterface command call.
 */
struct ScriptInterfaceDelegate {
    /**
     * \brief Initialises the delegate with a pointer to the script model that will receive registration calls.
     * \param scriptModel The script model to attach to this delegate object.
     */
    ScriptInterfaceDelegate(const std::shared_ptr<Script>& scriptModel) : _scriptModel(scriptModel) {}

private:
    /**
     * \brief The script model that this delegate calls into on behalf of the registrant.
     */
    const std::shared_ptr<Script> _scriptModel;
};
} // namespace model

namespace command {
/**
 * \brief Allows the invoker to register new additions to the script interface.
 */
struct RegisterInterface : public Command {
    /**
     * \brief Signature of the callback invoked by the script model that allows registrants to add themselves.
     */
    using Callback = std::function<void(const std::shared_ptr<model::ScriptInterfaceDelegate>&)>;

private:
    /**
     * \brief Stores a registrants dependencies and its delegate callback.
     */
    class Dependencies {
        /**
         * \brief Stores a script interface delegate callback, which is responsible for performing the actual script
         * interface registration.
         */
        class Registrant {
            /**
             * \brief The stored callback.
             */
            Callback _callback;

            /**
             * \brief Allows the registrant to return the root command object.
             */
            RegisterInterface* const _this;

        public:
            /**
             * \brief Initialises the inner Registrant object with the root command object.
             * \param me This.
             */
            Registrant(RegisterInterface* const me) : _this(me) {}

            /**
             * \brief Initialises the inner Registrant object with the root command object, and copies the callback from an
             * existing Registrant object.
             * \param me This.
             * \param cpy Registrant object to copy from.
             */
            Registrant(RegisterInterface* const me, const Registrant& cpy) : _this(me) {
                _callback = cpy._callback;
            }

            /**
             * \brief Initialises the inner Registrant object with the root command object, and moves the callback from an
             * existing Registrant object.
             * \param me This.
             * \param cpy Registrant object to move into this one.
             */
            Registrant(RegisterInterface* const me, Registrant&& cpy) noexcept : _this(me) {
                _callback = std::move(cpy._callback);
            }

            /**
             * \brief Copies the callback from an existing Registrant object.
             * \param cpy Registrant object to copy from.
             * \returns Reference to this.
             */
            Registrant& operator=(const Registrant& cpy) {
                _callback = cpy._callback;
                return *this;
            }

            /**
             * \brief Moves the callback from an existing Registrant object.
             * \param cpy Registrant object to move into this one.
             * \returns Reference to this.
             */
            Registrant& operator=(Registrant&& cpy) noexcept {
                _callback = std::move(cpy._callback);
                return *this;
            }

            /**
             * \brief Set the delegate callback that will be invoked once all of the dependencies have been registered.
             * \param cb The callback.
             * \returns Reference to the root command object.
             */
            inline RegisterInterface& AndIWant(const Callback& cb) {
                _callback = cb;
                return *_this;
            }

            /**
             * \brief Get the delegate callback.
             * \returns Reference to the callback.
             * \throws cw::AssertionError if a delegate callback was not set.
             */
            inline const Callback& whatDoTheyWant() const {
                ASSERT(
                    _callback,
                    "This RegisterInterface command wasn't given a delegate callback: please chain the "
                    "IAm()[.IDependOn()].andIWant() methods!"
                );
                return _callback;
            }
        } _registrant; ///< This script registrant's delegate callback.

        /**
         * \brief The models that this model depends on.
         */
        std::set<std::type_index> _dependencies;

    public:
        /**
         * \brief Initialises the inner Registrant object with the root command object.
         * \param me This.
         */
        Dependencies(RegisterInterface* const me) : _registrant(me) {}

        /**
         * \brief Initialises the inner Registrant object with the root command object, and copies the registrant information
         * from an existing Dependencies object.
         * \param me This.
         * \param cpy Dependencies object to copy from.
         */
        Dependencies(RegisterInterface* const me, const Dependencies& cpy) : _registrant(me, cpy._registrant) {
            _dependencies = cpy._dependencies;
        }

        /**
         * \brief Initialises the inner Registrant object with the root command object, and moves the registrant information
         * from an existing Dependencies object.
         * \param me This.
         * \param cpy Dependencies object to move into this one.
         */
        Dependencies(RegisterInterface* const me, Dependencies&& cpy) noexcept
            : _registrant(me, std::move(cpy._registrant)) {
            _dependencies = std::move(cpy._dependencies);
        }

        /**
         * \brief Copies the dependencies information from an existing Dependencies object.
         * \param cpy Dependencies object to copy from.
         * \returns Reference to this.
         */
        Dependencies& operator=(const Dependencies& cpy) {
            _dependencies = cpy._dependencies;
            _registrant = cpy._registrant;
            return *this;
        }

        /**
         * \brief Moves the dependencies from an existing Dependencies object.
         * \param cpy Dependencies object to move into this one.
         * \returns Reference to this.
         */
        Dependencies& operator=(Dependencies&& cpy) noexcept {
            _dependencies = std::move(cpy._dependencies);
            _registrant = std::move(cpy._registrant);
            return *this;
        }

        /**
         * \brief Set the dependencies that this model has.
         * \tparam Ds The concrete type of each model that this model relies on to perform a successful script interface
         * registration. Can be empty, but if this model has no dependencies, they could also use the direct IWant() method.
         * \returns A reference to an object that lets you set the delegate callback.
         */
        template <typename... Ds> [[nodiscard]] inline Registrant& IDependOn() {
            _dependencies = { typeid(Ds)... }; // https://stackoverflow.com/a/24758374.
            return _registrant;
        }

        /**
         * \brief Set the delegate callback that will be responsible for the script interface registration for this model.
         * \details If this model has dependencies, you should set this callback via the IDependOn() method.
         * \param cb The callback to set.
         * \returns Reference to the root command object.
         */
        inline RegisterInterface& AndIWant(const Callback& cb) {
            return _registrant.AndIWant(cb);
        }

        /**
         * \brief Returns a list of type indices that describe the models that this script registrant depends on, if any.
         * \returns The type indices of the dependencies.
         */
        inline const std::set<std::type_index>& whoDoTheyDependOn() const {
            return _dependencies;
        }

        /**
         * \brief Can be used to invoke the delegate callback.
         * \returns Reference to the delegate callback.
         * \throws cw::AssertionError if a delegate callback was not set.
         */
        inline const Callback& whatDoTheyWant() const {
            return _registrant.whatDoTheyWant();
        }
    } _dependencies; ///< This script registrant's dependencies.

    /**
     * \brief Identifies the model who is the registrant.
     */
    std::optional<std::type_index> _id;

public:
    /**
     * \brief Initialises the inner Registrant object with the root command object.
     */
    RegisterInterface() : _dependencies(this) {}

    /**
     * \brief Initialises the inner Registrant object with the root command object, and copies the dependency information
     * from an existing RegisterInterface object.
     * \param cpy RegisterInterface object to copy from.
     */
    RegisterInterface(const RegisterInterface& cpy) : _dependencies(this, cpy._dependencies) {
        _id = cpy._id;
    }

    /**
     * \brief Initialises the inner Registrant object with the root command object, and moves the dependency information
     * from an existing RegisterInterface object.
     * \param cpy RegisterInterface object to move into this one.
     */
    RegisterInterface(RegisterInterface&& cpy) noexcept : _dependencies(this, std::move(cpy._dependencies)) {
        _id = std::move(cpy._id);
    }

    /**
     * \brief Copies an existing RegisterInterface object.
     * \param cpy RegisterInterface object to copy from.
     * \returns Reference to this.
     */
    RegisterInterface& operator=(const RegisterInterface& cpy) {
        _id = cpy._id;
        _dependencies = cpy._dependencies;
        return *this;
    }

    /**
     * \brief Moves an existing RegisterInterface object into this one.
     * \param cpy RegisterInterface object to move into this one.
     * \returns Reference to this.
     */
    RegisterInterface& operator=(RegisterInterface&& cpy) noexcept {
        _id = std::move(cpy._id);
        _dependencies = std::move(cpy._dependencies);
        return *this;
    }

    /**
     * \brief Sets up this command with the concrete model type who is putting themselves forward as a registrant.
     * \tparam T The concrete type of the model invoking the command.
     * \returns A reference to an object that lets you set the dependencies of this model.
     */
    template <typename T> [[nodiscard]] inline Dependencies& IAm() {
        _id = typeid(T);
        return _dependencies;
    }

    /**
     * \brief Returns the type index of the model this script registrant object relates to.
     * \returns The type index of the model type.
     * \throws cw::AssertionError if the script registrant didn't set up the command object with their concrete type.
     */
    inline std::type_index whoAreThey() const {
        ASSERT(
            _id,
            "This RegisterInterface command wasn't given a concrete model type: please chain the "
            "IAm()[.IDependOn()].andIWant() methods!"
        );
        return *_id;
    }

    /**
     * \brief Returns a list of type indices that describe the models that this script registrant depends on, if any.
     * \returns The type indices of the dependencies.
     */
    inline const std::set<std::type_index>& whoDoTheyDependOn() const {
        return _dependencies.whoDoTheyDependOn();
    }

    /**
     * \brief Can be used to invoke the delegate callback.
     * \returns Reference to the delegate callback.
     * \throws cw::AssertionError if a delegate callback was not set.
     */
    inline const Callback& whatDoTheyWant() const {
        return _dependencies.whatDoTheyWant();
    }

    /**
     * \brief Used to validate that the command is valid.
     * \param existingIdentities A set containing type IDs of models that have yet to be registered, and models that have
     * aleady been registered.
     * \returns Reference to this if the command is valid.
     * \throws cw::AssertionError if the command was not setup correctly.
     */
    inline const RegisterInterface& validate(const std::set<std::type_index>& existingIdentities) const {
        const auto theyAre = whoAreThey();
        ASSERT(
            !existingIdentities.contains(theyAre),
            "The model {} has already been registered, or has already been queued for registration!",
            theyAre
        );
        ASSERT(
            !whoDoTheyDependOn().contains(theyAre), "The model {} cannot depend on the script interface of itself!", theyAre
        );
        whatDoTheyWant();
        return *this;
    }
};
} // namespace command
} // namespace cw

namespace fmt {
/**
 * \brief FMT formatter for cw::command::RegisterInterface.
 */
template <> struct formatter<cw::command::RegisterInterface> {
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
    template <typename FormatContext>
    auto format(const cw::command::RegisterInterface& o, FormatContext& ctx) const -> decltype(ctx.out()) {
        std::optional<std::type_index> identity;
        try {
            identity = o.whoAreThey();
        } catch (...) {}
        return fmt::format_to(ctx.out(), "{{ Model: {}, Dependencies: {} }}", identity, o.whoDoTheyDependOn());
    }
};
} // namespace fmt

namespace cw {
namespace request {
/**
 * \brief Instruct the script model to generate script interface documentation.
 */
struct GenerateDocumentation : public Request {
    /**
     * \brief True will be returned from the request if generation was successful, false otherwise.
     */
    using ReturnType = bool;
};
} // namespace request

namespace model {
/**
 * \brief Manages external scripts.
 */
class Script : public Model, public std::enable_shared_from_this<Script> {
public:
    /**
     * \brief Sets up the script engine that drives the script model.
     * \param documentationOutputFile If the documentation of the script interface should be generated, this should be the
     * name of the HTML file outputted. No documentation will be generated if this parameter is empty.
     */
    Script(const std::string& documentationOutputFile = "") : _engine(std::make_unique<AngelScriptEngine>()) {
        ASSERT(_engine, "Script engine failed to load.");
        if (!documentationOutputFile.empty()) { _engine->setUpDocumentationGenerator(documentationOutputFile); }
    }

    /**
     * \copydoc Model::registerModel
     * \warning This model must be registered before any model that adds to the script interface.
     */
    void registerModel(const std::shared_ptr<ReadWriteController>& controller) final {
        REGISTER(controller, Command, command::RegisterInterface, Script::_registerInterface, this);
        REGISTER(controller, Request, request::GenerateDocumentation, Script::_generateDocumentation, this);
    }

    /**
     * \brief Deletes every module currently loaded and loads new ones based on the contents of the given JSON.
     * \copydetails JSONSerialised::fromJSON
     */
    void fromJSON(const json& j) final {
        _lastKnownJSON = j;
        ASSERT(
            _cachedRegistrants.empty(),
            "{} model{} could not register {} script interface{}! {}. Aborting script model deserialisation...",
            _cachedRegistrants.size(),
            (_cachedRegistrants.size() == 1 ? "" : "s"),
            (_cachedRegistrants.size() == 1 ? "its" : "their"),
            (_cachedRegistrants.size() == 1 ? "" : "s"),
            _cachedRegistrants
        );
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

    friend struct ScriptInterfaceDelegate;

private:
    /// \cond

    DECLARE_COMMAND(_registerInterface) {
        RECEIVE_COMMAND(command::RegisterInterface);
        try {
            std::set<std::type_index> existingRegistrants = _getCachedRegistrantIdentities();
            existingRegistrants.insert(_successfulRegistrants.begin(), _successfulRegistrants.end());
            _cachedRegistrants.push_front(command.validate(existingRegistrants));
        } catch (...) {
            LOG(err, "RegisterInterface command was invalid, rethrowing error...");
            throw;
        }
        LOG(trace,
            "Pushed new registrant to the cache: {} depends on {}",
            _cachedRegistrants.front().whoAreThey(),
            _cachedRegistrants.front().whoDoTheyDependOn());
        LOG(trace, "Registrant cache is now being traversed");
        for (auto itr = _cachedRegistrants.begin(); itr != _cachedRegistrants.end();) {
            const std::type_index id = itr->whoAreThey();
            const auto& dependencies = itr->whoDoTheyDependOn();
            std::set<std::type_index> unregisteredDependencies;
            std::set_difference(
                dependencies.begin(),
                dependencies.end(),
                _successfulRegistrants.begin(),
                _successfulRegistrants.end(),
                std::inserter(unregisteredDependencies, unregisteredDependencies.end())
            );
            if (!unregisteredDependencies.empty()) {
                LOG(trace,
                    "Registrant {} is waiting on {} before it can be registered: {}. This registrant's complete dependency "
                    "list: {}. The registrants registered so far: {}. Advancing to the next cache entry",
                    id,
                    (unregisteredDependencies.size() == 1 ? "this dependency" : "these dependencies"),
                    unregisteredDependencies,
                    dependencies,
                    _successfulRegistrants);
                ++itr;
                continue;
            }
            LOG(trace,
                "Registrant {}'s dependenc{} ({}) {} been registered! The registrants registered so far: {}. Invoking this "
                "registrant's callback now",
                id,
                (dependencies.size() == 1 ? "y" : "ies"),
                dependencies,
                (dependencies.size() == 1 ? "has" : "have all"),
                _successfulRegistrants);
            try {
                itr->whatDoTheyWant()(std::make_shared<ScriptInterfaceDelegate>(shared_from_this()));
            } catch (...) {
                LOG(err,
                    "Failed to register the script interface components of model {}, erasing from the script model and "
                    "rethrowing error...",
                    id);
                _cachedRegistrants.erase(itr);
                throw;
            }
            _successfulRegistrants.insert(id);
            LOG(trace,
                "Registrant {} has been successfully registered with the script interface. List of registered registrants: "
                "{}",
                id,
                _successfulRegistrants);
            _cachedRegistrants.erase(itr);
            itr = _cachedRegistrants.begin();
            LOG(trace,
                "Removed registrant {} from the cache, remaining registrants: {}, starting traversal from the top",
                id,
                _getCachedRegistrantIdentities());
        }
        LOG(trace,
            "RegisterInterface command processing finished and registrant cache traversal has ended, {} registrant{} still "
            "cached",
            _cachedRegistrants.size(),
            (_cachedRegistrants.size() == 1 ? "" : "s"));
    }

    DECLARE_REQUEST(_generateDocumentation) {
        RECEIVE_REQUEST(request::GenerateDocumentation);
        return _engine->documentationGeneratorIsSetUp() && _engine->generateDocumentation();
    }

    /// \endcond

    /**
     * \brief Creates a set containing model types that have yet to be registered.
     * \returns The set of type IDs.
     */
    std::set<std::type_index> _getCachedRegistrantIdentities() const {
        std::set<std::type_index> cachedIDs;
        for (const auto& entry : _cachedRegistrants) { cachedIDs.insert(entry.whoAreThey()); }
        return cachedIDs;
    }

    /**
     * \brief Caches the JSON that was last given to fromJSON().
     */
    cw::json _lastKnownJSON;

    /**
     * \brief Points to the underlying script engine.
     */
    const std::unique_ptr<AngelScriptEngine> _engine;

    /**
     * \brief The registrants that require their dependencies to be registered first before they can be registered.
     */
    std::list<command::RegisterInterface> _cachedRegistrants;

    /**
     * \brief Keeps track of which models have been registered successfully.
     */
    std::set<std::type_index> _successfulRegistrants;
};
} // namespace model
} // namespace cw
