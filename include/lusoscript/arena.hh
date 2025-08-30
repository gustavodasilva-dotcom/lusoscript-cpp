#ifndef LUSOSCRIPT_ARENA_H
#define LUSOSCRIPT_ARENA_H

#include <cstddef>
#include <memory>
#include <new>
#include <type_traits>

namespace arena {
struct DestructNode {
  void (*dtor)(void *);
  DestructNode *prev;
  void *obj;
};

template <typename T>
struct NoopDeleter {
  void operator()(T *) const noexcept {}
};

class Arena {
 public:
  explicit Arena(std::size_t size);

  ~Arena();

  template <typename T, typename... Args>
  T *create(Args &&...args) {
    // Allocate space for object.
    auto [ptr, new_offset] = allocate(offset_, sizeof(T), alignof(T));

    if constexpr (std::is_trivially_destructible_v<T>) {
      // If T has trivial destructor, construct object as usual.
      T *obj = new (ptr) T(std::forward<Args>(args)...);
      offset_ = new_offset;
      return obj;
    } else {
      // If not, create a DestructNode to store the destructor that will be
      // called later on.

      // Allocate space for DestructNode.
      auto [dnode_ptr, final_offset] =
          allocate(new_offset, sizeof(DestructNode), alignof(DestructNode));

      // Only construct T after successful allocation.
      T *obj = new (ptr) T(std::forward<Args>(args)...);

      auto dtor_call = [](void *p) { static_cast<T *>(p)->~T(); };
      DestructNode *new_node =
          new (dtor_call) DestructNode{dtor_call, tail_, obj};

      tail_ = new_node;

      // Commit offset after successful allocation and construction.
      offset_ = final_offset;

      return obj;
    }
  }

  // Follows the same logic of the previous function, but returns a
  // `unique_ptr`.
  template <typename T, typename... Args>
  std::unique_ptr<T, NoopDeleter<T>> make_unique(Args &&...args) {
    auto [ptr, new_offset] = allocate(offset_, sizeof(T), alignof(T));

    if constexpr (std::is_trivially_destructible_v<T>) {
      T *obj = new (ptr) T(std::forward<Args>(args)...);
      offset_ = new_offset;
      return std::unique_ptr<T, NoopDeleter<T>>(ptr);
    } else {
      auto [dnode_ptr, final_offset] =
          allocate(new_offset, sizeof(DestructNode), alignof(DestructNode));

      T *obj = new (ptr) T(std::forward<Args>(args)...);

      // Store the destructor function pointer.
      void (*dtor)(void *) = [](void *p) { static_cast<T *>(p)->~T(); };

      // Create new destruction node.
      DestructNode *new_node = new (dnode_ptr) DestructNode{dtor, tail_, obj};

      tail_ = new_node;
      offset_ = final_offset;

      return std::unique_ptr<T, NoopDeleter<T>>(obj);
    }
  }

  void reset() {
    callDestructors();
    offset_ = 0;
  }

  // Non-copyable, non-moveable type
  Arena(const Arena &) = delete;
  Arena(Arena &&) = delete;

 private:
  char *buffer_;
  std::size_t capacity_;
  std::size_t offset_;
  DestructNode *tail_ = nullptr;

  void callDestructors();
  std::pair<void *, std::size_t> allocate(std::size_t curr_offset,
                                          std::size_t size,
                                          std::size_t alignment);
};
};  // namespace arena

#endif
