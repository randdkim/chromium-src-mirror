// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_PASSWORD_MANAGER_CORE_BROWSER_STUB_FORM_SAVER_H_
#define COMPONENTS_PASSWORD_MANAGER_CORE_BROWSER_STUB_FORM_SAVER_H_

#include "base/macros.h"
#include "components/password_manager/core/browser/form_saver.h"

namespace password_manager {

// The no-op FormSaver stub to support tests.
class StubFormSaver : public FormSaver {
 public:
  StubFormSaver() = default;

  ~StubFormSaver() override = default;

  // FormSaver:
  void PermanentlyBlacklist(autofill::PasswordForm* observed) override {}
  void Save(const autofill::PasswordForm& pending,
            const autofill::PasswordFormMap& best_matches,
            const autofill::PasswordForm* old_primary_key) override {}
  void Update(
      const autofill::PasswordForm& pending,
      const autofill::PasswordFormMap& best_matches,
      const std::vector<const autofill::PasswordForm*>* credentials_to_update,
      const autofill::PasswordForm* old_primary_key) override {}
  void PresaveGeneratedPassword(
      const autofill::PasswordForm& generated) override {}
  void RemovePresavedPassword() override {}

 private:
  DISALLOW_COPY_AND_ASSIGN(StubFormSaver);
};

}  // namespace password_manager

#endif  // COMPONENTS_PASSWORD_MANAGER_CORE_BROWSER_STUB_FORM_SAVER_H_
