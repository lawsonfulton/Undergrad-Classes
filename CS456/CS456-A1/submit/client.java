import java.io.*;
import java.net.*;

public class client {

	public static void main(String[] args) {
		validateArguments(args);
		
		String server_address = args[0];
		int n_port = Integer.parseInt(args[1]);
		String message = args[2];		
		
		System.out.println("Attempting to contact server " + server_address + " on port " + n_port);
		try {
			int r_port = negotiateTransactionPort(server_address, n_port);
			
			System.out.println("Negotiated transaction port on " + r_port);
			
			initiateTransaction(message, server_address, r_port);
		
		} catch (UnknownHostException e) {
			e.printStackTrace();
			System.out.println("Error: The host was not recognized.");
		} catch (IOException e) {
			e.printStackTrace();
			System.out.println("Error: There was an error communicating with the server.");
		} 
		finally
		{
			System.exit(1);
		}
	}
	
	//Check that the supplied command line arguments are formatted properly
	private static void validateArguments(String[] args) {
		if(args.length != 3)
		{
			throw new IllegalArgumentException("Too few arguments");
		}
		
		if(!isValidPort(args[1]))
		{
			throw new IllegalArgumentException("Supplied port is invalid");
		}
	}
	
	private static boolean isValidPort(String port) {
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

	/* 
	 * Phase 1
	 * Communicates with the server to obtain a port over which to do the transaction.
	 */
	private static int negotiateTransactionPort(String server_address, int n_port) throws UnknownHostException, IOException {
		Socket negotiationSocket = new Socket(server_address, n_port);
		DataOutputStream outToServer = new DataOutputStream(negotiationSocket.getOutputStream());
		DataInputStream inFromServer = new DataInputStream(negotiationSocket.getInputStream());

		outToServer.write(13); //Greet the server with pre-arranged message

		int r_port = inFromServer.readInt(); //Receive the negotiated port
	
		negotiationSocket.close();	
		return r_port;
	}

	/*
	 * Phase 2
	 * Sends the message and receives the reversed reply
	 */
	private static void initiateTransaction(String message, String server_address, int r_port) throws SocketException,
			UnknownHostException, IOException {
		System.out.println("Attempting to send message: " + message);

		DatagramSocket transactionSocket = new DatagramSocket();
		sendOriginalMessage(message, server_address, r_port, transactionSocket);
		
		System.out.println("Sent message to server.");
		
		String reply = receiveReply(message, transactionSocket);
		
		System.out.println("Received reply from server: " + reply);
		
		transactionSocket.close();
	}

	private static void sendOriginalMessage(String message, String server_address, int r_port, DatagramSocket transactionSocket)
			throws UnknownHostException, IOException {
		byte[] messageData = message.getBytes();	
		
		InetAddress serverInetAddress = InetAddress.getByName(server_address);
		DatagramPacket sendPacket = new DatagramPacket(messageData, messageData.length, serverInetAddress, r_port);
		transactionSocket.send(sendPacket);
	}
	
	private static String receiveReply(String message, DatagramSocket transactionSocket) throws IOException {
		byte[] replyData = message.getBytes();
		DatagramPacket receivePacket = new DatagramPacket(replyData , replyData.length);
		transactionSocket.receive(receivePacket);
		
		String reply = new String(receivePacket.getData());
		return reply;
	}
}
