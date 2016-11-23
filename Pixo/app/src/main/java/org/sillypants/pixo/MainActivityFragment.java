package org.sillypants.pixo;

import android.support.v4.app.Fragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.util.Log;
import android.content.res.AssetManager;
import java.io.File;

/**
 * A placeholder fragment containing a simple view.
 */
public class MainActivityFragment extends Fragment {

    public MainActivityFragment() {
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
            Bundle savedInstanceState) {
//        View v = inflater.inflate(R.layout.fragment_main, container, false);
//
//        // Example of a call to a native method
//        TextView tv = (TextView) v.findViewById(R.id.sample_text);
//        tv.setText(stringFromJNI());

        return new PixView(getActivity());
    }

}
