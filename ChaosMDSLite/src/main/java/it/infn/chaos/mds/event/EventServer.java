package it.infn.chaos.mds.event;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketException;

abstract public class EventServer implements Runnable {

	private boolean						run					= false;
	private int							port				= 0;

	public void init(int port) throws IOException {
		run = true;
		this.port = port;
	}

	abstract void consumeEventData(byte[] eventData, int dataLength);

	@Override
	public void run() {
		DatagramSocket serverSocket;
		try {
			serverSocket = new DatagramSocket(port);
			byte[] receiveData = new byte[1024];
			while (run) {
				try {
					DatagramPacket receivePacket = new DatagramPacket(receiveData, receiveData.length);

					serverSocket.receive(receivePacket);
					
					consumeEventData(receivePacket.getData(), receivePacket.getLength());
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		} catch (SocketException e) {
			e.printStackTrace();
		}

	}
}
