/* -*- Mode: Java; tab-width: 20; indent-tabs-mode: nil; -*-
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

package org.mozilla.gecko;

import java.util.ArrayList;
import java.util.List;

import android.content.IntentSender;
import android.net.Uri;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

import com.google.android.gms.fido.Fido;
import com.google.android.gms.fido.fido2.Fido2PendingIntent;
import com.google.android.gms.fido.fido2.Fido2PrivilegedApiClient;
import com.google.android.gms.fido.fido2.api.common.AlgorithmIdentifier;
import com.google.android.gms.fido.fido2.api.common.BrowserMakeCredentialOptions;
import com.google.android.gms.fido.fido2.api.common.MakeCredentialOptions;
import com.google.android.gms.fido.fido2.api.common.PublicKeyCredentialEntity;
import com.google.android.gms.fido.fido2.api.common.PublicKeyCredentialParameters;
import com.google.android.gms.fido.fido2.api.common.PublicKeyCredentialType;
import com.google.android.gms.fido.fido2.api.common.PublicKeyCredentialUserEntity;
import com.google.android.gms.tasks.Task;
import com.google.android.gms.tasks.OnSuccessListener;

public class WebAuthnMakeCredentialActivity extends AppCompatActivity
{
    private static final String LOGTAG = "WebAuthnMakeCredentialActivity";

    @Override
    public void finish() {
        super.finish();
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Fido2PrivilegedApiClient fidoClient =
            Fido.getFido2PrivilegedApiClient(getApplicationContext());

        // ECDSA w/ SHA-256 only, for now.
        List<PublicKeyCredentialParameters> params =
            new ArrayList<PublicKeyCredentialParameters>();
        params.add(new PublicKeyCredentialParameters(
            PublicKeyCredentialType.PUBLIC_KEY.toString(),
            AlgorithmIdentifier.ECDSA.toString()));

        String userId = getIntent().getStringExtra("userId");
        Log.w(LOGTAG, "userId=" + userId);

        String userName = getIntent().getStringExtra("userName");
        Log.w(LOGTAG, "userName=" + userName);

        String userIcon = getIntent().getStringExtra("userIcon");
        Log.w(LOGTAG, "userIcon=" + userIcon);

        String userDisplayName = getIntent().getStringExtra("userDisplayName");
        Log.w(LOGTAG, "userDisplayName=" + userDisplayName);

        PublicKeyCredentialUserEntity user =
            new PublicKeyCredentialUserEntity(userId, userName, userIcon, userDisplayName);

        String rpId = getIntent().getStringExtra("rpId");
        Log.w(LOGTAG, "rpId=" + rpId);

        String rpName = getIntent().getStringExtra("rpName");
        Log.w(LOGTAG, "rpName=" + rpName);

        String rpIcon = getIntent().getStringExtra("rpIcon");
        Log.w(LOGTAG, "rpIcon=" + rpIcon);

        PublicKeyCredentialEntity rp =
            new PublicKeyCredentialEntity(rpId, rpName, rpIcon);

        byte[] challenge = getIntent().getByteArrayExtra("challenge");
        Log.w(LOGTAG, "challenge=" + challenge);

        MakeCredentialOptions requestOptions = new MakeCredentialOptions.Builder()
                .setChallenge(challenge)
                .setParameters(params)
                .setUser(user)
                .setRp(rp)
                .build();

        Uri origin = Uri.parse(getIntent().getStringExtra("origin"));
        Log.w(LOGTAG, "origin=" + origin);

        BrowserMakeCredentialOptions browserOptions = new BrowserMakeCredentialOptions.Builder()
                .setMakeCredentialOptions(requestOptions)
                .setOrigin(origin)
                .build();

        Task<Fido2PendingIntent> result = fidoClient.getRegisterIntent(browserOptions);

        result.addOnSuccessListener(new OnSuccessListener<Fido2PendingIntent>() {
            @Override
            public void onSuccess(Fido2PendingIntent pendingIntent) {
                if (pendingIntent.hasPendingIntent()) {
                    try {
                        pendingIntent.launchPendingIntent(WebAuthnMakeCredentialActivity.this, 0);
                    } catch (IntentSender.SendIntentException e) {
                        e.printStackTrace();
                    }
                }
            }
        });
    }

    @Override
    public void onBackPressed() {
        super.onBackPressed();
    }
}
