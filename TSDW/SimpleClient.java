import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.ConnectException;
import java.net.Socket;

public class SimpleClient {
    public static void main(String args[]) {
        try { 
            System.out.println("Apertura connessione..."); 
            Socket s1 = new Socket ("151.97.119.246", 8080);
            InputStream is = s1.getInputStream(); 
            BufferedReader dis = new BufferedReader( new InputStreamReader(is));
            System.out.println("Risposta del server: " + dis.readLine());
            dis.close(); 
            s1.close(); 
            System.out.println("Chiusura connessione effettuata"); 
        } catch (ConnectException connExc) { 
            System.err.println("Errore nella connessione"); 
        } catch (IOException ex) { ex.printStackTrace(); } 
    } 
}