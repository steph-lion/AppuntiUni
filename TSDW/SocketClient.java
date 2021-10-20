import java.net.*;
import java.io.*;

public class SocketClient
{
	public static void main(String[] args)
	{
		// verifica correttezza dei parametri
		if (args.length != 1)
		{
			System.out.println("Usage: java myclient \"message to send\"");
			return;
		}
		try
		{
			InetAddress address = InetAddress.getByName("localhost");
			Socket client = new Socket(address, 12345);
			System.out.println("Client ready.\n");
			PrintWriter out = new PrintWriter(new BufferedWriter(new OutputStreamWriter(client.getOutputStream())), true);
			System.out.println("Buffer ready, sending message \""+args[0]+"\"...\n");
			out.println(args[0]);
			System.out.println("Message sent.\n");
			client.close();
		}
		catch (Exception e)
		{
			e.printStackTrace();
		}
	}
}