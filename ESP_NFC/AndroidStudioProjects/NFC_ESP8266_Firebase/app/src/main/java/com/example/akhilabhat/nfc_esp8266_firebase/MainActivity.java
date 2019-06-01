package com.example.akhilabhat.nfc_esp8266_firebase;

import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.TextView;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Date;

public class MainActivity extends AppCompatActivity {

    private DatabaseReference lastRead_M, lastRead_A, lastRead_S, lastRead_Y;
    TextView Mt, Mn, At, An, St, Sn, Yt, Yn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        final DateFormat df = new SimpleDateFormat("MM/dd/yyyy HH:mm:ss");

        Mt = (TextView) findViewById(R.id.marktime);
        Mn = (TextView) findViewById(R.id.markname);
        Mn.setText("MARK");

        At = (TextView) findViewById(R.id.akhilatime);
        An = (TextView) findViewById(R.id.akhilaname);
        An.setText("AKHILA");

        St = (TextView) findViewById(R.id.sarnabtime);
        Sn = (TextView) findViewById(R.id.sarnabname);
        Sn.setText("SARNAB");

        Yt = (TextView) findViewById(R.id.yashtime);
        Yn = (TextView) findViewById(R.id.yashname);
        Yn.setText("YASH");

        lastRead_M = FirebaseDatabase.getInstance().getReference().child("Mark").child("Time");
        lastRead_A = FirebaseDatabase.getInstance().getReference().child("Akhila").child("Time");
        lastRead_S = FirebaseDatabase.getInstance().getReference().child("Sarnab").child("Time");
        lastRead_Y = FirebaseDatabase.getInstance().getReference().child("Yash").child("Time");

        lastRead_M.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot dataSnapshot) {
                long t = Long.valueOf(dataSnapshot.getValue().toString());
                Date time = new Date(t);
                String date = df.format(time);
                Mt.setText(date);
            }
            @Override
            public void onCancelled(@NonNull DatabaseError databaseError) { }
        });

        lastRead_A.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot dataSnapshot) {
                long t = Long.valueOf(dataSnapshot.getValue().toString());
                Date time = new Date(t);
                String date = df.format(time);
                At.setText(date);
            }
            @Override
            public void onCancelled(@NonNull DatabaseError databaseError) { }
        });

        lastRead_S.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot dataSnapshot) {
                long t = Long.valueOf(dataSnapshot.getValue().toString());
                Date time = new Date(t);
                String date = df.format(time);
                St.setText(date);
            }
            @Override
            public void onCancelled(@NonNull DatabaseError databaseError) { }
        });

        lastRead_Y.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot dataSnapshot) {
                long t = Long.valueOf(dataSnapshot.getValue().toString());
                Date time = new Date(t);
                String date = df.format(time);
                Yt.setText(date);
            }
            @Override
            public void onCancelled(@NonNull DatabaseError databaseError) { }
        });
    }
}
