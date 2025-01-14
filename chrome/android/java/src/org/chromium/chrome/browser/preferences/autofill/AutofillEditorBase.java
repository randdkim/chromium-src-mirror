// Copyright 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.preferences.autofill;

import android.app.Fragment;
import android.content.Context;
import android.os.Bundle;
import android.text.Editable;
import android.text.TextWatcher;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.Button;
import android.widget.LinearLayout;

import org.chromium.chrome.R;
import org.chromium.chrome.browser.payments.ui.EditorView;
import org.chromium.chrome.browser.payments.ui.FadingEdgeScrollView;
import org.chromium.chrome.browser.widget.DualControlLayout;

/** Base class for Autofill editors (e.g. credit cards and profiles). */
public abstract class AutofillEditorBase
        extends Fragment implements OnItemSelectedListener, TextWatcher {

    /** GUID of the profile we are editing.  Empty if creating a new profile. */
    protected String mGUID;

    /** Whether or not the editor is creating a new entry. */
    protected boolean mIsNewEntry;

    /** Context for the app. */
    protected Context mContext;

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
        super.onCreateView(inflater, container, savedInstanceState);
        setHasOptionsMenu(true);
        mContext = container.getContext();

        // We know which profile to edit based on the GUID stuffed in
        // our extras by AutofillPreferences.
        Bundle extras = getArguments();
        if (extras != null) {
            mGUID = extras.getString(AutofillPreferences.AUTOFILL_GUID);
        }
        if (mGUID == null) {
            mGUID = "";
            mIsNewEntry = true;
        } else {
            mIsNewEntry = false;
        }
        getActivity().setTitle(getTitleResourceId(mIsNewEntry));

        // Hide the top shadow on the ScrollView because the toolbar draws one.
        FadingEdgeScrollView scrollView = (FadingEdgeScrollView) inflater.inflate(
                R.layout.autofill_editor_base, container, false);
        scrollView.setShadowVisibility(false, true);

        // Inflate the editor and buttons into the "content" LinearLayout.
        LinearLayout contentLayout = (LinearLayout) scrollView.findViewById(R.id.content);
        inflater.inflate(getLayoutId(), contentLayout, true);
        inflater.inflate(R.layout.autofill_editor_base_buttons, contentLayout, true);

        return scrollView;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        if (item.getItemId() == R.id.delete_menu_id) {
            deleteEntry();
            getActivity().finish();
            return true;
        } else if (item.getItemId() == R.id.help_menu_id) {
            EditorView.launchAutofillHelpPage(mContext);
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    @Override
    public void onCreateOptionsMenu(Menu menu, MenuInflater inflater) {
        menu.clear();
        inflater.inflate(R.menu.payments_editor_menu, menu);

        MenuItem deleteItem = menu.findItem(R.id.delete_menu_id);
        if (deleteItem != null) deleteItem.setVisible(!mIsNewEntry);
    }

    /** Initializes the buttons within the layout. */
    protected void initializeButtons(View layout) {
        DualControlLayout buttonBar = (DualControlLayout) layout.findViewById(R.id.button_bar);
        buttonBar.setAlignment(DualControlLayout.ALIGN_END);

        Button button = (Button) layout.findViewById(R.id.button_secondary);
        button.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    getActivity().finish();
                }
            });

        button = (Button) layout.findViewById(R.id.button_primary);
        button.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    saveEntry();
                    getActivity().finish();
                }
            });
        button.setEnabled(false);
    }

    /** Returns the ID of the layout to inflate. */
    protected abstract int getLayoutId();

    /** Called when the entry should be saved. */
    protected abstract void saveEntry();

    /** Called when the entry being edited should be deleted. */
    protected abstract void deleteEntry();

    /** @return ID of the String to use as the title in the ActionBar. */
    protected abstract int getTitleResourceId(boolean isNewEntry);

    @Override
    public void onNothingSelected(AdapterView<?> parent) {}

    @Override
    public void afterTextChanged(Editable s) {}

    @Override
    public void beforeTextChanged(CharSequence s, int start, int count, int after) {}
}
