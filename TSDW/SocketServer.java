import java.net.*;
import java.io.*;

public class SocketServer
{
	public static void main(String[] args)
	{
		try
		{
			ServerSocket server = new ServerSocket(12345);
			Socket client;
			BufferedReader in;
			System.out.println("Server ready (CTRL-C quits)\n");
			boolean open=true;
			while(open)
			{
				// chiamata bloccante, in attesa di connessione da parte di un client
				client = server.accept();
				System.out.println("Client connected: "+client);
				in = new BufferedReader(new InputStreamReader(client.getInputStream()));
				String message=in.readLine();
				System.out.println(">> "+client.getInetAddress()+": "+message+"\n");
				client.close();
				if (message.equals("exit")) open=false;
			}
			server.close();
			System.out.println("Server closed");
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
}
