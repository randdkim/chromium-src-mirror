// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

package org.chromium.chrome.browser.webapps;

import android.content.Context;
import android.content.SharedPreferences;
import android.os.SystemClock;
import android.util.Log;

import org.chromium.base.ContextUtils;
import org.chromium.base.ThreadUtils;
import org.chromium.base.VisibleForTesting;
import org.chromium.base.metrics.RecordHistogram;
import org.chromium.webapk.lib.common.WebApkConstants;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import java.util.concurrent.TimeUnit;

/**
 * Before Lollipop, the only way to create multiple retargetable instances of the same Activity
 * was to explicitly define them in the Manifest.  Given that the user can potentially have an
 * unlimited number of shortcuts for launching these Activities, we have to actively assign
 * shortcuts when they launch.  Activities are reused in order of when they were last used, with
 * the least recently used ones reassigned first.
 *
 * In order to accommodate a limited number of WebappActivities with a potentially unlimited number
 * of webapps, we have to rotate the available WebappActivities between the webapps we start up.
 * Activities are reused in order of when they were last used, with the least recently used
 * ones culled first.
 *
 * It is impossible to know whether Tasks have been removed from the Recent Task list without the
 * GET_TASKS permission.  As a result, the list of Activities inside the Recent Task list will
 * be highly unlikely to match the list maintained in memory.  Instead, we store the mapping as it
 * was the last time we changed it, which allows us to launch webapps in the WebappActivity they
 * were most recently associated with in cases where a user restarts a webapp from the Recent Tasks.
 * Note that in situations where the user manually clears the app data, we will again have an
 * incorrect mapping.
 *
 * Unless otherwise noted, all methods MUST be called on the UI thread to avoid threading issues.
 *
 * EXAMPLE:
 * - 3 Activities are available for assignment (0, 1, 2).
 * - 4 webapps exist (X, Y, Z, W).
 *
 *    ACTION        EFFECT                                                ACTIVITY LIST
 * 0) Clean slate                                                         (0 -) (1 -) (2 -)
 * 1) Start X       Assigned to Activity 0 and pushed back.               (1 -) (2 -) (0 X)
 * 2) Start Y       Assigned to Activity 1 and pushed back.               (2 -) (0 X) (1 Y)
 * 3) Start Z       Assigned to Activity 2 and pushed back.               (0 X) (1 Y) (2 Z)
 * 4) Restart Y     Re-assigned to Activity 1 and pushed back.            (0 X) (2 Z) (1 Y)
 * 4) Start W       Assigned to Activity 0 and pushed back.  X evicted.   (2 Z) (1 Y) (0 W)
 * 5) Restart X     Assigned to Activity 2 and pushed back.  Z evicted.   (1 Y) (0 W) (2 X)
 */
public class ActivityAssigner {
    private static final String TAG = "ActivityAssigner";

    // Don't ever change this.  10 is enough for everyone.
    static final int NUM_WEBAPP_ACTIVITIES = 10;

    // A sanity check limit to ensure that we aren't reading an unreasonable number of preferences.
    // This number is different from above because the number of WebappActivities available may
    // change.
    static final int MAX_WEBAPP_ACTIVITIES_EVER = 100;

    // Don't ever change the package.  Left for backwards compatibility.
    @VisibleForTesting
    static final String PREF_PACKAGE[] = {"com.google.android.apps.chrome.webapps",
        "com.google.android.apps.chrome.webapps.webapk"};

    static final String PREF_NUM_SAVED_ENTRIES[] = {"ActivityAssigner.numSavedEntries",
        "ActivityAssigner.numSavedEntries.webapk"};
    static final String PREF_ACTIVITY_INDEX[] = {"ActivityAssigner.activityIndex",
        "ActivityAssigner.activityIndex.webapk"};
    static final String PREF_WEBAPP_ID[] = {"ActivityAssigner.webappId",
        "ActivityAssigner.webappId.webapk"};

    static final int INVALID_ACTIVITY_INDEX = -1;
    static final int WEBAPP_ACTIVITY_INDEX = 0;
    static final int WEBAPK_ACTIVITY_INDEX = 1;
    static final int ACTIVITY_TYPE_COUNT = 2;

    private static final Object LOCK = new Object();
    private static List<ActivityAssigner> sInstances;

    private final Context mContext;
    private final List<ActivityEntry> mActivityList;
    // The type index of the Activities managed. Either {@link WEBAPP_ACTIVITY_INDEX} or
    // {@link WEBAPK_ACTIVITY_INDEX}.
    private final int mActivityTypeIndex;

    /**
     * Pre-load shared prefs to avoid being blocked on the
     * disk access async task in the future.
     */
    public static void warmUpSharedPrefs(Context context) {
        for (int i = 0; i < ACTIVITY_TYPE_COUNT; ++i) {
            context.getSharedPreferences(PREF_PACKAGE[i], Context.MODE_PRIVATE);
        }
    }

    static class ActivityEntry {
        final int mActivityIndex;
        final String mWebappId;

        ActivityEntry(int activity, String webapp) {
            mActivityIndex = activity;
            mWebappId = webapp;
        }
    }

    /**
     * Returns the singleton instance, creating it if necessary.
     * @param webappId The app ID.
     */
    public static ActivityAssigner instance(String webappId) {
        ThreadUtils.assertOnUiThread();
        synchronized (LOCK) {
            if (sInstances == null) {
                sInstances = new ArrayList<ActivityAssigner>(ACTIVITY_TYPE_COUNT);
                for (int i = 0; i < ACTIVITY_TYPE_COUNT; ++i) {
                    sInstances.add(new ActivityAssigner(i));
                }
            }
        }
        return sInstances.get(ActivityAssigner.getIndex(webappId));
    }

    private ActivityAssigner(int activityTypeIndex) {
        mContext = ContextUtils.getApplicationContext();
        mActivityTypeIndex = activityTypeIndex;
        mActivityList = new ArrayList<ActivityEntry>();
        restoreActivityList();
    }

    /**
     * Assigns the app with the given ID to one of the available Activity instances.
     * If we know that the app was previously launched in one of the Activities, re-use it.
     * Otherwise, take the least recently used ID and use that.
     * @param webappId ID of the webapp.
     * @return Index of the Activity to use for the webapp.
     */
    int assign(String webappId) {
        // Reuse a running Activity with the same ID, if it exists.
        int activityIndex = checkIfAssigned(webappId);

        // Allocate the one in the front of the list.
        if (activityIndex == INVALID_ACTIVITY_INDEX) {
            activityIndex = mActivityList.get(0).mActivityIndex;
            ActivityEntry newEntry = new ActivityEntry(activityIndex, webappId);
            mActivityList.set(0, newEntry);
        }

        markActivityUsed(activityIndex, webappId);
        return activityIndex;
    }

    /**
     * Returns {@link WEBAPP_ACTIVITY_INDEX} for WebappActivity, {@link WEBAPK_ACTIVITY_INDEX} for
     * WebApkActivity whose webappId starts with "webapk:".
     */
    static int getIndex(String webappId) {
        return webappId.startsWith(WebApkConstants.WEBAPK_ID_PREFIX) ? WEBAPK_ACTIVITY_INDEX
                : WEBAPP_ACTIVITY_INDEX;
    }

    /**
     * Checks if the webapp with the given ID has been assigned to an Activity already.
     * @param webappId ID of the webapp being displayed.
     * @return Index of the Activity for the webapp if assigned, INVALID_ACTIVITY_INDEX otherwise.
     */
    int checkIfAssigned(String webappId) {
        if (webappId == null) {
            return INVALID_ACTIVITY_INDEX;
        }

        // Go backwards in the queue to catch more recent instances of any duplicated webapps.
        for (int i = mActivityList.size() - 1; i >= 0; i--) {
            if (webappId.equals(mActivityList.get(i).mWebappId)) {
                return mActivityList.get(i).mActivityIndex;
            }
        }
        return INVALID_ACTIVITY_INDEX;
    }

    /**
     * Moves an Activity to the back of the queue, indicating that the app is still in use and
     * shouldn't be killed.
     * @param activityIndex Index of the Activity in the LRU buffer.
     * @param webappId The ID of the app being shown in the Activity.
     */
    void markActivityUsed(int activityIndex, String webappId) {
        // Find the entry corresponding to the Activity.
        int elementIndex = findActivityElement(activityIndex);

        if (elementIndex == -1) {
            Log.e(TAG, "Failed to find WebappActivity entry: " + activityIndex + ", " + webappId);
            return;
        }

        // We have to reassign the app ID in case Activities get repurposed.
        ActivityEntry updatedEntry = new ActivityEntry(activityIndex, webappId);
        mActivityList.remove(elementIndex);
        mActivityList.add(updatedEntry);
        storeActivityList();
    }

    /**
     * Finds the index of the ActivityElement corresponding to the given activityIndex.
     * @param activityIndex Index of the activity to find.
     * @return The index of the ActivityElement in the activity list, or -1 if it couldn't be found.
     */
    private int findActivityElement(int activityIndex) {
        for (int elementIndex = 0; elementIndex < mActivityList.size(); elementIndex++) {
            if (mActivityList.get(elementIndex).mActivityIndex == activityIndex) {
                return elementIndex;
            }
        }
        return -1;
    }

    /**
     * Returns the current mapping between Activities and apps.
     */
    @VisibleForTesting
    List<ActivityEntry> getEntries() {
        return mActivityList;
    }

    /**
     * Returns the type index of the Activities managed.
     */
    @VisibleForTesting
    int getActivityTypeIndex() {
        return mActivityTypeIndex;
    }

    /**
     * Restores/creates the mapping between apps and activities.
     * The logic is slightly complicated to future-proof against situations where the number of
     * Activity is changed.
     */
    private void restoreActivityList() {
        boolean isMapDirty = false;
        mActivityList.clear();

        // Create a Set of indices corresponding to every possible Activity.
        // As ActivityEntries are read, they are and removed from this list to indicate that the
        // Activity has already been assigned.
        Set<Integer> availableWebapps = new HashSet<Integer>();
        for (int i = 0; i < NUM_WEBAPP_ACTIVITIES; ++i) {
            availableWebapps.add(i);
        }

        // Restore any entries that were previously saved.  If it seems that the preferences have
        // been corrupted somehow, just discard the whole map.
        SharedPreferences prefs = mContext.getSharedPreferences(PREF_PACKAGE[mActivityTypeIndex],
                Context.MODE_PRIVATE);
        try {
            long time = SystemClock.elapsedRealtime();
            final int numSavedEntries = prefs.getInt(PREF_NUM_SAVED_ENTRIES[mActivityTypeIndex], 0);
            try {
                RecordHistogram.recordTimesHistogram("Android.StrictMode.WebappSharedPrefs",
                        SystemClock.elapsedRealtime() - time, TimeUnit.MILLISECONDS);
            } catch (UnsatisfiedLinkError error) {
                // Intentionally ignored - it's ok to miss recording the metric occasionally.
            }
            if (numSavedEntries <= NUM_WEBAPP_ACTIVITIES) {
                for (int i = 0; i < numSavedEntries; ++i) {
                    String currentActivityIndexPref = PREF_ACTIVITY_INDEX[mActivityTypeIndex] + i;
                    String currentWebappIdPref = PREF_WEBAPP_ID[mActivityTypeIndex] + i;

                    int activityIndex = prefs.getInt(currentActivityIndexPref, i);
                    String webappId = prefs.getString(currentWebappIdPref, null);
                    ActivityEntry entry = new ActivityEntry(activityIndex, webappId);

                    if (availableWebapps.remove(entry.mActivityIndex)) {
                        mActivityList.add(entry);
                    } else {
                        // If the same activity was assigned to two different entries, or if the
                        // number of activities changed, discard it and mark that it needs to be
                        // rewritten.
                        isMapDirty = true;
                    }
                }
            }
        } catch (ClassCastException exception) {
            // Something went wrong reading the preferences.  Nuke everything.
            mActivityList.clear();
            availableWebapps.clear();
            for (int i = 0; i < NUM_WEBAPP_ACTIVITIES; ++i) {
                availableWebapps.add(i);
            }
        }

        // Add entries for any missing Activities.
        for (Integer availableIndex : availableWebapps) {
            ActivityEntry entry = new ActivityEntry(availableIndex, null);
            mActivityList.add(entry);
            isMapDirty = true;
        }

        if (isMapDirty) {
            storeActivityList();
        }
    }

    /**
     * Saves the mapping between apps and Activities.
     */
    private void storeActivityList() {
        SharedPreferences prefs = mContext.getSharedPreferences(PREF_PACKAGE[mActivityTypeIndex],
                Context.MODE_PRIVATE);
        SharedPreferences.Editor editor = prefs.edit();
        editor.clear();
        editor.putInt(PREF_NUM_SAVED_ENTRIES[mActivityTypeIndex], mActivityList.size());
        for (int i = 0; i < mActivityList.size(); ++i) {
            String currentActivityIndexPref = PREF_ACTIVITY_INDEX[mActivityTypeIndex] + i;
            String currentWebappIdPref = PREF_WEBAPP_ID[mActivityTypeIndex] + i;
            editor.putInt(currentActivityIndexPref, mActivityList.get(i).mActivityIndex);
            editor.putString(currentWebappIdPref, mActivityList.get(i).mWebappId);
        }
        editor.apply();
    }
}
