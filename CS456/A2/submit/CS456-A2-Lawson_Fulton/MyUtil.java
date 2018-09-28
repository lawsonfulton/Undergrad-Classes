import java.io.*;
import java.net.*;
import java.util.*;
import java.nio.charset.Charset;
import java.io.File;
import java.util.Scanner;

public class MyUtil
{
	//Check that the supplied command line arguments are formatted properly
	public static void validateArguments(String[] args) {
		if(args.length != 4)
		{
			System.out.println("Error: Incorrect number of args.");
			System.exit(1);
		}
		
		if(!isValidPort(args[1]) || !isValidPort(args[2]))
		{
			System.out.println("Error: Invalid port number.");
			System.exit(1);
		}
	}

	public static String loadFile(String fileName, Charset encoding) throws Exception
	{
		Scanner in = new Scanner(new File(fileName));
		in.useDelimiter("\\A");

		String output = in.next();

  		return output;
	}
	
	public static boolean isValidPort(String port) {
		int value;
		try { 
	        value = Integer.parseInt(port); 
	    } catch(NumberFormatException e) { 
	        return false; 
	    }
		if(value < 0 || value > 65535) 
		{
			return false;
		}
		
	    return true;
	}

	public static ArrayList<String> splitInChunks(String s, int chunkSize)
	{
	    ArrayList<String> result = new ArrayList<String>();
	    
	    int length = s.length();
	    for (int i = 0; i < length; i += chunkSize)
	    {
	        result.add(s.substring(i, Math.min(length, i + chunkSize)));
    	}

    	return result;
	}

	public static packet receivePacket(DatagramSocket socket) throws Exception
	{
		byte[] buf = new byte[512];
		DatagramPacket datagramPacket = new DatagramPacket(buf, buf.length);
		socket.receive(datagramPacket);

		return packet.parseUDPdata(buf);
	}

	public static void sendPacket(packet pkt, DatagramSocket socket, 
									InetAddress address, int port) throws Exception
	{
		byte[] messageData = pkt.getUDPdata();
		DatagramPacket sendPacket = new DatagramPacket(messageData, messageData.length, address, port);
		socket.send(sendPacket);
	}
}




