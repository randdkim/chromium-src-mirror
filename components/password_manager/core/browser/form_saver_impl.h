// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_PASSWORD_MANAGER_CORE_BROWSER_FORM_SAVER_IMPL_H_
#define COMPONENTS_PASSWORD_MANAGER_CORE_BROWSER_FORM_SAVER_IMPL_H_

#include <memory>

#include "base/macros.h"
#include "components/password_manager/core/browser/form_saver.h"

namespace password_manager {

class PasswordStore;

// The production code implementation of FormSaver.
class FormSaverImpl : public FormSaver {
 public:
  // |store| needs to outlive |this| and will be used for all PasswordStore
  // operations.
  explicit FormSaverImpl(PasswordStore* store);

  ~FormSaverImpl() override;

  // FormSaver:
  void PermanentlyBlacklist(autofill::PasswordForm* observed) override;
  void Save(const autofill::PasswordForm& pending,
            const autofill::PasswordFormMap& best_matches,
            const autofill::PasswordForm* old_primary_key) override;
  void Update(
      const autofill::PasswordForm& pending,
      const autofill::PasswordFormMap& best_matches,
      const std::vector<const autofill::PasswordForm*>* credentials_to_update,
      const autofill::PasswordForm* old_primary_key) override;
  void PresaveGeneratedPassword(
      const autofill::PasswordForm& generated) override;
  void RemovePresavedPassword() override;

 private:
  // Implements both Save and Update, because those methods share most of the
  // code.
  void SaveImpl(
      const autofill::PasswordForm& pending,
      bool is_new_login,
      const autofill::PasswordFormMap& best_matches,
      const std::vector<const autofill::PasswordForm*>* credentials_to_update,
      const autofill::PasswordForm* old_primary_key);

  // Marks all of |best_matches_| as not preferred unless the username is
  // |preferred_username| or the credential is PSL matched.
  void UpdatePreferredLoginState();

  // Iterates over all |best_matches| and deletes from the password store all
  // which are not PSL-matched, have an empty username, and a password equal to
  // |pending_->password_value|.
  void DeleteEmptyUsernameCredentials();

  // Cached pointer to the PasswordStore.
  PasswordStore* const store_;

  // Caches the best matches during a call to Save() or Update().
  const autofill::PasswordFormMap* best_matches_ = nullptr;

  // Caches the pending credential during a call to Save() or Update().
  const autofill::PasswordForm* pending_ = nullptr;

  // Stores the pre-saved credential (happens during password generation).
  std::unique_ptr<autofill::PasswordForm> presaved_;

  DISALLOW_COPY_AND_ASSIGN(FormSaverImpl);
};

}  // namespace password_manager

#endif  // COMPONENTS_PASSWORD_MANAGER_CORE_BROWSER_FORM_SAVER_IMPL_H_
