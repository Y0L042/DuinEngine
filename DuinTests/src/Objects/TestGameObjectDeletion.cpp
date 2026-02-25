#include <doctest.h>
#include "Defines.h"
#include <Duin/Objects/GameObject.h>
#include <memory>
#include <vector>
#include <string>

// Tracks construction/destruction order via a shared log.
class TrackedObject : public duin::GameObject
{
  public:
    TrackedObject(std::vector<std::string> &log, std::string name)
        : log(log), name(std::move(name))
    {
        log.push_back(name + ":constructed");
    }

    ~TrackedObject() override
    {
        log.push_back(name + ":destroyed");
    }

  private:
    std::vector<std::string> &log;
    std::string name;
};

namespace TestGameObject
{

TEST_SUITE("GameObject - Ownership and Lifetime")
{
    TEST_CASE("Child is destroyed when parent is destroyed")
    {
        std::weak_ptr<duin::GameObject> weakChild;

        {
            auto parent = std::make_shared<duin::GameObject>();
            auto child = parent->CreateChildObject<duin::GameObject>();
            weakChild = child;

            // Both parent and local child share_ptr are alive
            MSG_CHECK(weakChild.expired(), !weakChild.expired());

            // Drop the local handle — parent is now the sole owner
            child.reset();
            MSG_CHECK(weakChild.expired(), !weakChild.expired());
            parent.reset();
        }
        // parent destroyed here — child must be gone too
        MSG_CHECK(weakChild.expired(), weakChild.expired());
    }

    TEST_CASE("All children destroyed when parent is destroyed")
    {
        std::weak_ptr<duin::GameObject> weakA, weakB, weakC;

        {
            auto parent = std::make_shared<duin::GameObject>();
            weakA = parent->CreateChildObject<duin::GameObject>();
            weakB = parent->CreateChildObject<duin::GameObject>();
            weakC = parent->CreateChildObject<duin::GameObject>();
        }

        MSG_CHECK(weakA.expired(), weakA.expired());
        MSG_CHECK(weakB.expired(), weakB.expired());
        MSG_CHECK(weakC.expired(), weakC.expired());
    }

    TEST_CASE("Removing a child before parent destruction keeps the child alive")
    {
        std::weak_ptr<duin::GameObject> weakChild;
        std::shared_ptr<duin::GameObject> savedChild;

        {
            auto parent = std::make_shared<duin::GameObject>();
            auto child  = parent->CreateChildObject<duin::GameObject>();
            weakChild   = child;

            // Caller holds a reference AND removes it from the parent
            savedChild = child;
            parent->RemoveChildObject(child);
        }
        // parent is gone — but savedChild still holds the object alive
        MSG_CHECK(weakChild.expired(), !weakChild.expired());

        savedChild.reset();
        MSG_CHECK(weakChild.expired(), weakChild.expired());
    }

    TEST_CASE("Removed child has no parent")
    {
        auto parent = std::make_shared<duin::GameObject>();
        auto child  = parent->CreateChildObject<duin::GameObject>();

        CHECK_MESSAGE(!child->GetParent().expired(), "Sanity check");

        parent->RemoveChildObject(child);
        CHECK_FALSE(child->GetParent().lock() == parent);
        MSG_CHECK(child->GetParent(), child->GetParent().expired());
    }

    TEST_CASE("Reparented child survives destruction of original parent")
    {
        std::weak_ptr<duin::GameObject> weakChild;

        auto newParent = std::make_shared<duin::GameObject>();

        {
            auto oldParent = std::make_shared<duin::GameObject>();
            auto child     = oldParent->CreateChildObject<duin::GameObject>();
            weakChild      = child;

            oldParent->TransferChildObject(child, newParent);
        }
        // oldParent destroyed — child was transferred so it must still be alive
        MSG_CHECK(weakChild.expired(), !weakChild.expired());
    }

    TEST_CASE("Reparented child's parent is updated to new parent")
    {
        auto oldParent = std::make_shared<duin::GameObject>();
        auto newParent = std::make_shared<duin::GameObject>();
        auto child     = oldParent->CreateChildObject<duin::GameObject>();

        oldParent->TransferChildObject(child, newParent);

        MSG_CHECK(child->GetParent(), child->GetParent().lock() == newParent);
    }

    TEST_CASE("Reparented child's children also survive destruction of original parent")
    {
        std::weak_ptr<duin::GameObject> weakGrandchild;

        auto newParent = std::make_shared<duin::GameObject>();

        {
            auto oldParent  = std::make_shared<duin::GameObject>();
            auto child      = oldParent->CreateChildObject<duin::GameObject>();
            auto grandchild = child->CreateChildObject<duin::GameObject>();
            weakGrandchild  = grandchild;
            grandchild.reset();

            // Transfer child (carrying grandchild) to newParent
            oldParent->TransferChildObject(child, newParent);
        }

        // oldParent gone — grandchild lives on under newParent->child
        MSG_CHECK(weakGrandchild.expired(), !weakGrandchild.expired());
    }

    TEST_CASE("Destroying intermediate node destroys its subtree")
    {
        // root -> mid -> leaf
        // Dropping mid (after removing it from root) destroys both mid and leaf.
        std::weak_ptr<duin::GameObject> weakMid, weakLeaf;

        auto root = std::make_shared<duin::GameObject>();
        auto mid  = root->CreateChildObject<duin::GameObject>();
        auto leaf = mid->CreateChildObject<duin::GameObject>();

        weakMid  = mid;
        weakLeaf = leaf;
        leaf.reset();

        // Remove mid from root so root no longer owns it
        root->RemoveChildObject(mid);

        MSG_CHECK(weakMid.expired(),  !weakMid.expired());   // mid still alive via local 'mid'
        MSG_CHECK(weakLeaf.expired(), !weakLeaf.expired());  // leaf alive via mid's children

        mid.reset(); // drop the last reference to mid
        MSG_CHECK(weakMid.expired(),  weakMid.expired());
        MSG_CHECK(weakLeaf.expired(), weakLeaf.expired());
    }

    TEST_CASE("External shared_ptr to child extends lifetime beyond parent destruction")
    {
        std::shared_ptr<duin::GameObject> savedChild;
        std::weak_ptr<duin::GameObject> weakParent;

        {
            auto parent = std::make_shared<duin::GameObject>();
            weakParent  = parent;
            savedChild  = parent->CreateChildObject<duin::GameObject>();
        }

        // Parent is gone
        MSG_CHECK(weakParent.expired(), weakParent.expired());
        // But our external handle keeps the child alive
        MSG_CHECK(savedChild, savedChild != nullptr);
    }
}

} // namespace TestGameObject
