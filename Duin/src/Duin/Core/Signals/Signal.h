#pragma once

#include "Duin/Core/Utils/UUID.h"
#include "Duin/Core/Debug/DNAssert.h"
#include "Duin/Core/Debug/DNLog.h"

#include <vector>
#include <functional>
#include <utility>
#include <algorithm>
#include <memory>

namespace duin
{

template <typename... types>
class Signal;
template <typename... types>
class SignalImpl;

/**
 * @brief RAII wrapper that automatically disconnects a signal listener on destruction.
 *
 * Lives exclusively behind a `shared_ptr<ScopedConnection>` returned by
 * `Signal::ConnectScoped`. Non-copyable and non-movable.
 *
 * Call `Release()` to opt out of auto-disconnect (the listener stays connected
 * even after the shared_ptr is destroyed).
 *
 * The Signal safely invalidates all live ScopedConnections in its destructor,
 * so destroying the Signal while a shared_ptr is still held will NOT crash when
 * the shared_ptr is eventually destroyed.
 *
 * @code
 * struct Foo {
 *     std::shared_ptr<duin::ScopedConnection> conn;
 *     void Init(duin::Signal<int>& sig) {
 *         conn = sig.ConnectScoped([this](int i) { ... });
 *     }
 * };
 * @endcode
 */
class ScopedConnection
{
  public:
    ~ScopedConnection()
    {
        if (disconnect_)
            disconnect_();
    }

    ScopedConnection(const ScopedConnection &) = delete;
    ScopedConnection &operator=(const ScopedConnection &) = delete;
    ScopedConnection(ScopedConnection &&) = delete;
    ScopedConnection &operator=(ScopedConnection &&) = delete;

    /**
     * @brief Releases ownership — the listener will NOT be disconnected on destruction.
     */
    void Release()
    {
        disconnect_ = nullptr;
    }

    bool IsValid()
    {
        return (disconnect_ != nullptr);
    }

  private:
    template <typename...>
    friend class SignalImpl;

    explicit ScopedConnection(UUID uuid, std::function<void()> fn) : uuid(uuid), disconnect_(std::move(fn))
    {
    }

    void Invalidate()
    {
        disconnect_ = nullptr;
    }

    UUID uuid;
    std::function<void()> disconnect_;
};

// ─── SignalImpl ───────────────────────────────────────────────────────────────

template <typename... types>
class SignalImpl
{
  public:
    using Listener = std::pair<UUID, std::function<void(types...)>>;

    ~SignalImpl()
    {
        for (auto &[id, weakConn] : scopedConnections_)
        {
            if (auto conn = weakConn.lock())
                conn->Invalidate();
        }
    }

    SignalImpl() = default;
    SignalImpl(const SignalImpl &) = delete;
    SignalImpl &operator=(const SignalImpl &) = delete;
    SignalImpl(SignalImpl &&) = delete;
    SignalImpl &operator=(SignalImpl &&) = delete;

    UUID Connect(std::function<void(types...)> callback)
    {
        UUID uuid;
        callbacks_.emplace_back(uuid, std::move(callback));
        return uuid;
    }

    std::shared_ptr<ScopedConnection> ConnectScoped(std::shared_ptr<SignalImpl> self,
                                                    std::function<void(types...)> callback)
    {
        UUID id = Connect(std::move(callback));
        std::weak_ptr<SignalImpl> weakSelf = self;
        auto conn = std::shared_ptr<ScopedConnection>(new ScopedConnection(id, [weakSelf, id]() {
            if (auto impl = weakSelf.lock())
                impl->Disconnect(id);
        }));
        scopedConnections_.emplace_back(id, std::weak_ptr<ScopedConnection>(conn));
        return conn;
    }

    void Disconnect(UUID uuid)
    {
        for (auto &[id, weakConn] : scopedConnections_)
        {
            if (id == uuid)
            {
                if (auto conn = weakConn.lock())
                    conn->Invalidate();
                break;
            }
        }
        std::erase_if(scopedConnections_, [&](const auto &p) { return p.first == uuid; });

        if (emitExecuting_)
            eraseQueue_.push_back(uuid);
        else
            std::erase_if(callbacks_, [&](const Listener &l) { return l.first == uuid; });
    }

    void Disconnect(std::weak_ptr<ScopedConnection> sc)
    {
        if (!sc.expired())
        {
            Disconnect(sc.lock()->uuid);
        }
        else
        {
            DN_CORE_WARN("Attempting to disconnect a freed ScopedConnection!");
        }
    }

    void DisconnectAll()
    {
        for (auto &[id, weakConn] : scopedConnections_)
        {
            if (auto conn = weakConn.lock())
                conn->Invalidate();
        }
        scopedConnections_.clear();

        if (emitExecuting_)
            queueEraseAll_ = true;
        else
            callbacks_.clear();
    }

    bool Emit(types... args)
    {
        DN_CORE_WARN_IF(emitExecuting_,
                         "Recursive infinite emition detected! Do not re-emit signal while it is still emitting");
        //DN_CORE_ASSERT(!emitExecuting_,
        //               "Recursive infinite emition detected! Do not re-emit signal while it is still emitting");

        if (!emitExecuting_)
        {
            recursionLimitHit = false;
        }

        ++recursiveEmissionCount;
        if (recursiveEmissionCount >= 30)
        {
            DN_CORE_FATAL("Recursive emittion exceeded 30! Preventing recursion loop.");
            --recursiveEmissionCount;
            recursionLimitHit = true;
            return false;
        }

        emitExecuting_ = true;

        for (size_t i = 0; i < callbacks_.size(); i++)
        {
            auto &[id, fn] = callbacks_[i];
            if (fn)
                fn(args...);
            else
                Disconnect(id);
        }

        emitExecuting_ = false;

        if (queueEraseAll_)
        {
            callbacks_.clear();
            queueEraseAll_ = false;
        }
        else
        {
            EraseQueue();
        }

        bool res = recursionLimitHit;

        --recursiveEmissionCount;
        return !res;
    }

    size_t GetListenerCount() const
    {
        return callbacks_.size();
    }

  private:
    std::vector<Listener> callbacks_;
    std::vector<std::pair<UUID, std::weak_ptr<ScopedConnection>>> scopedConnections_;
    std::vector<UUID> eraseQueue_;

    bool emitExecuting_ = false;
    bool queueEraseAll_ = false;
    bool recursionLimitHit = false;
    size_t recursiveEmissionCount = 0;

    void EraseQueue()
    {
        // O(N^2) 'cause why not
        for (auto &uuid : eraseQueue_)
        {
            std::erase_if(callbacks_, [uuid](const Listener &l) { return l.first == uuid; });
        }
        eraseQueue_.clear();
    }
};

// ─── Signal ──────────────────────────────────────────────────────────────────

/**
 * @brief Signal class for event-driven programming.
 *
 * A type-safe signal/slot system supporting multiple listeners.
 * Listeners are identified by UUIDs and can be connected, disconnected, and notified with arbitrary arguments.
 * NOT THREAD-SAFE!
 *
 * Move-only: moving a Signal preserves all existing ScopedConnections.
 *
 * @tparam types Argument types passed to listeners when the signal is emitted.
 *
 * @code
 * duin::Signal<int, std::string> sig;
 * duin::UUID id = sig.Connect([](int a, const std::string& b) { ... });
 * sig.Emit(42, "hello");
 * sig.Disconnect(id);
 * @endcode
 */
template <typename... types>
class Signal
{
  public:
    using Listener = std::pair<UUID, std::function<void(types...)>>;

    Signal() : impl_(std::make_shared<SignalImpl<types...>>())
    {
    }

    ~Signal() = default;

    Signal(const Signal &) = delete;
    Signal &operator=(const Signal &) = delete;

    Signal(Signal &&) = default;
    Signal &operator=(Signal &&) = default;

    /**
     * @brief Connects a listener to the signal (unsafe).
     *
     * Registers a callback to be invoked when the signal is emitted.
     * Returns a unique UUID for the listener, which can be used to disconnect it later.
     *
     * @param callback The function to call when the signal is emitted.
     * @return UUID assigned to the listener.
     */
    UUID Connect(std::function<void(types...)> callback)
    {
        return impl_->Connect(std::move(callback));
    }

    /**
     * @brief Connects a listener and returns a shared_ptr<ScopedConnection> that auto-disconnects.
     *
     * The listener is removed when the returned shared_ptr is destroyed (unless Release() was called).
     *
     * @param callback The function to call when the signal is emitted.
     * @return shared_ptr<ScopedConnection> that owns the connection lifetime.
     */
    std::shared_ptr<ScopedConnection> ConnectScoped(std::function<void(types...)> callback)
    {
        return impl_->ConnectScoped(impl_, std::move(callback));
    }

    /**
     * @brief Disconnects a listener by UUID.
     *
     * Removes the listener associated with the given UUID.
     *
     * @param uuid The UUID of the listener to disconnect.
     */
    void Disconnect(UUID uuid)
    {
        impl_->Disconnect(uuid);
    }

    void Disconnect(std::weak_ptr<ScopedConnection> sc)
    {
        impl_->Disconnect(sc);
    }

    void DisconnectAll()
    {
        impl_->DisconnectAll();
    }

    /**
     * @brief Emits the signal to all connected listeners.
     *
     * Invokes all registered callbacks with the provided arguments.
     *
     * @param args Arguments to pass to each listener.
     */
    bool Emit(types... args)
    {
        return impl_->Emit(args...);
    }

    /**
     * @brief Gets the UUID of this Signal instance.
     *
     * @return UUID of the signal.
     */
    UUID GetUUID()
    {
        return uuid_;
    }

    /**
     * @brief Returns count of connected listeners.
     *
     * @return Int count of listeners.
     */
    size_t GetListenerCount()
    {
        return impl_->GetListenerCount();
    }

  private:
    UUID uuid_;
    std::shared_ptr<SignalImpl<types...>> impl_ = nullptr;
};

} // namespace duin
