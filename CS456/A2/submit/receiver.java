import java.io.*;
import java.net.*;
import java.util.*;
import java.nio.charset.Charset;

public class receiver {
	static InetAddress emulatorInetAddress;
	static DatagramSocket socket;
	static PrintWriter dataWriter;
	static PrintWriter arrivalWriter;
	static int outgoingPort;
	static int incomingPort;

	private static void initializeVariables(String[] args) throws Exception
	{
		String emulatorAddress = args[0];
		outgoingPort = Integer.parseInt(args[1]);
		incomingPort = Integer.parseInt(args[2]);		
		String fileName = args[3];

		dataWriter = new PrintWriter(fileName, "UTF-8");
		arrivalWriter = new PrintWriter("arrival.log", "UTF-8");

		emulatorInetAddress = InetAddress.getByName(emulatorAddress); 
		socket = new DatagramSocket(incomingPort);
	}

	public static void main(String[] args) throws Exception {
		MyUtil.validateArguments(args);
		initializeVariables(args);

		System.out.println("Waiting to start receiving packets.");

		boolean isFirst = true; //To handle special case of first packet
		boolean receivedEOT = false;
		int expectSeqNum = 0;
		int total = 0;
		while(!receivedEOT)
		{
			packet pkt = MyUtil.receivePacket(socket);	
			arrivalWriter.println(pkt.getSeqNum());

			if(pkt.getSeqNum() == expectSeqNum) //Check if expecting
			{
				System.out.println("Received packet " + total);
				total++;
				
				if(pkt.getType() == 2) //Is EOT packet
				{
					System.out.println("Received EOT packet. Quitting.");
					receivedEOT = true;
				}

				sendAck(pkt.getSeqNum(), receivedEOT);
				dataWriter.write(new String(pkt.getData()));
				
				isFirst = false;
				expectSeqNum = (expectSeqNum + 1) % 32;
			}
			else
			{
				//If it isn't the first packet, resend the ACK
				//for the last received pacet.
				if(!isFirst)
				{
					sendAck((expectSeqNum - 1) % 32, false);
				}
			}
		}

		dataWriter.close();
		arrivalWriter.close();
	}
	
	private static void sendAck(int seqNum, boolean isEOT) throws Exception
	{
		packet pkt;
		if(isEOT)
		{
			pkt = packet.createEOT(seqNum);
		}
		else
		{
			pkt = packet.createACK(seqNum);
		}

		MyUtil.sendPacket(pkt, socket, emulatorInetAddress, outgoingPort);
	}
}



