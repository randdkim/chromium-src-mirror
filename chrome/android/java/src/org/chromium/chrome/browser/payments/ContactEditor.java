// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.payments;

import android.content.Context;
import android.telephony.PhoneNumberUtils;
import android.util.Patterns;

import org.chromium.base.Callback;
import org.chromium.chrome.R;
import org.chromium.chrome.browser.autofill.PersonalDataManager;
import org.chromium.chrome.browser.autofill.PersonalDataManager.AutofillProfile;
import org.chromium.chrome.browser.payments.ui.EditorFieldModel;
import org.chromium.chrome.browser.payments.ui.EditorFieldModel.EditorFieldValidator;
import org.chromium.chrome.browser.payments.ui.EditorModel;
import org.chromium.chrome.browser.payments.ui.EditorView;

import java.util.ArrayList;
import java.util.List;

import javax.annotation.Nullable;

/**
 * Contact information editor.
 */
public class ContactEditor {
    private final boolean mRequestPayerPhone;
    private final boolean mRequestPayerEmail;
    private final List<CharSequence> mPhoneNumbers;
    private final List<CharSequence> mEmailAddresses;
    @Nullable private EditorView mEditorView;
    @Nullable private Context mContext;
    @Nullable private EditorFieldValidator mPhoneValidator;
    @Nullable private EditorFieldValidator mEmailValidator;

    /**
     * Builds a contact information editor.
     *
     * @param requestPayerPhone Whether to request the user's phone number.
     * @param requestPayerEmail Whether to request the user's email address.
     */
    public ContactEditor(boolean requestPayerPhone, boolean requestPayerEmail) {
        assert requestPayerPhone || requestPayerEmail;
        mRequestPayerPhone = requestPayerPhone;
        mRequestPayerEmail = requestPayerEmail;
        mPhoneNumbers = new ArrayList<>();
        mEmailAddresses = new ArrayList<>();
    }

    /**
     * Returns whether the following contact information can be sent to the merchant as-is without
     * editing first.
     *
     * @param phone The phone number to check.
     * @param email The email address to check.
     * @return Whether the contact information is complete.
     */
    public boolean isContactInformationComplete(@Nullable String phone, @Nullable String email) {
        return (!mRequestPayerPhone || getPhoneValidator().isValid(phone))
                && (!mRequestPayerEmail || getEmailValidator().isValid(email));
    }

    /**
     * Sets the user interface to be used for editing contact information.
     *
     * @param editorView The user interface to be used.
     */
    public void setEditorView(EditorView editorView) {
        assert editorView != null;
        mEditorView = editorView;
        mContext = mEditorView.getContext();
    }

    /**
     * Adds the given phone number to the autocomplete list, if it's valid.
     *
     * @param phoneNumber The phone number to possibly add.
     */
    public void addPhoneNumberIfValid(@Nullable CharSequence phoneNumber) {
        if (getPhoneValidator().isValid(phoneNumber)) mPhoneNumbers.add(phoneNumber);
    }

    /**
     * Adds the given email address to the autocomplete list, if it's valid.
     *
     * @param emailAddress The email address to possibly add.
     */
    public void addEmailAddressIfValid(@Nullable CharSequence emailAddress) {
        if (getEmailValidator().isValid(emailAddress)) mEmailAddresses.add(emailAddress);
    }

    /**
     * Shows the user interface for editing the given contact. The contact is also updated on disk,
     * so there's no need to do that in the calling code.
     *
     * @param toEdit   The contact to edit. Can be null if the user is adding a new contact instead
     *                 of editing an existing one.
     * @param callback The callback to invoke with the complete and valid contact information. Can
     *                 be invoked with null if the user clicked Cancel.
     */
    public void editContact(
            @Nullable AutofillContact toEdit, final Callback<AutofillContact> callback) {
        assert mEditorView != null;
        assert mContext != null;

        final AutofillContact contact = toEdit == null
                ? new AutofillContact(new AutofillProfile(), null, null, false) : toEdit;

        final EditorFieldModel phoneField = mRequestPayerPhone
                ? new EditorFieldModel(EditorFieldModel.INPUT_TYPE_HINT_PHONE,
                          mContext.getString(R.string.autofill_profile_editor_phone_number),
                          mPhoneNumbers, getPhoneValidator(),
                          mContext.getString(R.string.payments_phone_required_validation_message),
                          mContext.getString(R.string.payments_phone_invalid_validation_message),
                          contact.getPayerPhone())
                : null;

        final EditorFieldModel emailField = mRequestPayerEmail
                ? new EditorFieldModel(EditorFieldModel.INPUT_TYPE_HINT_EMAIL,
                          mContext.getString(R.string.autofill_profile_editor_email_address),
                          mEmailAddresses, getEmailValidator(),
                          mContext.getString(R.string.payments_email_required_validation_message),
                          mContext.getString(R.string.payments_email_invalid_validation_message),
                          contact.getPayerEmail())
                : null;

        EditorModel editor =
                new EditorModel(mContext.getString(R.string.payments_add_contact_details_label));
        if (phoneField != null) editor.addField(phoneField);
        if (emailField != null) editor.addField(emailField);

        editor.setCancelCallback(new Runnable() {
            @Override
            public void run() {
                callback.onResult(null);
            }
        });

        editor.setDoneCallback(new Runnable() {
            @Override
            public void run() {
                String phone = null;
                String email = null;

                if (phoneField != null) {
                    phone = phoneField.getValue().toString();
                    contact.getProfile().setPhoneNumber(phone);
                }

                if (emailField != null) {
                    email = emailField.getValue().toString();
                    contact.getProfile().setEmailAddress(email);
                }

                PersonalDataManager.getInstance().setProfile(contact.getProfile());
                contact.completeContact(phone, email);
                callback.onResult(contact);
            }
        });

        mEditorView.show(editor);
    }

    private EditorFieldValidator getPhoneValidator() {
        if (mPhoneValidator == null) {
            mPhoneValidator = new EditorFieldValidator() {
                @Override
                public boolean isValid(@Nullable CharSequence value) {
                    return value != null
                            && PhoneNumberUtils.isGlobalPhoneNumber(
                                       PhoneNumberUtils.stripSeparators(value.toString()));
                }
            };
        }
        return mPhoneValidator;
    }

    private EditorFieldValidator getEmailValidator() {
        if (mEmailValidator == null) {
            mEmailValidator = new EditorFieldValidator() {
                @Override
                public boolean isValid(@Nullable CharSequence value) {
                    return value != null && Patterns.EMAIL_ADDRESS.matcher(value).matches();
                }
            };
        }
        return mEmailValidator;
    }
}
