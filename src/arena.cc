#include "lusoscript/arena.hh"

arena::Arena::Arena(std::size_t size)
    : buffer_(static_cast<char *>(::operator new(size))),
      capacity_(size),
      offset_(0) {}

arena::Arena::~Arena() {
  callDestructors();
  ::operator delete(buffer_);
}

void arena::Arena::callDestructors() {
  while (tail_) {
    tail_->dtor(tail_->obj);
    tail_ = tail_->prev;
  }
}

// Checks if there's memory of `size` bytes with specified `alignment`
// available.
std::pair<void *, std::size_t> arena::Arena::allocate(std::size_t curr_offset,
                                                      std::size_t size,
                                                      std::size_t alignment) {
  char *current_ptr = buffer_ + curr_offset;
  std::size_t space = capacity_ - curr_offset;
  void *aligned_ptr = current_ptr;

  // Align the pointer to the specified alignment.
  if (std::align(alignment, size, aligned_ptr, space) == nullptr) {
    throw std::bad_alloc();
  }

  auto new_offset = static_cast<char *>(aligned_ptr) - buffer_ + size;

  return {aligned_ptr, new_offset};
}
