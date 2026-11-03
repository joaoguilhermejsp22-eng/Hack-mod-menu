package com.anon.ffmenu;
import android.app.Activity;
import android.os.Bundle;
import android.widget.LinearLayout;
import android.widget.Switch;
import android.widget.TextView;
public class MainActivity extends Activity {
    static { System.loadLibrary("ModMenu"); }
    private native void toggle(int id, boolean on);
    @Override protected void onCreate(Bundle b){
        super.onCreate(b);
        LinearLayout ll = new LinearLayout(this);
        ll.setOrientation(LinearLayout.VERTICAL);
        TextView t = new TextView(this); t.setText("FF Menu"); ll.addView(t);
        addSwitch(ll,"ESP",1); addSwitch(ll,"Aimbot",2); addSwitch(ll,"No Recoil",3);
        setContentView(ll);
    }
    private void addSwitch(LinearLayout p, String label, final int id){
        Switch s = new Switch(this); s.setText(label);
        s.setOnCheckedChangeListener((v,on)->toggle(id,on));
        p.addView(s);
    }
}
