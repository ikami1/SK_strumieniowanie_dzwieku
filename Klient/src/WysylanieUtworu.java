
import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.util.logging.Level;
import java.util.logging.Logger;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Aven
 */
public class WysylanieUtworu implements Runnable{
    private final static int PORT=41112;
    private String pathname;
    
    public WysylanieUtworu(String p){
        pathname=p;
    }
    
    @Override
    public void run() {
        try {
            Socket sock = new Socket("192.168.2.54",PORT);
            OutputStream os = sock.getOutputStream();
            DataOutputStream ds= new DataOutputStream(os);
            File plik;
            synchronized(pathname){
                plik=new File(pathname);
            }
            Long l=plik.length();
            byte plikBuf[]=new byte[(int)plik.length()];
            int licz;
            String s="1";
            FileInputStream fis = new FileInputStream(plik);
            BufferedInputStream in = new BufferedInputStream(fis);
            os.write(s.getBytes("UTF-8"));
            ds.writeInt((int)plik.length());
            byte tab[] = new byte[100];
            byte temp[]=plik.getName().getBytes("UTF-8");
            for(int i=0;i<100;i++){
                if(i<=temp.length)
                    tab[i]=temp[i];
                else
                    tab[i]=' ';
            }
            os.write(tab);
            while((licz=in.read(plikBuf))>0){
                os.write(plikBuf);
            }
            sock.close();
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }
    
    public void setPath(String p){
        synchronized(pathname){
            pathname=p;
        }
    }
}
