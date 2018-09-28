import java.io.*;
import java.net.*;

public class server {
	public static void main(String[] args) {
		try {
			Socket negotiationSocket = waitForClientConnection();
			
			//Create a UDP socket for the transaction phase
			DatagramSocket transactionSocket = new DatagramSocket();
			int r_port = transactionSocket.getLocalPort();
			
			sendTransactionPort(negotiationSocket, r_port);
			
			System.out.println("Opened transaction datagram socket on port " + r_port);	
			
			handleClientMessage(transactionSocket);
			
			negotiationSocket.close();
			transactionSocket.close();
		} catch (IOException e) {
			e.printStackTrace();
			System.out.println("Error: There was an error communicating with the client.");
		}
		
	}

	/*
	 * Block until the server is contacted by a valid client. Return the resulting socket.
	 */
	private static Socket waitForClientConnection() throws IOException {
		//Open a socket to listen for incoming connections
		ServerSocket welcomeSocket = new ServerSocket(0);
		int n_port = welcomeSocket.getLocalPort();
		
		Socket negotiationSocket;
		
		//Keep waiting for connections indefinitely
		while(true)
		{
			System.out.println("Listening on port " + n_port);
			
			negotiationSocket = welcomeSocket.accept();
			System.out.println("Contacted by client at " + negotiationSocket.getInetAddress());

			int greeting = getGreeting(negotiationSocket);
			
			//Check to see that we are being contacted by the right client
			if(isgreetingValid(greeting))
			{
				break;
			}
			else
			{
				System.out.println("Received invalid greeting.");
			}
		}
		
		welcomeSocket.close();
		return negotiationSocket;
	}
	
	private static int getGreeting(Socket negotiationSocket) throws IOException {
		BufferedReader inFromClient = new BufferedReader(new InputStreamReader(negotiationSocket.getInputStream()));
		int greeting = inFromClient.read();
		return greeting;
	}

	private static boolean isgreetingValid(int greeting) {
		return greeting == 13;
	}
	
	/*
	 * Sent the negotiated transaction port to the client via TCP
	 */
	private static void sendTransactionPort(Socket negotiationSocket, int r_port) throws IOException {
		DataOutputStream outToClient = new DataOutputStream(negotiationSocket.getOutputStream());
		outToClient.writeInt(r_port);
	}

	/*
	 * Receive a message from the client, reverse it, and send it back to its source.
	 */
	private static void handleClientMessage(DatagramSocket transactionSocket) throws IOException {
		byte[] receiveData = new byte[65535];
		DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);
		transactionSocket.receive(receivePacket);
		
		String message = new String(receivePacket.getData()).trim();
		InetAddress client_address = receivePacket.getAddress();
		int clientPort = receivePacket.getPort();
		
		System.out.println("Received datagram on transaction port containing: " + message);
		
		String reply = new StringBuilder(message).reverse().toString();
		byte[] replyData = reply.getBytes();	
		DatagramPacket sendPacket = new DatagramPacket(replyData, replyData.length, client_address, clientPort);
		transactionSocket.send(sendPacket);
				
		System.out.println("Sent reply to client.");
	}
}
