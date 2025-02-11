// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_METRICS_ANTIVIRUS_METRICS_PROVIDER_WIN_H_
#define CHROME_BROWSER_METRICS_ANTIVIRUS_METRICS_PROVIDER_WIN_H_

#include "components/metrics/metrics_provider.h"

#include <iwscapi.h>
#include <stddef.h>

#include <string>
#include <vector>

#include "base/callback_forward.h"
#include "base/feature_list.h"
#include "base/gtest_prod_util.h"
#include "base/macros.h"
#include "base/memory/weak_ptr.h"
#include "base/sequenced_task_runner.h"
#include "base/threading/thread_checker.h"
#include "components/metrics/proto/system_profile.pb.h"

// AntiVirusMetricsProvider is responsible for adding antivirus information to
// the UMA system profile proto.
class AntiVirusMetricsProvider : public metrics::MetricsProvider {
 public:
  static constexpr base::Feature kReportNamesFeature = {
      "ReportFullAVProductDetails", base::FEATURE_DISABLED_BY_DEFAULT};

  explicit AntiVirusMetricsProvider(
      scoped_refptr<base::TaskRunner> task_runner);

  ~AntiVirusMetricsProvider() override;

  // metrics::MetricsDataProvider:
  void ProvideSystemProfileMetrics(
      metrics::SystemProfileProto* system_profile_proto) override;

  // Fetches AntiVirus data asynchronously and calls |done_callback| when
  // done. Should be called before ProvideSystemProfileMetrics to ensure that
  // data is ready to be collected.
  void GetAntiVirusMetrics(const base::Closure& done_callback);

 private:
  // This enum is reported via a histogram so new values should always be added
  // at the end.
  enum ResultCode {
    RESULT_SUCCESS = 0,
    RESULT_GENERIC_FAILURE = 1,
    RESULT_FAILED_TO_INITIALIZE_COM = 2,
    RESULT_FAILED_TO_CREATE_INSTANCE = 3,
    RESULT_FAILED_TO_INITIALIZE_PRODUCT_LIST = 4,
    RESULT_FAILED_TO_GET_PRODUCT_COUNT = 5,
    RESULT_FAILED_TO_GET_ITEM = 6,
    RESULT_FAILED_TO_GET_PRODUCT_STATE = 7,
    RESULT_PRODUCT_STATE_INVALID = 8,
    RESULT_FAILED_TO_GET_PRODUCT_NAME = 9,
    RESULT_FAILED_TO_GET_REMEDIATION_PATH = 10,
    RESULT_COUNT = 11
  };

  typedef metrics::SystemProfileProto::AntiVirusProduct AvProduct;

  static ResultCode FillAntiVirusProducts(std::vector<AvProduct>* products);
  static std::vector<AvProduct> GetAntiVirusProductsOnFileThread();

  // Called when metrics are done being gathered from the FILE thread.
  // |done_callback| is the callback that should be called once all metrics are
  // gathered.
  void GotAntiVirusProducts(const base::Closure& done_callback,
                            const std::vector<AvProduct>& av_products);

  // The TaskRunner on which file operations are performed (supplied by the
  // embedder).
  scoped_refptr<base::TaskRunner> task_runner_;

  // Information on installed AntiVirus gathered.
  std::vector<AvProduct> av_products_;

  base::ThreadChecker thread_checker_;
  base::WeakPtrFactory<AntiVirusMetricsProvider> weak_ptr_factory_;

  DISALLOW_COPY_AND_ASSIGN(AntiVirusMetricsProvider);
};

#endif  // CHROME_BROWSER_METRICS_ANTIVIRUS_METRICS_PROVIDER_WIN_H_
