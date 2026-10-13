// Copyright 2016 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// The in-memory entry of tests/fuzz_target.cc, compiled into libbloaty so it
// is an ordinary library function: Bloaty runs over a byte buffer instead of
// a file on disk. The two classes are copied verbatim from that file; keep
// them identical, since a binary linking both sees the same inline classes.

#include <string_view>

#include "bloaty.h"
#include "bloaty.pb.h"

using std::string_view;

namespace bloaty {

class StringPieceInputFile : public InputFile {
 public:
  StringPieceInputFile(string_view data)
      : InputFile("fake_StringPieceInputFile_file") {
    data_ = data;
  }

  bool TryOpen(std::string_view /* filename */,
               std::unique_ptr<InputFile>& file) override {
    file.reset(new StringPieceInputFile(data_));
    return true;
  }
};

class StringPieceInputFileFactory : public InputFileFactory {
 public:
  StringPieceInputFileFactory(string_view data) : data_(data) {}
 private:
  string_view data_;
  std::unique_ptr<InputFile> OpenFile(
      const std::string& /* filename */) const override {
    return std::unique_ptr<InputFile>(new StringPieceInputFile(data_));
  }
};

void RunBloaty(string_view data, const std::string& data_source) {
  StringPieceInputFileFactory factory(data);
  bloaty::RollupOutput output;
  bloaty::Options options;
  std::string error;
  options.add_data_source(data_source);
  options.add_filename("dummy_filename");
  bloaty::BloatyMain(options, factory, &output, &error);
}

}  // namespace bloaty
