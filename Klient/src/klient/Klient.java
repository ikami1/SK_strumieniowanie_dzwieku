/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package klient;

import java.io.*;
import java.io.InputStream;
import java.net.InetAddress;
import java.net.Socket;
import java.util.logging.*;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.SourceDataLine;

/**
 *
 * @author Aven
 */
public class Klient {
    private final static int BUFBYTESIZE = 7056;
    private final static int PORT=41111;
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        try {                     
            //init bufora na audiostream
            final AudioFormat format = new AudioFormat(44100F,16,2,true,false);//Definicja formatu audio: SampleRate, SampleSizeInBits, channels, signed, bigEndian,
            final DataLine.Info info = new DataLine.Info(SourceDataLine.class, format);
            final SourceDataLine soundLine = (SourceDataLine) AudioSystem.getLine(info);
            
            Socket sock = new Socket("127.0.0.1",PORT);//Potencjalnie zmienić String na InetAddress
            
            InputStream is = sock.getInputStream();
            
            int bufSize=7056;
            soundLine.open(format, bufSize);
            soundLine.start();
            
            byte[] buf=new byte[BUFBYTESIZE];
            while(true){
                int len = is.read(buf);
                if(len == -1) break;
                soundLine.write(buf, 0, bufSize);
            }
            
            sock.close();
        } catch (Exception ex) {
            Logger.getLogger(Klient.class.getName()).log(Level.SEVERE, null, ex);
        }
    }
    
}
