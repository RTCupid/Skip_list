#ifndef SKIP_LIST_HPP
#define SKIP_LIST_HPP

#include <vector>
#include <random>
#include <utility>
#include <iostream>
#include <cassert>

/**
 * @brief Probabilistic skip list with unique keys.
 *
 * Implements an ordered associative container with expected O(log n)
 * complexity for insert, erase and search operations.
 * Keys are stored in ascending order. Duplicate keys are ignored.
 *
 * @tparam Key type of key, must be LessThanComparable (operator<)
 */
template <typename Key>
class SkipList {
private:
    /**
     * @brief Node of the skip list.
     */
    struct Node {
        const Key key;               ///< Stored key (immutable)
        std::vector<Node*> next;    ///< Pointers to next nodes at each level

        explicit Node(const Key& k, int level) : key(k), next(level, nullptr) {}
    };

public:
    /**
     * @brief Forward iterator providing read‑only access to keys.
     *
     * Iteration follows the level 0 list, thus keys are visited in ascending order.
     */
    class Iterator {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type        = Key;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const Key*;
        using reference         = const Key&;

        Iterator() = default;
        explicit Iterator(Node* node) : node_(node) {}

        reference operator*() const { return node_->key; }
        pointer   operator->() const { return &node_->key; }

        Iterator& operator++() {
            assert(node_);
            node_ = node_->next[0];
            return *this;
        }
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        bool operator==(const Iterator& other) const { return node_ == other.node_; }
        bool operator!=(const Iterator& other) const { return node_ != other.node_; }

    private:
        Node* node_ = nullptr;
        friend class SkipList;
    };

    // ---------- Constructors / Destructor / Assignment ----------

    /**
     * @brief Constructs an empty skip list.
     *
     * @param probability      Probability p of promoting a node to the next level (0 < p < 1)
     * @param maxAllowedLevel  Maximum level a node can reach (prevents infinite growth)
     */
    explicit SkipList(double probability = 0.5, int maxAllowedLevel = 32);

    /**
     * @brief Destructor – frees all allocated nodes.
     */
    ~SkipList();

    // Copying is prohibited (intrusive pointer management)
    SkipList(const SkipList&) = delete;
    SkipList& operator=(const SkipList&) = delete;

    /**
     * @brief Move constructor – transfers ownership of resources.
     *
     * @param other The source skip list (left in a valid empty state)
     */
    SkipList(SkipList&& other) noexcept;

    /**
     * @brief Move assignment – transfers ownership of resources.
     *
     * @param other The source skip list (left in a valid empty state)
     * @return Reference to this list
     */
    SkipList& operator=(SkipList&& other) noexcept;

    // ---------- Main operations ----------

    /**
     * @brief Inserts a key into the skip list.
     *
     * If the key already exists, the list remains unchanged.
     *
     * @param key The key to insert.
     */
    void insert(const Key& key);

    /**
     * @brief Removes a key from the skip list.
     *
     * @param key The key to erase.
     * @return true  if the key was found and removed,
     * @return false if the key was not present.
     */
    bool erase(const Key& key);

    /**
     * @brief Checks whether a key is present in the skip list.
     *
     * @param key The key to search for.
     * @return true  if the key exists,
     * @return false otherwise.
     */
    bool contains(const Key& key) const;

    // ---------- Print by levels ----------

    /**
     * @brief Prints the entire skip list level by level.
     *
     * Each level from the highest down to 0 is printed as a space‑separated list of keys.
     * @param os Output stream (default: std::cout)
     */
    void printByLevels(std::ostream& os = std::cout) const;

    // ---------- Iterators ----------

    /**
     * @brief Returns an iterator to the first element (level 0).
     */
    Iterator begin() const { return Iterator(head_->next[0]); }

    /**
     * @brief Returns an iterator past the last element.
     */
    Iterator end() const { return Iterator(nullptr); }

private:
    Node*   head_;                ///< Dummy head node (key is default constructed)
    int     maxLevel_;           ///< Current number of levels (height of the head)
    int     maxAllowedLevel_;    ///< Level cap, set at construction
    double  probability_;        ///< Probability p for level promotion

    mutable std::mt19937 rng_;   ///< Random number generator
    mutable std::uniform_real_distribution<double> dist_; ///< Uniform [0,1) distribution

    /**
     * @brief Generates a random level for a new node.
     *
     * Uses probability_ and the current max level to decide how high the node should go.
     * The result is always between 1 and maxAllowedLevel_ (inclusive).
     *
     * @return Randomly determined level.
     */
    int randomLevel() const;

    /**
     * @brief Restores the object to a valid empty state.
     *
     * Intended only for use on moved‑from objects. Creates a new head node if needed.
     */
    void resetToEmpty();
};

// ---------- Method implementation ----------

template <typename Key>
SkipList<Key>::SkipList(double probability, int maxAllowedLevel)
    : probability_(probability)
    , maxAllowedLevel_(maxAllowedLevel)
    , maxLevel_(1)
    , dist_(0.0, 1.0)
{
    std::random_device rd;
    rng_.seed(rd());
    head_ = new Node(Key(), maxLevel_);
}

template <typename Key>
SkipList<Key>::~SkipList() {
    if (!head_) return;
    Node* cur = head_->next[0];
    while (cur) {
        Node* next = cur->next[0];
        delete cur;
        cur = next;
    }
    delete head_;
}

template <typename Key>
SkipList<Key>::SkipList(SkipList&& other) noexcept
    : head_(std::exchange(other.head_, nullptr))
    , maxLevel_(std::exchange(other.maxLevel_, 1))
    , maxAllowedLevel_(other.maxAllowedLevel_)
    , probability_(other.probability_)
    , rng_(std::move(other.rng_))
    , dist_(other.dist_)
{
    if (!head_) {
        head_ = new Node(Key(), maxLevel_);
    }
    other.resetToEmpty();
}

template <typename Key>
auto SkipList<Key>::operator=(SkipList&& other) noexcept -> SkipList& {
    if (this != &other) {
        if (head_) {
            Node* cur = head_->next[0];
            while (cur) {
                Node* next = cur->next[0];
                delete cur;
                cur = next;
            }
            delete head_;
        }

        head_   = std::exchange(other.head_, nullptr);
        maxLevel_ = std::exchange(other.maxLevel_, 1);
        maxAllowedLevel_ = other.maxAllowedLevel_;
        probability_ = other.probability_;
        rng_    = std::move(other.rng_);
        dist_   = other.dist_;

        if (!head_) {
            head_ = new Node(Key(), maxLevel_);
        }
        other.resetToEmpty();
    }
    return *this;
}

template <typename Key>
int SkipList<Key>::randomLevel() const {
    int level = 1;
    while (dist_(rng_) < probability_ && level < maxAllowedLevel_ && level < maxLevel_ + 1) {
        ++level;
    }
    return level;
}

template <typename Key>
void SkipList<Key>::insert(const Key& key) {
    std::vector<Node*> update(maxLevel_, nullptr);
    Node* cur = head_;

    for (int i = maxLevel_ - 1; i >= 0; --i) {
        while (cur->next[i] && cur->next[i]->key < key) {
            cur = cur->next[i];
        }
        update[i] = cur;
    }
    cur = cur->next[0];

    if (cur && cur->key == key) {
        return;
    }

    int newLevel = randomLevel();

    if (newLevel > maxLevel_) {
        head_->next.resize(newLevel, nullptr);
        update.resize(newLevel, nullptr);
        for (int i = maxLevel_; i < newLevel; ++i) {
            update[i] = head_;
        }
        maxLevel_ = newLevel;
    }

    auto* newNode = new Node(key, newLevel);

    for (int i = 0; i < newLevel; ++i) {
        newNode->next[i] = update[i]->next[i];
        update[i]->next[i] = newNode;
    }
}

template <typename Key>
bool SkipList<Key>::erase(const Key& key) {
    std::vector<Node*> update(maxLevel_, nullptr);
    Node* cur = head_;

    for (int i = maxLevel_ - 1; i >= 0; --i) {
        while (cur->next[i] && cur->next[i]->key < key) {
            cur = cur->next[i];
        }
        update[i] = cur;
    }
    cur = cur->next[0];

    if (!cur || cur->key != key) {
        return false;
    }

    for (int i = 0; i < maxLevel_; ++i) {
        if (update[i]->next[i] == cur) {
            update[i]->next[i] = cur->next[i];
        }
    }
    delete cur;

    while (maxLevel_ > 1 && head_->next[maxLevel_ - 1] == nullptr) {
        --maxLevel_;
        head_->next.pop_back();
    }

    return true;
}

template <typename Key>
bool SkipList<Key>::contains(const Key& key) const {
    Node* cur = head_;
    for (int i = maxLevel_ - 1; i >= 0; --i) {
        while (cur->next[i] && cur->next[i]->key < key) {
            cur = cur->next[i];
        }
    }
    cur = cur->next[0];
    return cur && cur->key == key;
}

template <typename Key>
void SkipList<Key>::printByLevels(std::ostream& os) const {
    if (!head_) {
        os << "SkipList (empty - moved from)\n";
        return;
    }
    os << "SkipList (levels = " << maxLevel_ << ", p = " << probability_ << "):\n";
    for (int i = maxLevel_ - 1; i >= 0; --i) {
        os << "Level " << i << ": ";
        Node* node = head_->next[i];
        while (node) {
            os << node->key << ' ';
            node = node->next[i];
        }
        os << '\n';
    }
    os.flush();
}

template <typename Key>
void SkipList<Key>::resetToEmpty() {
    if (!head_) {
        head_ = new Node(Key(), maxLevel_);
    }
}

#endif // SKIP_LIST_HPP
