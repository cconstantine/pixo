package org.sillypants.pixo;

import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.widget.TextView;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import fr.bmartel.android.fadecandy.client.FadecandyClient;
import fr.bmartel.android.fadecandy.inter.IFcServerEventListener;
import fr.bmartel.android.fadecandy.inter.IUsbEventListener;
import fr.bmartel.android.fadecandy.model.ServerError;
import fr.bmartel.android.fadecandy.model.ServiceType;
import fr.bmartel.android.fadecandy.model.UsbItem;


public class MainActivity extends AppCompatActivity {
    FadecandyClient mFadecandyClient;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }
    @Override
    public void onResume() {
        super.onResume();  // Always call the superclass method first

        mFadecandyClient = new FadecandyClient(this,
		new IFcServerEventListener() {
            @Override
            public void onServerStart() {
                Log.v("MainActivity", "onServerStart");
            }

            @Override
            public void onServerClose() {
                Log.v("MainActivity", "onServerClose");
            }

            @Override
            public void onServerError(ServerError error) {
            	// a server error occured
            }
        }, new IUsbEventListener() {
            @Override
            public void onUsbDeviceAttached(UsbItem usbItem) {
                // a Fadecandy device has been attached
            }

            @Override
            public void onUsbDeviceDetached(UsbItem usbItem) {
                // a Fadecandy device has been detached
            }
        },
        "org.sillypants.pixo/.activity.MainActivity"
        );

        mFadecandyClient.startServer();
        mFadecandyClient.setServiceType(ServiceType.NON_PERSISTENT_SERVICE);


    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }
    @Override
    protected void onStop() {
        mFadecandyClient.stopService();
        super.onStop();
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("native-lib");
    }
}
