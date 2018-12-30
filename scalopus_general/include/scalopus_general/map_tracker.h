/*
  Copyright (c) 2018, Ivor Wanders
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

  Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef SCALOPUS_SCOPE_MAP_TRACKER_H
#define SCALOPUS_SCOPE_MAP_TRACKER_H

#include <map>
#include <shared_mutex>
#include <string>

namespace scalopus
{
/**
 * @brief Thread-safe class for arbritrary mapping.
 */
template <typename Key, typename Value>
class MapTracker
{
public:
  MapTracker(MapTracker const&) = delete;    //! Delete the copy constructor.
  void operator=(MapTracker const&) = delete;  //! Delete the assigment operator.

  /**
   * @brief Function to insert an key->value pair. Is thread safe.
   * @param key The key to store in the map.
   * @param value The value to associate to this key.
   */
  void insert(const Key& key, const Value& value)
  {
    std::unique_lock<decltype(mutex_)> lock(mutex_);
    mapping_[key] = value;
  }

  /**
   * @brief Function to retrieve the mapping between entry and exit ids and provided names.
   * @return Map that holds trace point id - trace name mapping.
   */
  std::map<Key, Value> getMap() const
  {
    std::shared_lock<decltype(mutex_)> lock(mutex_);
    return mapping_;
  }

private:
  std::map<Key, Value> mapping_;
  mutable std::shared_timed_mutex mutex_;  //! Mutex for the mapping container.

protected:
  //! Make constructor private such that we can ensure it is a singleton.
  MapTracker() = default;
};
}  // namespace scalopus

#endif  // SCALOPUS_SCOPE_MAP_TRACKER_H