//===- unittests/BuildSystem/BuildValueTest.cpp ---------------------------===//
//
// This source file is part of the Swift.org open source project
//
// Copyright (c) 2016 Apple Inc. and the Swift project authors
// Licensed under Apache License v2.0 with Runtime Library Exception
//
// See http://swift.org/LICENSE.txt for license information
// See http://swift.org/CONTRIBUTORS.txt for the list of Swift project authors
//
//===----------------------------------------------------------------------===//

#include "llbuild/Basic/Hashing.h"
#include "llbuild/BuildSystem/BuildValue.h"

#include "gtest/gtest.h"

using namespace llbuild;
using namespace llbuild::buildsystem;
using namespace llvm;

namespace {

/// We should support decoding an empty value without crashing.
TEST(BuildValueTest, emptyDecode) {
  auto result = BuildValue::fromData(core::ValueType());
  EXPECT_EQ(result.toData(), BuildValue::makeInvalid().toData());
}

TEST(BuildValueTest, virtualValueSerialization) {
  // Check that two identical values are equivalent.
  {
    BuildValue a = BuildValue::makeVirtualInput();
    EXPECT_EQ(a.toData(), BuildValue::makeVirtualInput().toData());
  }

  // Check that a moved complex value is equivalent.
  {
    BuildValue tmp = BuildValue::makeVirtualInput();
    BuildValue a = std::move(tmp);
    EXPECT_EQ(a.toData(), BuildValue::makeVirtualInput().toData());
  }

  // Check that an rvalue initialized complex value is equivalent.
  {
    BuildValue tmp = BuildValue::makeVirtualInput();
    BuildValue a{ std::move(tmp) };
    EXPECT_EQ(a.toData(), BuildValue::makeVirtualInput().toData());
  }

  // Check that a round-tripped value is equivalent.
  EXPECT_EQ(BuildValue::makeVirtualInput().toData(),
            BuildValue::fromData(
                BuildValue::makeVirtualInput().toData()).toData());
}

TEST(BuildValueTest, commandValueSingleOutputSerialization) {
  basic::FileInfo infos[1] = {};
  infos[0].size = 1;
  
  // Check that two identical values are equivalent.
  {
    BuildValue a = BuildValue::makeSuccessfulCommand(infos);
    EXPECT_EQ(a.toData(), BuildValue::makeSuccessfulCommand(infos).toData());
  }

  // Check that a moved complex value is equivalent.
  {
    BuildValue tmp = BuildValue::makeSuccessfulCommand(infos);
    BuildValue a = std::move(tmp);
    EXPECT_EQ(a.toData(), BuildValue::makeSuccessfulCommand(infos).toData());
  }

  // Check that an rvalue initialized complex value is equivalent.
  {
    BuildValue tmp = BuildValue::makeSuccessfulCommand(infos);
    BuildValue a{ std::move(tmp) };
    EXPECT_EQ(a.toData(), BuildValue::makeSuccessfulCommand(infos).toData());
  }

  // Check that a round-tripped value is equivalent.
  EXPECT_EQ(BuildValue::makeSuccessfulCommand(infos).toData(),
            BuildValue::fromData(
                BuildValue::makeSuccessfulCommand(infos).toData()).toData());
}

TEST(BuildValueTest, commandValueMultipleOutputsSerialization) {
  basic::FileInfo infos[2] = {};
  infos[0].size = 1;
  infos[1].size = 2;
  
  // Check that two identical values are equivalent.
  {
    BuildValue a = BuildValue::makeSuccessfulCommand(infos);
    EXPECT_EQ(a.toData(),
              BuildValue::makeSuccessfulCommand(infos).toData());
  }

  // Check that a moved complex value is equivalent.
  {
    BuildValue tmp = BuildValue::makeSuccessfulCommand(infos);
    BuildValue a = std::move(tmp);
    EXPECT_EQ(a.toData(),
              BuildValue::makeSuccessfulCommand(infos).toData());
  }

  // Check that an rvalue initialized complex value is equivalent.
  {
    BuildValue tmp = BuildValue::makeSuccessfulCommand(infos);
    BuildValue a{ std::move(tmp) };
    EXPECT_EQ(a.toData(), BuildValue::makeSuccessfulCommand(infos).toData());
  }

  // Check that a round-tripped value is equivalent.
  EXPECT_EQ(BuildValue::makeSuccessfulCommand(infos).toData(),
            BuildValue::fromData(
                BuildValue::makeSuccessfulCommand(infos).toData()).toData());
}

TEST(BuildValueTest, commandValueWithSignatureSingleOutputSerialization) {
  auto signature = llbuild::basic::CommandSignature(0xDEADBEEF);
  basic::FileInfo infos[1] = {};
  infos[0].size = 1;

  // Check that two identical values are equivalent.
  {
    BuildValue a = BuildValue::makeSuccessfulCommandWithOutputSignature(infos, signature);
    EXPECT_EQ(a.toData(),
              BuildValue::makeSuccessfulCommandWithOutputSignature(infos, signature).toData());
  }

  // Check that a moved complex value is equivalent.
  {
    BuildValue tmp = BuildValue::makeSuccessfulCommandWithOutputSignature(infos, signature);
    BuildValue a = std::move(tmp);
    EXPECT_EQ(a.toData(),
              BuildValue::makeSuccessfulCommandWithOutputSignature(infos, signature).toData());
  }

  // Check that an rvalue initialized complex value is equivalent.
  {
    BuildValue tmp = BuildValue::makeSuccessfulCommandWithOutputSignature(infos, signature);
    BuildValue a{ std::move(tmp) };
    EXPECT_EQ(a.toData(),
              BuildValue::makeSuccessfulCommandWithOutputSignature(infos, signature).toData());
  }

  // Check that a round-tripped value is equivalent.
  EXPECT_EQ(BuildValue::makeSuccessfulCommandWithOutputSignature(infos, signature).toData(),
            BuildValue::fromData(
                BuildValue::makeSuccessfulCommandWithOutputSignature(
                    infos, signature).toData()).toData());
}

TEST(BuildValueTest, directoryListValues) {
  basic::FileInfo mockInfo{};
  std::vector<std::string> strings{ "hello", "world" };
  
  // Check that two identical values are equivalent.
  {
    BuildValue a = BuildValue::makeDirectoryContents(mockInfo, strings);
    EXPECT_EQ(a.toData(),
              BuildValue::makeDirectoryContents(mockInfo, strings).toData());
  }

  // Check that a moved complex value is equivalent.
  {
    BuildValue tmp = BuildValue::makeDirectoryContents(mockInfo, strings);
    BuildValue a = std::move(tmp);
    EXPECT_EQ(a.toData(),
              BuildValue::makeDirectoryContents(mockInfo, strings).toData());
  }

  // Check that an rvalue initialized complex value is equivalent.
  {
    BuildValue tmp = BuildValue::makeDirectoryContents(mockInfo, strings);
    BuildValue a{ std::move(tmp) };
    EXPECT_EQ(a.toData(),
              BuildValue::makeDirectoryContents(mockInfo, strings).toData());
  }

  // Check the contents are correct.
  {
    BuildValue tmp = BuildValue::makeDirectoryContents(mockInfo, strings);
    auto result = tmp.getDirectoryContents();
    EXPECT_EQ(result.size(), 2U);
    EXPECT_EQ(result[0], "hello");
    EXPECT_EQ(result[1], "world");
  }
}

TEST(BuildValueTest, staleFileRemovalValues) {
  std::vector<std::string> files { "a.out", "Info.plist" };

  {
    BuildValue a = BuildValue::makeStaleFileRemoval(ArrayRef<std::string>(files));
    auto nodes = a.getStaleFileList();
    EXPECT_EQ(2UL, nodes.size());
    EXPECT_EQ(nodes[0], "a.out");
    EXPECT_EQ(nodes[1], "Info.plist");
  }

  {
    BuildValue a = BuildValue::makeStaleFileRemoval(ArrayRef<std::string>(files));
    BuildValue b = BuildValue::fromData(a.toData());
    auto nodes = b.getStaleFileList();
    EXPECT_EQ(2UL, nodes.size());
    EXPECT_EQ(nodes[0], "a.out");
    EXPECT_EQ(nodes[1], "Info.plist");
  }
}

}
