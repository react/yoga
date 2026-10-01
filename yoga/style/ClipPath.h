/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <memory>
#include <string>
#include <string_view>

namespace facebook::yoga {

/**
 * Owned, opaque CSS clip-path metadata.
 *
 * Yoga does not interpret this value during layout. The default value is
 * represented without an allocation, while non-default values are immutable
 * and deep-copied with a Style.
 */
class ClipPath {
 public:
  ClipPath() noexcept = default;

  ClipPath(const ClipPath& other) {
    if (other.serialized_ != nullptr) {
      serialized_ = std::make_unique<const std::string>(*other.serialized_);
    }
  }

  ClipPath& operator=(const ClipPath& other) {
    if (this != &other) {
      ClipPath copy{other};
      serialized_.swap(copy.serialized_);
    }
    return *this;
  }

  ClipPath(ClipPath&&) noexcept = default;
  ClipPath& operator=(ClipPath&&) noexcept = default;

  explicit ClipPath(const char* serialized) {
    if (serialized != nullptr && std::string_view{serialized} != "none") {
      serialized_ = std::make_unique<const std::string>(serialized);
    }
  }

  explicit ClipPath(std::string_view serialized) {
    if (serialized != "none") {
      serialized_ = std::make_unique<const std::string>(serialized);
    }
  }

  bool isNone() const noexcept {
    return serialized_ == nullptr;
  }

  std::string_view serialized() const noexcept {
    return serialized_ == nullptr ? std::string_view{"none"}
                                  : std::string_view{*serialized_};
  }

  const char* c_str() const noexcept {
    return serialized_ == nullptr ? "none" : serialized_->c_str();
  }

  bool operator==(const ClipPath& other) const noexcept {
    return serialized_ == other.serialized_ ||
        (serialized_ != nullptr && other.serialized_ != nullptr &&
         *serialized_ == *other.serialized_);
  }

 private:
  std::unique_ptr<const std::string> serialized_{};
};

} // namespace facebook::yoga
