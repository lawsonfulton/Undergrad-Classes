import java.io.*;
import java.net.*;
import java.util.*;
import java.nio.charset.Charset;
import java.util.concurrent.atomic.*;
import java.util.Timer;

public class sender
{
	//Global Constants
	static String emulatorAddress;
	static int outgoingPort;      
	static int incomingPort;           
	static String fileName;       
	static String fileContents;  
	static public ArrayList<String> dataChunks;
	static InetAddress emulatorInetAddress;

	static final int windowSize = 10; 
	static final long timeout = 200; //timer expiry period in millis

	static DatagramSocket socket;
	static PrintWriter seqNumWriter;

	//Shared state between sending and receiving threads
	class Shared
	{
		public volatile boolean timerStarted = false;
		public volatile boolean wasSignalled = false;
		public volatile int lastConfirmedChunk = -1;
		public volatile int lastSentChunk = -1;

		public Timer timer = new Timer();

		public synchronized void restartTimer()
		{
			timerStarted = true;
			timer.cancel();
			timer.purge();
			timer = new Timer();
			timer.schedule(new ResendTask(), timeout, timeout);
		}

		public synchronized void stopTimer()
		{
			timerStarted = false;
			timer.cancel();
			timer.purge();
		}

		public synchronized boolean isTimerRunning()
		{
			return timerStarted;
		}
	}

	final Shared shared = new Shared();

	//Thread for sending packets the first time in order.
	public class SenderTask implements Runnable {
		//Waits for the receiving thread to notify that there is room in
		//the sliding window
		private void waitForRoom()
		{
		    synchronized(shared)
		    {
		    	if(isWindowFull())
				{
					waitForSignal();
			    }
		    }
	    }

	    private boolean isWindowFull()
	    {
	    	return shared.lastSentChunk - shared.lastConfirmedChunk >= windowSize;
	    }

	    private void waitForSignal()
	    {
	    	shared.wasSignalled = false;
	    	while(!shared.wasSignalled)
	    	{
	        	try{
	        		shared.wait();
	         	} catch(InterruptedException e){}
			}
	     	shared.wasSignalled = false;
	    }

	    //Start of thread execution. Sends all packets once and then EOT.
		@Override
		public void run()
		{
			System.out.println("Sending the data in " + dataChunks.size() + " chunks.");
			try
			{
				while(dataStillLeft())
				{
					waitForRoom(); //Don't send a packet unless room in window

					synchronized(shared)
					{
						sendNextPacket();
						if(!shared.isTimerRunning())
						{
							shared.restartTimer();
						}
					}
					
				}
			} 
			catch (Exception e)
			{
				e.printStackTrace();
				System.out.println("There was an error.");
				System.exit(1);
			}

			System.out.println("Done sending all packets once. Waiting for ACKs.");
		}

		private boolean dataStillLeft()
		{
			return shared.lastSentChunk + 1 < dataChunks.size();
		}

		//Does the work of transmitting a packet for the first time only
		private void sendNextPacket() throws Exception
		{
			synchronized(shared)
			{
				shared.lastSentChunk++;

				packet pkt;
				int seqnum = shared.lastSentChunk % 32;
				pkt = packet.createPacket(seqnum, dataChunks.get(shared.lastSentChunk));

				MyUtil.sendPacket(pkt, socket, emulatorInetAddress, outgoingPort);
				recordSeqNum(seqnum);
			}

			System.out.println("Sent packet " + shared.lastSentChunk);
		}

	} 

	//A task that will get executed regularly whenever the timer expires
	//Resends any packets that have been sent at least once, but have not yet
	//received and ACK.
	class ResendTask extends TimerTask
	{	
        public void run()
        {
        	try
        	{
	        	synchronized(shared)
	        	{
		        	for (int i = shared.lastConfirmedChunk + 1; i <= shared.lastSentChunk; i++) 
		        	{
		        		packet pkt;
		        		if (i == dataChunks.size()) // Is EOT
		        		{
		        			pkt = packet.createEOT(i % 32);
		        		}
		        		else
		        		{
		        			pkt = packet.createPacket(i % 32, dataChunks.get(i));
		        		}

						MyUtil.sendPacket(pkt, socket, emulatorInetAddress, outgoingPort);
						recordSeqNum(pkt.getSeqNum());
		        	}
		        }
		     }
		     catch (Exception e)
		     {
		     	System.out.println("Error: Uhoh, something went wrong.");
		     	e.printStackTrace();
		     	System.exit(1);
		     }
        }
    }
	
	//Thread dedicated to receiving ACKs for sent packets.
	public class ReceivingTask implements Runnable {
		public PrintWriter ackWriter;

		public ReceivingTask() throws Exception
		{
			ackWriter = new PrintWriter("ack.log", "UTF-8");
		}

		//If there is room in the window, wakeup the sending thread
		private void notifyIfRoom()
		{
			synchronized(shared)
			{
				if(shared.lastSentChunk - shared.lastConfirmedChunk < windowSize)
				{
					shared.wasSignalled = true;
					shared.notify();
				}
			}
		}

		@Override
		public void run()
		{
			try
			{
				while(true)
				{
					notifyIfRoom();

					packet pkt = MyUtil.receivePacket(socket);
					ackWriter.println(pkt.getSeqNum());

					//Got an ACK for the EOT packet
					if(pkt.getType() == 2)
					{
						System.out.println("Received EOT ACK. Quitting.");
						shared.stopTimer();
						break;
					}

					synchronized(shared)
					{
						int lastSeqNum = shared.lastConfirmedChunk % 32;
						int newSeqNum = pkt.getSeqNum();
						
						//Java has a stupid modulus operator
						//This means that we are adding the difference mod 32.
						shared.lastConfirmedChunk += ((newSeqNum - lastSeqNum) % 32 + 32) % 32;

						updateTimer();

						if(shared.lastConfirmedChunk == shared.lastSentChunk)
						{
							System.out.println("Received all ACKs. Sending EOT packet.");
							sendEOT((shared.lastConfirmedChunk + 1)%32);
						}
					}
				}
			}
			catch(Exception e)
			{
				System.out.println("Oh boy, not again...");
				e.printStackTrace();
				System.exit(1);
			}
			finally
			{
				ackWriter.close();
				seqNumWriter.close();
			}
		}

		private void sendEOT(int seqNum) throws Exception
		{
			packet pkt = packet.createEOT(seqNum);
			MyUtil.sendPacket(pkt, socket, emulatorInetAddress, outgoingPort);

			recordSeqNum(seqNum);
		}

		private void updateTimer()
		{
			if(shared.lastConfirmedChunk == shared.lastSentChunk)
			{
				shared.stopTimer();
			}
			else
			{
				shared.restartTimer();
			}
		}
	} 

	//Create the threads and initiate transmission.
	private void startNback() throws Exception
	{
		SenderTask senderTask = new SenderTask();
		ReceivingTask receivingTask = new ReceivingTask();

		Thread sendThread = new Thread(senderTask);
		Thread recvThread = new Thread(receivingTask);
		sendThread.start();
		recvThread.start();
	}

	public static void main(String[] args) throws Exception 
	{
		MyUtil.validateArguments(args);
		
		emulatorAddress = args[0];
		outgoingPort = Integer.parseInt(args[1]);
		incomingPort = Integer.parseInt(args[2]);		
		fileName = args[3];

		try
		{
			fileContents = MyUtil.loadFile(fileName, Charset.defaultCharset());
		}
		catch (FileNotFoundException e)
		{
			System.out.println("Couldn't find the input file");
			e.printStackTrace();
			System.exit(1);
		}

		dataChunks = MyUtil.splitInChunks(fileContents, 500);

		emulatorInetAddress = InetAddress.getByName(emulatorAddress); 
		socket = new DatagramSocket(incomingPort);

		seqNumWriter = new PrintWriter("seqnum.log", "UTF-8");

		sender s = new sender();
		s.startNback();
	}

	public static void recordSeqNum(int seqNum)
	{
		seqNumWriter.println(seqNum);	
	}
}
