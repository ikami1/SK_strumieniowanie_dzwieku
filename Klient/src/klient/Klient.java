/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
package klient;

import java.io.*;
import java.io.InputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.nio.ByteBuffer;
import java.util.logging.*;
import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.SourceDataLine;

/**
 *
 * @author Aven
 */
public class Klient extends Thread{
    private final static int BUFBYTESIZE = 8*1024;
    private final static int PORT=41111;
    static AudioInputStream ais;
    private static Boolean rozlacz;
    
    public Klient(){
        rozlacz=true;
    }
    
    @Override
    public void start() {
        while(true){
            try {  
                while(rozlacz){
                    Thread.sleep(500);
                }
                //init bufora na audiostream
                final AudioFormat format = new AudioFormat(44100F,16,2,true,false);//Definicja formatu audio: SampleRate, SampleSizeInBits, channels, signed, bigEndian,
                final DataLine.Info info = new DataLine.Info(SourceDataLine.class, format);
                final SourceDataLine soundLine = (SourceDataLine) AudioSystem.getLine(info);
                Socket sock = new Socket("192.168.2.54",PORT);
                InputStream is = sock.getInputStream();
                int bufSize=8*1024;
                soundLine.open(format);
                soundLine.start();

                byte[] buf=new byte[BUFBYTESIZE];

                while(true){
                    int len = is.read(buf);
                    synchronized(rozlacz){
                        if(rozlacz) break;
                    }
                    soundLine.write(buf, 0, bufSize);
                }

                soundLine.drain();
                soundLine.close();
                sock.close();
            } catch (Exception ex) {
                synchronized(rozlacz){
                    rozlacz=true;
                }
                ex.printStackTrace();
            }
        }
    }
    
    public Boolean getStatus(){
        synchronized(rozlacz){
            return !rozlacz;
        }
    }
    
    public void changeStatus(){
        synchronized(rozlacz){
            rozlacz=!rozlacz;
        }
    }
}
