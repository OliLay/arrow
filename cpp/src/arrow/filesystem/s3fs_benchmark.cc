// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include <memory>
#include <numeric>
#include <sstream>
#include <utility>

#include "arrow/filesystem/util_internal.h"
#include "arrow/util/thread_pool.h"
#include "benchmark/benchmark.h"

#include <aws/core/auth/AWSCredentials.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/HeadBucketRequest.h>
#include <aws/s3/model/PutObjectRequest.h>

#include "arrow/filesystem/filesystem.h"
#include "arrow/filesystem/s3_internal.h"
#include "arrow/filesystem/s3_test_util.h"
#include "arrow/filesystem/s3fs.h"
#include "arrow/io/caching.h"
#include "arrow/io/interfaces.h"
#include "arrow/status.h"
#include "arrow/table.h"
#include "arrow/testing/gtest_util.h"
#include "arrow/testing/random.h"
#include "arrow/type.h"
#include "arrow/util/future.h"
#include "arrow/util/key_value_metadata.h"

#include "parquet/arrow/reader.h"
#include "parquet/arrow/writer.h"
#include "parquet/properties.h"

namespace arrow {
namespace fs {

using ::arrow::fs::internal::ConnectRetryStrategy;
using ::arrow::fs::internal::OutcomeToStatus;
using ::arrow::fs::internal::ToAwsString;

static void OpenFileAsync(benchmark::State& st) {
  const auto num_files = st.range(0);
  const auto num_threads = st.range(1);

  InitializeS3(S3GlobalOptions{}).ok();

  auto base_uri{"s3://bucket/test"};

  auto options = S3Options::FromUri("s3://bucket").ValueOrDie();

  std::shared_ptr<arrow::internal::ThreadPool> pool;
  std::shared_ptr<S3FileSystem> fs;
  pool = ::arrow::internal::ThreadPool::Make(num_threads).ValueOrDie();
  io::IOContext context{pool.get()};
  fs = S3FileSystem::Make(options, context).ValueOrDie();

  auto path{fs->PathFromUri(base_uri).ValueOrDie()};

  for (int i = 0; i < num_files; i++) {
    auto str = fs->OpenOutputStream(path + std::to_string(i) + ".txt").ValueOrDie();
    str->Write("a").ok();
    auto stat = str->Close();
  }

  for (auto _ : st) {
    std::vector<Future<std::shared_ptr<io::RandomAccessFile>>> futures;
    for (int i = 0; i < num_files; i++) {
      auto fut = fs->OpenInputFileAsync(path + std::to_string(i) + ".txt");
      futures.push_back(std::move(fut));
    }

    for (auto fut : futures) {
      fut.Wait();
    }
  }
}

BENCHMARK(OpenFileAsync)
    ->ArgsProduct({{32}, {4, 8, 16, 32}})
    ->MeasureProcessCPUTime()
    ->Iterations(1)
    ->Unit(benchmark::kSecond);

}  // namespace fs
}  // namespace arrow
