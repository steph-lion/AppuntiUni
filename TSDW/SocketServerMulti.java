import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

public class SocketServerMulti {

    static int port=12345;
    public static void main(String[] args) {
        
        try {
            ServerSocket server=new ServerSocket(port);
            System.out.println("Server in ascolto sulla porta "+port);
            while (true){
                Socket client=server.accept();
                System.out.println("Ho acquisito un client");
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            handleClient(client);
                        } catch (IOException e) {
                            e.printStackTrace();
                        }   
                    }
                }).start();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }
    public static void handleClient(Socket client) throws IOException{
        BufferedReader in=new BufferedReader(new InputStreamReader(client.getInputStream()));
        PrintWriter out=new PrintWriter(new BufferedWriter(new OutputStreamWriter(client.getOutputStream())),true);
        System.out.println("Thread in contatto con "+client.getInetAddress());
        while (true){
            String msg=in.readLine();
            System.out.println("Il client "+client.getInetAddress()+" ha detto: "+msg);
            if (msg.equalsIgnoreCase("exit")) {
                System.out.println("Il client "+client.getInetAddress()+" ha chiuso la connessione ");
                break;
            }
        }
        in.close();
        out.close();
        client.close();
    }
}
